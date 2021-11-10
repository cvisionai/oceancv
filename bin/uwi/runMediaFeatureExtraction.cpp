#include "opencv2/opencv.hpp"
#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafilters.hpp"

#include "oceancv/util/cli_args.h"
#include "oceancv/util/file_parser.h"
#include "oceancv/util/cli_arg_provenance.h"
#include "oceancv/util/progress_bar.h"

#include "oceancv/ml/mat_pair.h"
#include "oceancv/cudaimg/fspice_normalization.h"
#include "oceancv/cudaimg/image_entropy.h"
#include "MPEG7FexLib/Feature.h"

#include "oceancv/ml/feature_descriptor.h"
#include "oceancv/ml/color_descriptors.h"
#include "oceancv/ml/mpeg7_descriptors.h"
#include "oceancv/ml/metric.h"

using namespace std;

int main(int argc, char** argv) {

	// Parse config parameter
	ocv::cli_args args(argc,argv,{{"i","Path to video file or list file containing image paths"},
									{"o","Output path"},
								{"n","Result name"},
							{"t","Media type: img = images, vid = videos"},
						{"f","Features: all or basic","basic"},
					{"r","Reduce image resolution to fraction (0..1)","1"},
				{"p","Create proxy image (0/1)","1"}});

	// Check if media list file exists
	ocv::forceFileExists(args.s("i"));

	assert(args.s("t") == "img" || args.s("t") == "vid");

	// Store Provenance
	ocv::cli_arg_provenance prov(args,args.s("o"));

	ocv::data_file_structure dfs(args.s("o"));


	// Read files
	std::vector<std::string> file_list;


	std::string file_path, file_name, file_type, dst_folder;
	cv::Mat img, tile, gray;
	cv::cuda::GpuMat c_img, c_tmp, c_gray, c_row;
	cv::Scalar mean;
	double entropy;

	// Try to open video file
	cv::VideoCapture vid_capture;
	if(args.s("t") == "vid") {
		vid_capture.open(args.s("i"));
		if(!vid_capture.isOpened()) {
			cout << "Could not open video " << args.s("i") << endl;
			return -1;
		}
		// TODO Add frame idcs to file_list
		size_t num_seconds = vid_capture.get(cv::CAP_PROP_FRAME_COUNT) / vid_capture.get(cv::CAP_PROP_FPS);
		std::cout << "[MeFex] Video is " << num_seconds << " seconds long" << std::endl;
		file_list = std::vector<std::string>(num_seconds);
		for(int i = 0; i < num_seconds; i++) {
			file_list[i] = std::to_string(i);
		}
	} else {
		file_list = ocv::readASCIFileLines(args.s("i"));
		std::cout << "[MeFex] Processing " << file_list.size() << " files " << std::endl;
	}
	ocv::progress_bar pg(file_list);

	// TODO Prepare extractors
	std::map<std::string,ocv::FeatureDescriptor<float>*> descriptors;
	std::vector<std::string> desc_gray = {"HaralickTextureDescriptor","HomogeneousTextureDescriptor"};
	cv::FileStorage det_file;

	if(args.s("f") == "all") {
std::cout << "[MeFex] Creating Descriptors" << std::endl;

		descriptors.insert(std::pair<std::string,ocv::FeatureDescriptor<float>*>("ColorStatisticDescriptor",new ocv::ColorStatisticDescriptor<float>(3,true,true,true,true,true)));
		descriptors.insert(std::pair<std::string,ocv::FeatureDescriptor<float>*>("HistogramColorDescriptor",new ocv::HistogramColorDescriptor<float>(3,8)));
		descriptors.insert(std::pair<std::string,ocv::FeatureDescriptor<float>*>("HaralickTextureDescriptor",new ocv::HaralickTextureDescriptor<float>(16)));
		descriptors.insert(std::pair<std::string,ocv::FeatureDescriptor<float>*>("ColorLayoutDescriptor",new ocv::ColorLayoutDescriptor<float>()));
		descriptors.insert(std::pair<std::string,ocv::FeatureDescriptor<float>*>("ColorStructureDescriptor",new ocv::ColorStructureDescriptor<float>()));
		descriptors.insert(std::pair<std::string,ocv::FeatureDescriptor<float>*>("DominantColorDescriptor",new ocv::DominantColorDescriptor<float>()));
		descriptors.insert(std::pair<std::string,ocv::FeatureDescriptor<float>*>("EdgeHistogramDescriptor",new ocv::EdgeHistogramDescriptor<float>()));
		descriptors.insert(std::pair<std::string,ocv::FeatureDescriptor<float>*>("HomogeneousTextureDescriptor",new ocv::HomogeneousTextureDescriptor<float>()));
		descriptors.insert(std::pair<std::string,ocv::FeatureDescriptor<float>*>("ScalableColorDescriptor",new ocv::ScalableColorDescriptor<float>()));

std::cout << "[MeFex] Writing descriptor setup to protocol file" << std::endl;

		// Store setup of detailed vectors
		det_file.open(dfs.protocol()+args.s("n") + "_MeFex-feature-setup.json",cv::FileStorage::WRITE);
		vector<string> setup;
		det_file << "descriptors" << "[";
		int idx = 0;
		for(auto desc : descriptors) {
			setup = desc.second->setup();
			det_file << "{" << "index" << idx << "name" << desc.first << "features" << "[";
			for(int i = 0; i < desc.second->size(); i++)
				det_file << setup[i];
			det_file << "]" << "}";
		}
		det_file << "]";
		det_file.release();

		// Create rootpath for full descriptor result files
		dst_folder = dfs.products() + args.s("n") + "_MeFex/";
		if(!ocv::fileExists(dst_folder))
			mkdir(dst_folder.c_str(),0755);

	}

	std::cout << "[MeFex] Start media feature extraction" << std::endl;

	// Open master result file (color, entropy)
	std::ofstream res_file(dfs.products()+args.s("n") + "_MeFex-basic-features.txt");

	// Write header to basic feature file
	if(args.s("t") == "img")
		res_file << "Image number/name";
	else
		res_file << "Video second";
	res_file << "\tEntropy\tMean color R\tMean color G\tMean color B\n";


	cv::Mat_<float> vec;
	cv::Mat_<cv::Vec3b> proxy;

	for(int i = 0; i < file_list.size(); i++) {

		// Advance the CLI progress bar
		pg.advance(i);

		file_path = file_list[i];

		if(args.s("t") == "img") {
			ocv::forceFileExists(file_path);
			img = cv::imread(file_path,cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION);
		} else {
			// Read frame and skip the next n frames to get to one second intervals
			if(!vid_capture.read(img)) {
				std::cout << "[MeFex] ERROR: Could not read from video at second " << i << std::endl;
				break;
			}
			for(int f = 0; f < vid_capture.get(cv::CAP_PROP_FPS)-1; f++)
				vid_capture.grab();
		}

		file_name = ocv::fileName(file_path);
		file_type = ocv::fileType(file_name);

		// Maybe resize the image?
		if(args.f("r") < 1) {
			c_tmp.upload(img);
			c_img = cv::cuda::GpuMat(0,0,CV_8UC3);
			cv::cuda::resize(c_tmp,c_img,cv::Size(0,0),args.f("r"),args.f("r"),cv::INTER_CUBIC);
		} else {
			c_img.upload(img);
		}

		if(i == 0) {
			proxy = cv::Mat_<cv::Vec3b>(c_img.rows,file_list.size());
		}

		// Convert to grayscale for entropy computation
		cv::cuda::cvtColor(c_img,c_gray,cv::COLOR_BGR2GRAY);

		// Apply basic extractors: entropy, mean color
		entropy = ocv::cuda::entropy(c_gray);
		mean = cv::cuda::sum(c_img);
		mean /= (float) (c_img.cols * c_img.rows);
		res_file << file_name << "\t" << entropy << "\t" << mean[2] << "\t" << mean[1] << "\t" << mean[0] << std::endl;

		// Add mean color of each row in the image to proxy image (in the proxy image, each image from the dataset is represented by one column)
		if(args.s("p") == "1") {
			for(int j = 0; j < c_img.rows; j++) {
				c_row = c_img.row(j);
				mean = cv::cuda::sum(c_row);
				mean /= (float) c_img.cols;
				proxy(j,i) = cv::Vec3b(mean[0],mean[1],mean[2]);
			}
			cv::imwrite(dfs.products() + args.s("n") + "_MeFex-proxy.png",proxy);
		}

		// All other descriptors (mainly MPEG7), costly to compute!
		if(args.s("f") == "all") {

			// Open result file
			if(args.s("t") == "img") {
				det_file.open(dst_folder + ocv::replace("."+file_type,"_MeFex-extended-features.json",file_name),cv::FileStorage::WRITE);
			} else {
				det_file.open(dst_folder + "second_" + file_name + "_MeFex-extended-features.json",cv::FileStorage::WRITE);
			}
			det_file << "entropy" << entropy;
			det_file << "mean-color-RGB" << "[" << mean[2] << mean[1] << mean[0] << "]";

			if(args.f("r") < 1) {
				c_img.download(img);
				c_gray.download(gray);
			} else {
				cv::cvtColor(img,gray,cv::COLOR_BGR2GRAY);
			}

			// Apply complex extractors: color histogram, MPEG7, entropy in tiles
			for(auto desc : descriptors) {
				vec = cv::Mat_<float>(1,desc.second->size());
				if(std::find(desc_gray.begin(),desc_gray.end(),desc.first) == desc_gray.end())
					desc.second->extract(img,vec);
				else
					desc.second->extract(gray,vec);
				det_file << desc.first << "[";
				for(int j = 0; j < vec.cols; j++)
					det_file << vec(0,j);
				det_file << "]";
			}

		}

		det_file.release();

	}

	if(args.s("p") == "1") {
		cv::imwrite(dfs.products() + args.s("n") + "_MeFex-proxy.png",proxy);
	}
}
