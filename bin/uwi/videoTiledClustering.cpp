#include "opencv2/opencv.hpp"
#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafilters.hpp"

#include "oceancv/util/cli_args.h"
#include "oceancv/util/file_parser.h"
#include "oceancv/cudaimg/fspice_normalization.h"

#include "oceancv/ml/mat_pair.h"

#include "MPEG7FexLib/Feature.h"

using namespace std;

/*float calcEntropy(const vector<size_t>& hist) {
	size_t sum = 0;
	for(size_t v : hist)
		sum += v;
	float e = 0.0;
	for(size_t v : hist) {
		if(v != 0)
			e -= 1.0 * v / sum * log2(1.0 * v / sum);
	}
	return e;
}*/


int main(int argc, char** argv) {

	// Parse config parameter
	ocv::cli_args args(argc,argv,{{"i","Input video"},{"o","Output path"},{"step","Step width between tiles. If negative then it will be set to the tile width","-1"},{"tile","Tile size in pixels (width and height -> tiles are squares). If negative, the whole image will be seen as one tile","-1"},{"skip_seconds","How many seconds of the video to skip before starting the analysis","0"},{"total_seconds","How many seconds to run the analysis","-1"},{"scale","Whether to scale the image to uniform square meters","-1"},{"area_file","Full path to the file containing the image footprints for this video",""}});

	// Check if video file exists
	ocv::forceFileExists(args["i"]);

	// Try to open video file
	cv::VideoCapture vid_capture(args["i"]);
	if(!vid_capture.isOpened()) {
		cout << "Could not open video " << args["i"] << endl;
		return -1;
	}


	// The various images, channels, intermediates, for CPU and GPU
	cv::Mat cut, cut_scaled, frame_Lab, mask,mask_dil,gui_frame, frame, frame_a, frame_b, frame_a_f, frame_b_f, location, tmp1, tmp2;
	cv::cuda::GpuMat change_g, green_g, frame_g, tmp1_g, tmp2_g, tmp3_g, frame_a_fg, frame_b_fg, img_L, img_a, img_b, img_La, img_Lab;
	vector<cv::Mat> frames_a,frames_b;
	vector<cv::cuda::GpuMat> channels_g;

	// Required for contour detection
	vector<cv::Vec4i> hierarchy;
	vector<vector<cv::Point>> contours;

	// Init CUDA
	cout << "pre CUDA" << endl;
	cv::cuda::DeviceInfo di;
	di.totalMemory();

	cout << "have CUDA" << endl;

	// 3x3 pixel erosion filter
	cv::Ptr<cv::cuda::Filter> erode = cv::cuda::createMorphologyFilter(cv::MORPH_ERODE,CV_8UC1,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));
	cv::Ptr<cv::cuda::Filter> dilate = cv::cuda::createMorphologyFilter(cv::MORPH_DILATE,CV_8UC1,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5)));


	// Superpixel data structures
//	int step = args.i("sp_step");
//	int weight = args.i("sp_weight");

	/*mali::Image<float>* m_image;
	mali::LayerStack<float> m_layer_stack;
	mali::Mat<float> tmp_mat;
	Superpixel<float>* sup;
	*/

	// Iterate over video frames
	cv::Vec3b col;
	cv::Vec3f col_f, dom_col, med_col;
//	double min_dist,fac_f;
	//size_t dom_col_num;

//	float median_ent, mean_ent, var_ent, size_fac, entropy;
	vector<float> contour_areas;
	vector<size_t> color_histogram;
	uint seconds = 0;

	//int bins = 8;
//	int color_bins[bins][bins][bins] = {0};

	//int bin_width = 256 / bins;

	//int use_doms = 2;

	int sx = 100;
	int ex = 1800;
	int sy = 300;
	int ey = 900;

	int step = args.i("step");
	int tile_w = args.i("tile");
	int tile_h = args.i("tile");

	int tsx, tsy;//, tex, tey;//, L, a, b;

//	int tile_width = 200;
//	int tile_height = 200;

	int tiles_x = (ex - sx) / step;
	int tiles_y = (ey - sy) / step;
	//int c, ndc;

	if(args.i("tile") < 0) {
		tiles_x = 1;
		tiles_y = 1;
		tile_w = ex - sx;
		tile_h = ey - sy;
	}

	cout << "Tiles: " << tiles_x << " " << tiles_y << endl;
	cout << "Sizes: " << tile_w << " " << tile_h << endl;
	//int num_dc = 3;
	int vec_size = 32;//1 + num_dc * 4 + num_dc * 3;

