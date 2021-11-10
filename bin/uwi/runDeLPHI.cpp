 #include <fstream>

#include "opencv2/opencv.hpp"

#include "oceancv/util/cli_args.h"
#include "oceancv/util/cli_arg_provenance.h"
#include "oceancv/util/file_parser.h"
#include "oceancv/util/load_remote_image.h"
#include "oceancv/util/progress_bar.h"

#include "oceancv/uwi/delphi.h"
#include "oceancv/uwi/laser.h"

/**
 * Applies the DeLPHI algorithm by means of a traing model (use trainDeLPHI to create one) to a set of images.
 * See command line arguments for details on how to parameterize.
 */
int main(int argc, char** argv) {

	// Read command line arguments
	ocv::cli_args args = ocv::cli_args(argc,argv,{
			{"i","Input file of image paths"},
			{"d","Input file of stored DeLPHI model"},
			{"o","Output folder"},
			{"n","Output name"},
			{"lpm","Laser point detection mode (0 = getPixelsPerMeterEqualDistances, 1 = getPixelsPerMeterSonneOFOS)","0"},
			{"d1","Distance between lasers in meters"},
			{"d2","Optional second distance between lasers in meters","0.5"},
			{"mina","Min area to allow","0"},
			{"maxa","Max area to allow or -1 to ignore","-1"},
			{"img","Single image file name"," "}
		 });

	ocv::forceFileExists(args.s("i"));

	// Store Provenance
	ocv::cli_arg_provenance prov(args,args.s("o"));

	ocv::data_file_structure dfs(args.s("o"));
	std::string dst_folder = dfs.products();

	// Load DeLPHI model
	ocv::DeLPHI delphi(args.s("d"));
	if(!delphi.isTrained()) {
		std::cout << "DeLPHI model at " << args.s("i") << " is not trained. Exiting." << std::endl;
		return -1;
	}

	// Load image paths from file
	std::vector<std::vector<std::string>> img_paths = ocv::readASCIFile(args.s("i"));
	std::vector<std::string> file_names = {};
	for(int i = 0; i < img_paths.size(); i++)
		file_names.push_back(img_paths[i][0]);

	// Prepare output file
	std::ofstream res_acmw(dst_folder + args.s("n") + "_iacumaw.txt");

	res_acmw << "Image number/name\tImage area\tArea Error margin\tArea Measurements\tLP Coordinates" << std::endl;

	// Apply DeLPHI to one image
	std::vector<cv::Point> lps;
	std::string file_name, img_path;
	ocv::Aggregation<float> px_per_sqm;
	bool wrote;
	float var;
	cv::cuda::GpuMat c_mask;
	c_mask.upload(delphi.getMask());

	ocv::progress_bar pb(file_names);
	for(int i = 0; i < file_names.size(); i++) {

		pb.advance(i);

		img_path = file_names[i];
		file_name = ocv::fileName(img_path);

		if(args.s("img") != " " && args.s("img") != file_name)
			continue;

		ocv::forceFileExists(img_path);
		cv::Mat img = cv::imread(img_path,cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION );

		lps = ocv::cuda::applyDeLPHI(delphi,img,c_mask);

		wrote = false;
		if(lps.size() > 1) {

			// Assume laser points are equally distanced to derive pixel-to-meter scale factor

			switch(args.i("lpm")) {
				case 0:
					px_per_sqm = ocv::laserpoints::getPixelsPerMeterEqualDistances(lps,args.f("d1"));
				break;
				case 1:
					px_per_sqm = ocv::laserpoints::getPixelsPerMeterSonneOFOS(lps, args.f("d1"), args.f("d2"));
				break;
				default:
					std::cout << "Unknown area computation method. Exiting." << std::endl;
				break;
			}

			// Compute image area in square meters
			float img_area = ocv::laserpoints::getImageAreaInSquareMeters(img.size(), px_per_sqm.val());

			// Filter by meaningful sizes
			if(img_area > args.f("mina") && (img_area < args.f("maxa") || args.f("maxa") < 0)) {
				if(px_per_sqm.dev() < std::numeric_limits<float>::min())
					var = 0;
				else
					var = img_area * px_per_sqm.dev();
				res_acmw << file_name << "\t" << img_area << "\t" << var << "\t" << px_per_sqm.num() << "\t{";
				for(int i = 0; i < lps.size(); i++)
					res_acmw << "{'x':" << lps[i].x << ",'y':" << lps[i].y << "},";
				res_acmw << "}" << std::endl;
				wrote = true;
			}

		}

		if(!wrote) {
			res_acmw << file_name << "\t\t\t\t" << std::endl;
		}

	}

	std::cout << "done." << std::endl;

}
