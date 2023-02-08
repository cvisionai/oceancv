#include "opencv2/opencv.hpp"

#include "oceancv/util/cli_args.h"
#include "oceancv/util/cli_arg_provenance.h"
#include "oceancv/util/file_parser.h"
#include "oceancv/util/load_remote_image.h"

#include "oceancv/uwi/comonod.h"

using namespace std;

/**
* @author: Timm Schoening
* @date: 2016-12-15
* If condition is true, couts the message and returns the condition.
* Just to make plenty of sanity checks with debug outputs more compact.
**/
bool checkConditionWithMessage(bool condition,string message) {
	if(condition)
		cout << message << flush;
	return condition;
}

/**
 * Executes the CoMoNoD algorithm on a set of images. See command line arguments for details on how to parameterize.
 */
int main(int argc, char** argv) {

	// TODO: Make it operable without scaling

	// Read command line arguments
	ocv::cli_args args = ocv::cli_args(argc,argv,{

			{"i","Input file with list of full image paths"},
			{"o","Output folder"},

			{"ats","Adaptive threshold slope (not the thresh but the max slope change of the histogram left of the peak","0.1"},
			//{"min_npr","Minimum nodule pixel radius","5"},
			{"fspice_sigma","fSpice sigma","701"},
			{"sx","Start coordinate (x)","0"},{"sy","Start coordinate (y)","0"},{"ex","End coordinate (x)","-1"},{"ey","End coordinate (y)","-1"},
			{"min_nodule_size","Minimum size for a nodule to be detected (in square centimeters)","0"},
			{"max_nodule_size","Maximum size for a nodule to be detected (in square centimeters, default of -1 means no limit)","-1"},

			{"meta_file","Files with image meta data"},
			{"meta_img_col","Name of the log file column containing the image names","Image_filename"},
			{"meta_area_col","Name of the log file column containing the image area in m^2","Image_area"},
			//{"meta_lat_col","Name of the log file column containing the latitude value","ADCP_latitude"},
			//{"meta_lon_col","Name of the log file column containing the longitude value","ADCP_longitude"},
			{"meta_good_col","Name of the log file column containing the flag for good image quality (all rows where value==0 will be skipped)","Image_seafloorvisible"},
			//{"meta_area_factor","With what to scale the squared altitude to compute image area (DSC:3, AutoSub:0.19010992)","3"},

			{"target_area","Area in qm to scale all images to (if -1 it will be taken as the median of the available images)","-1"},

			{"continue","Continue failed run. Will move the results file and check if a single image file exists, then skip","1"},
			{"remote_images","Whether files shall be loaded through a URL rather than from the local file system","0"},

			{"store_sizes","Set to 1 if each individual nodule size shall be stored in an accompanying log file","0"},
			{"store_nth_result","Whether the n-th intermediate result images shall be stored","0"},
			{"skip_images","Whether images shall be skipped after processing one image, and if so: how many","0"},

		});

	// Convert args to variables
	string input_list = args.s("i"), dst_folder = args.s("o"), meta_file = args.s("meta_file");

	cv::Point top_left = cv::Point(args.i("sx"),args.i("sy"));
	cv::Point bottom_right = cv::Point(args.i("ex"),args.i("ey"));

	float adaptive_threshold_slope = args.f("ats");//, area_factor = args.f("meta_area_factor");
	float min_nodule_size = args.f("min_nodule_size");
	float max_nodule_size = args.f("max_nodule_size");
	float target_area = args.f("target_area");

	size_t skip = args.i("skip_images");

	// We assume that everything below this value is noise
	uint min_nodule_pixel_radius;// = args.i("min_npr");


	uint count = 0, threshold_used;
	double img_area, scale_fac, cm_per_pix;
	float max_ellipse_distortion = -1;
	float max_blob_distortion = -1;
	string file_name;
	vector<string> files;
	vector<double> orig_target_values = {0.01,0.10,0.25,0.50,0.75,0.90,0.99}, nod_size_bins = {0,0.01,0.25,0.5,0.75,0.99};
	uint nod_dir_hist_bins = 8;

	// Prepare OpenCV objects
	cv::Mat input,binary,blob_index;
	vector<vector<cv::Point>> contours;
	vector<ocv::CoMoNoDule> nodules = {};


	// Load input file name list
	ocv::forceFileExists(input_list);
	vector<vector<string>> input_image_files = ocv::readASCIFile(input_list);
  cout << "LISTING FILES" << endl;
	for(vector<string> input_file : input_image_files) {
      for(string name : input_file) {
          cout << name << endl;
      }
	}
  cout << "FILES LISTED" << endl;

	// Load meta data
	ocv::forceFileExists(meta_file);
	map<string,double> areas, quality;//latitudes, longitudes, quality;
	map<string,map<string,double>> data = ocv::getASCIFileColumnsAsDouble(meta_file,args.s("meta_img_col"),{args.s("meta_area_col"),args.s("meta_good_col")}); // ,args.s("meta_lat_col"),args.s("meta_lon_col")
	areas = data[args.s("meta_area_col")];
	//latitudes = data[args.s("meta_lat_col")];
	//longitudes = data[args.s("meta_lon_col")];
	quality = data[args.s("meta_good_col")];

	// Do not overwrite old result file, rename it with an increased running index appended
	if(args.i("continue") == 1 && ocv::fileExists(dst_folder + "CoMoNoD_result.csv")) {
		int i = 0;
		while(true) {
			i++;
			if(!ocv::fileExists(dst_folder + "CoMoNoD_result" + to_string(i) + ".csv")) {
				rename((dst_folder + "CoMoNoD_result.csv").c_str(),(dst_folder + "CoMoNoD_result" + to_string(i) + ".csv").c_str());
				break;
			}
		}
	}

	// Create result folder
	if(!ocv::fileExists(dst_folder))
		mkdir(dst_folder.c_str(), 0775);

	// Store Provenance
	ocv::cli_arg_provenance prov(args,dst_folder);

	// Result files: entire analysis, detailed per image, tiny per image
	ofstream results, single_nodule_results, tiny_nodule_results;
	results.precision(10);
	cout.precision(10);

	// The result file for the entire image set (single nodule results fused to bins)
	results.open(dst_folder + "CoMoNoD_result.csv");
	results << "Image Filename,Image Area [m^2],ScaleFactor,Threshold used,Resolution [cm^2/px],Cut Area [m^2],#Nodules,Coverage[%],";
	for(uint i = 0; i < nod_size_bins.size(); i++)
		results << "size:" << 100*nod_size_bins[i] << "%[cm^2],";
	results << "size:max[cm^2],";
	for(uint i = 0; i < orig_target_values.size(); i++)
		results << "coverage:" << 100 * orig_target_values[i] << "%[cm^2],";
	for(uint i = 0; i < 4; i++)
		results << "particle-size val " << i << ",";
	for(uint i = 0; i < nod_dir_hist_bins; i++)
		results << "nod-direction bin " << i << ",";
	results << endl;

	// The header for the single nodule result files
	string single_nodule_result_header = "Nodule cx,Nodule cy,Nodule width [px], Nodule height [px], Nodule Size [px],Nodule Size [cm^2],Nodule angle [°],\n";


	// Compute median image area if it has not been set on the command line
	if(target_area < 0) {

		vector<float> tmp_areas;
    cout << "starting loop" << endl;
		for(vector<string> input_file : input_image_files) {
			cout << "in loop" << endl;

			file_name = input_file[0].substr(input_file[0].find_last_of('/')+1);
			cout << "FILE_NAME: " << file_name << endl;

			// Check if this image has environment parameters (lat,lon,...) and those are valid (e.g. altitude > 0)
			if(checkConditionWithMessage(args.i("continue") == 1 && (ocv::fileExists(dst_folder + file_name + "_single_nodules.txt") || ocv::fileExists(dst_folder + file_name + "_nodules.txt")),""))
			  cout << "file" << endl;
				continue;
			if(checkConditionWithMessage(areas.find(file_name) == areas.end(),""))
			  cout << "areas0" << endl;
				continue;
			//if(checkConditionWithMessage(latitudes.find(file_name) == latitudes.end(),""))
			//	continue;
			//if(checkConditionWithMessage(longitudes.find(file_name) == longitudes.end(),""))
			//	continue;
			if(checkConditionWithMessage(quality.find(file_name) == quality.end(),""))
			  cout << "quality0" << endl;
				continue;
			if(checkConditionWithMessage(quality[file_name] == 0,""))
			  cout << "quality1" << endl;
				continue;
			if(checkConditionWithMessage(areas[file_name] == 0,""))
			  cout << "areas1" << endl;
				continue;
			//if(checkConditionWithMessage(areas[file_name] > 10,""))
			//	continue;

			cout << "PUSHING BACK: " << areas[file_name] << endl;
			tmp_areas.push_back(areas[file_name]);//area_factor * altitudes[file_name] * altitudes[file_name]);

		}
    cout << "past loop" << endl;

		if(tmp_areas.size() == 0) {
			cout << "No files found while computing the median image area." << endl;
			exit(0);
		}

		// Pick median value of all areas
		sort(tmp_areas.begin(),tmp_areas.end());
		target_area = tmp_areas[0.5*tmp_areas.size()];

		cout << "Computed median image area as: " << target_area << " m^2" << endl;

	}

	// A cuda filter pointer to erode / dilate with a 3x3 kernel
	cv::Ptr<cv::cuda::Filter> erode = cv::cuda::createMorphologyFilter(cv::MORPH_ERODE,CV_8UC1,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));
	cv::Ptr<cv::cuda::Filter> dilate = cv::cuda::createMorphologyFilter(cv::MORPH_DILATE,CV_8UC1,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));
	//std::vector<cv::Point> search_neighbors = ocv::createSearchNeighborList(min_nodule_pixel_radius*2);

	for(vector<string> input_file : input_image_files) {

		file_name = input_file[0].substr(input_file[0].find_last_of('/')+1);
		cout << count++ << " " << flush;

		// Check if this image has environment parameters (lat,lon,...) and those are valid (e.g. altitude > 0)
		if(checkConditionWithMessage(args.i("continue") == 1 && (ocv::fileExists(dst_folder + file_name + "_single_nodules.txt") || ocv::fileExists(dst_folder + file_name + "_nodules.txt")),"-xst "))
			continue;
		if(checkConditionWithMessage(areas.find(file_name) == areas.end(),"no area "))
			continue;
		//if(checkConditionWithMessage(latitudes.find(file_name) == latitudes.end(),"-lat "))
		//	continue;
		//if(checkConditionWithMessage(longitudes.find(file_name) == longitudes.end(),"-lon "))
		//	continue;
		if(checkConditionWithMessage(quality.find(file_name) == quality.end(),"no quality "))
			continue;
		if(checkConditionWithMessage(quality[file_name] == 0,"bad quality "))
			continue;
		if(checkConditionWithMessage(areas[file_name] == 0,"bad area "))
			continue;

		if(skip > 0) {
			skip--;
			continue;
		}

		skip = args.i("skip_images");

		// The area of the entire image in m^2
		img_area = areas[file_name];//area_factor * altitudes[file_name] * altitudes[file_name];

		// The scaling factor to make all images in the set have the same px2cm ratio
		scale_fac = sqrt(target_area / img_area);

		// If the scale_fac is too far off the original pixel size, this could lead to memory issues otherwise
		if(checkConditionWithMessage(scale_fac < 0.0 || scale_fac > 2.1,"bad scaling "))
			continue;

		// Load and check input image
		if(args.i("remote_images") == 1) {
			if(!ocv::loadFromUrl(input_file[0],input)) {
				cout << "Could not load image: " << input_file[0] << " from URL. Quit." << endl;
				exit(0);
			}
		} else {
			input = cv::imread(input_file[0]);
		}

		if(checkConditionWithMessage(input.cols < 1 || input.rows < 1,"empty image "))
			continue;
		if(checkConditionWithMessage(input.cols * input.rows * scale_fac * scale_fac > 30000000,"area too large "))
			continue;

		// Get size of region to be processed
		bottom_right.x = ((bottom_right.x < 0) ? input.cols : min(input.cols,bottom_right.x));
		bottom_right.y = ((bottom_right.y < 0) ? input.rows : min(input.rows,bottom_right.y));

		// Compute real world scaling factor (cm_per_pix)
		cm_per_pix = 10000.0 * img_area / (input.cols * input.rows);

        // Compute minimum nodule radius in pixels
        min_nodule_pixel_radius = std::sqrt(min_nodule_size/3.14159/cm_per_pix);

        std::cout << "area:" << img_area << " pixradius:" << min_nodule_pixel_radius << " cm_per_pix:" << cm_per_pix << " minsize:" << min_nodule_size << std::endl;


		// Store meta data for this image in result file
		results << file_name << "," << 0.01*round(100*areas[file_name]) << "," /*latitudes[file_name] << "," << longitudes[file_name] << "," <<*/ << scale_fac << ",";

		if(args.i("store_nth_result") > 0 && count % args.i("store_nth_result") == 0)
			cv::imwrite(args.s("o") + file_name + "_0_input.jpg",input);

		// First CoMoNoD step
		threshold_used = ocv::contrastMaximization(file_name, input, top_left, bottom_right, adaptive_threshold_slope, scale_fac, args.i("fspice_sigma"), binary, blob_index, contours);

		if(args.i("store_nth_result") > 0 && count % args.i("store_nth_result") == 0)
			cv::imwrite(args.s("o") + file_name + "_1_binary.jpg",binary);

		results << threshold_used << "," << cm_per_pix << "," << 0.01*round(100*(bottom_right.x-top_left.x) * (bottom_right.y-top_left.y) * cm_per_pix / 10000) << "," << flush;

		// Second CoMoNoD step
		ocv::noduleDelineation(file_name,binary, blob_index, contours, min_nodule_pixel_radius, scale_fac, erode, dilate);//, search_neighbors);

std::cout << "done delineation" << std::endl;

		ocv::getSingleNodulesAndFilter(binary, nodules, min_nodule_pixel_radius, cm_per_pix, min_nodule_size, max_nodule_size, max_ellipse_distortion, max_blob_distortion);

std::cout << "done getsinglenodules" << std::endl;

		if(args.i("store_nth_result") > 0 && count % args.i("store_nth_result") == 0) {
			for(auto nod : nodules)
				cv::ellipse(input,nod,cv::Scalar(255,255,255));
			cv::imwrite(args.s("o") + file_name + "_2_det.jpg",input);
		}

		// Compute nodule statistics and write to file!!!!
		results << nodules.size() << "," << ocv::noduleCoverage(nodules, binary.size()) << ",";

		// Store nodule size and coverage bins
		if(nodules.size() > 0) {

			std::vector<double> tmp_res = ocv::noduleSizeBins(nodules, nod_size_bins);
			for(float val : tmp_res)
				results << val << ",";

			tmp_res = ocv::noduleCoverageBins(nodules, binary.size(), orig_target_values);
			for(float val : tmp_res)
				results << val << ",";

			tmp_res = ocv::particleSizeAnalysis(tmp_res);
			for(float val : tmp_res)
				results << val << ",";

			std::vector<uint> tmp_hist = ocv::noduleDirectionHistogram(nodules, nod_dir_hist_bins);
			for(float val : tmp_hist)
				results << val << ",";

		} else {
			for(int i = 0; i < nod_size_bins.size() + 1 + orig_target_values.size() + 4 + 8; i++)
				results << ",";
		}

		if(args.b("store_sizes")) {

			single_nodule_results.open(dst_folder + file_name + "_nodules.txt");
			single_nodule_results << single_nodule_result_header;
			tiny_nodule_results.open(dst_folder + file_name + "_nodule_sizes.txt");

			for(auto nodule : nodules) {
				single_nodule_results << nodule.center.x << "," << nodule.center.y << "," << nodule.size.width << "," << nodule.size.height << "," << nodule.px_area << "," << nodule.cm_area << "," << nodule.angle << "\n";
				tiny_nodule_results << nodule.cm_area << "\n";
			}

			single_nodule_results.close();
			tiny_nodule_results.close();

		}

		// Finalise result writing for this image
		results << endl;

	}

}