//	vector<float> entropies;
//	vector<pair<int,int>> dom_col_idcs;
//	vector<cv::Vec3f> dom_cols;

	//cout << "Second,MeanEntropy,MedianEntropy,EntropyVariance,";
	//for(int j = 0; j < use_doms; j++)
	//	cout << "DomCol" << j << "L,DomCol" << j << "a,DomCol" << j << "b,";
	//cout << endl;

	//mali::MetaMat<float> features;
	//mali::Vector<float> vec(vec_size),out(3);

	vector<cv::Mat_<float>> inputs, outputs;

	cv::Mat_<float> vec(1,vec_size);
	cv::Mat_<float> out(1,3);



	Frame* mp7_frame;
	XM::ColorStructureDescriptor* csd;
	//XM::DominantColorDescriptor* dcd;
	//XM::DOMCOL* domcol;

	//int file_part = 0;
	int skip_seconds = args.i("skip_seconds");
	int total_seconds = args.i("total_seconds");

	// Skip frames to jump to start position
	for(int f = 0; f < skip_seconds; f++) {
		cout << "s" << f << " " << flush;
		for(int f2 = 0; f2 < 25; f2++)
			vid_capture.grab();
	}

	bool use;
	float area, scale_fac = 0.0;

	// Eventually get areas for this video
	map<int,double> areas = {};

	cout << "pre scale" << endl;

	if(args.f("scale") > 0) {

		ocv::forceFileExists(args.s("area_file"));

		// Load log file
		map<string,string> /*vid_frame,vid_video,*/vid_area;
		//vector<string> seps = ocv::FileParser::guessSeparators(log_file);
		//vector<string> header = ocv::FileParser::getHeader(log_file);

		cout << "scaling" << endl;

		std::map<std::string,std::map<std::string,std::string>> data = ocv::getASCIFileColumnsAsString(args.s("area_file"),"VideoFrameNumber",{"VideoFrameArea"});

			cout << "bar" << endl;

//		vid_frame = data["VideoFrameNumber"];
//		vid_video = data["VideoFilename"];
		vid_area = data["VideoFrameArea"];

		cout << /*vid_frame.size() << " " << vid_video.size() << " " <<*/ vid_area.size() << endl;

		if(/*vid_frame.size() == 0 || vid_video.size() == 0 ||*/ vid_area.size() == 0) {
			cout << "Could not find scale data" << endl;
			exit(0);
		}

		for(pair<string,string> p : vid_area) {

			// Check if this value belongs to the current video
//			if(args.s("i").find(p.second) == string::npos)
//				continue;
			if(vid_area[p.first] == "")
				continue;
			areas.insert(pair<int,double>(stoi(p.first),stof(p.second)));

		}

		if(areas.size() == 0) {
			cout << "No areas found for this video, can't scale the frames to uniform footprint. Quit." << endl;
			exit(0);
		}

	}


	cout << "Pre while" << endl;

	while(true) {

		use = true;

		// Try to fetch a frame
		if(!vid_capture.read(frame_a))
			break;

		// Write original frame
		//cv::imwrite("/vol/data/images/Condor_Video/src/H013_NINJA2_S080_S001_T003_" + to_string(seconds) + ".png",frame_a,{CV_IMWRITE_PNG_COMPRESSION,9});

		// Skip the next 24 frames to get to one second intervals
		for(int f = 0; f < vid_capture.get(cv::CAP_PROP_FPS)-1; f++)
			vid_capture.grab();

		cout << seconds + skip_seconds << " " << flush;

		if(args.f("scale") > 0) {

			//cout << areas[seconds] << " " << flush;
			if(areas[seconds] <= 0)
				use = false;
			area = areas[seconds];
			scale_fac = args.f("scale") / area;

			//cout << scale_fac << " -- " << flush;

			if(scale_fac < 0.5 || scale_fac > 2.0)
				use = false;
		}


		if(use) {

			// Start the GPU processing
			tmp1_g.upload(frame_a);

			// fSpice
			ocv::cuda::fspice(tmp1_g,tmp2_g,701);
			tmp2_g.download(frame_a);

			cv::imwrite(args["o"] + to_string(seconds) + ".png",frame_a,{cv::IMWRITE_PNG_COMPRESSION,9});



			for(int tx = 0; tx < tiles_x; tx++) {
				for(int ty = 0; ty < tiles_y; ty++) {

					tsx = sx + tx * step;
					tsy = sy + ty * step;

					// Limit to ROI
					cut = frame_a(cv::Rect(tsx,tsy,tile_w,tile_h));

					// Scale to unit size by LP measurement
					if(args.i("scale") > 0) {
						cv::resize(cut,cut_scaled,cv::Size(), scale_fac, scale_fac, cv::INTER_CUBIC);
						mp7_frame = new Frame(cut_scaled);

						//cout << args.f("scale") << " " << area << " " << scale_fac << cut_scaled.size() << endl;
						//exit(0);

					} else {
						mp7_frame = new Frame(cut);
					}

					//vec = mali::Vector<float>(vec_size,0);
					csd = Feature::getColorStructureD(mp7_frame, 32);
					for(size_t i = 0; i < csd->GetSize(); i++) {
						vec(i) = (int) csd->GetElement(i);
					}

					delete mp7_frame;
					delete csd;


					out(0) = seconds + skip_seconds;
					out(1) = tx;
					out(2) = ty;

					inputs.push_back(vec);
					outputs.push_back(out);

				}
			}

		} else {
			cout << "s" << seconds << " " << flush;
		}

		seconds++;
		if(total_seconds >= 0 && seconds > total_seconds)
			break;

	}

	cout << endl << inputs.size() << " " << outputs.size() << endl;

	// Write file to disk
	if(inputs.size() > 0) {
		cv::FileStorage file_i(args["o"] + ".ocvmpi", cv::FileStorage::WRITE);
		file_i << inputs;
		cv::FileStorage file_o(args["o"] + ".ocvmpo", cv::FileStorage::WRITE);
		file_o << outputs;
	}

}
