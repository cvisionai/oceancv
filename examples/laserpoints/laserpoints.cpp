#include <random>

#include "opencv2/opencv.hpp"

#include "oceancv/uwi/laser.h"
#include "oceancv/uwi/delphi.h"
#include "oceancv/util/file_parser.h"

using namespace std;

int main() {

	// Set paths to image data and annotation files
	std::string biigle_csv_annotation_report_file_path = "/home/tschoening/Desktop/SO268_OFOS_ImageCuration/283_full_annotation_report.csv";
	std::string image_folder = "/media/tschoening/SO268_OFOS/SO268-1_021-1_OFOS-02/SONNE_CAM-01_OFOS-Still/raw/";

	// Read annotation file and prepare the coordinates for DeLPHI
	std::map<std::string,std::map<std::string,std::string>> columns = ocv::getASCIFileColumnsAsString(biigle_csv_annotation_report_file_path,"annotation id",{"labels","image filename","annotation shape","x/radius","y"});
	std::map<std::string,std::vector<cv::Point>> tmp_lp_coordinates = {};
	std::vector<std::string> image_paths = {};
	std::vector<std::vector<cv::Point>> lp_coordinates = {};
	for(auto p : columns["image filename"]) {
		if(columns["labels"][p.first] != "DIAS > Laser point")
			continue;
		if(columns["annotation shape"][p.first] != "Point")
			continue;
		if(tmp_lp_coordinates.find(p.second) == tmp_lp_coordinates.end())
			tmp_lp_coordinates[p.second] = {};
		tmp_lp_coordinates[p.second].push_back(cv::Point(std::stoi(columns["x/radius"][p.first]),std::stoi(columns["y"][p.first])));
	}
	for(auto p : tmp_lp_coordinates) {
		if(p.second.size() > 0) {
			image_paths.push_back(image_folder + p.first);
			lp_coordinates.push_back(p.second);
		}
	}

	// Train DeLPHI
	ocv::DeLPHI delphi(image_paths, lp_coordinates);

	std::cout << "Have " << delphi.getCuratedColors().size() << " curated color vectors for matching." << std::endl;

	// Test I/O of trained model
	//delphi.writeModel("/home/tschoening/Desktop/DELPHI/","writetest");
	//ocv::DeLPHI d2("/home/tschoening/Desktop/DELPHI/writetest");

	std::vector<std::string> apply_img_paths = {"SO268-1_021-1_OFOS-02_20190304_102150.JPG","SO268-1_021-1_OFOS-02_20190304_162040.JPG","SO268-1_021-1_OFOS-02_20190304_084114.JPG"};

	cv::cuda::GpuMat c_mask;
	c_mask.upload(delphi.getMask());

	// Apply DeLPHI to one image
	if(delphi.isTrained()) {

		std::vector<cv::Point> lps;

		for(std::string img_path : apply_img_paths) {

			ocv::forceFileExists(image_folder + img_path);
			cv::Mat img = cv::imread(image_folder + img_path,cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION );

			lps = ocv::cuda::applyDeLPHI(delphi,img,c_mask);

			if(lps.size() > 0) {

				std::cout << img_path << " Found " << lps.size() << " lps" << std::endl;

				// Assume laser points are equally distanced to derive pixel-to-meter scale factor
				ocv::Aggregation<float> px_per_sqm = ocv::laserpoints::getPixelsPerMeterSonneOFOS(lps, 0.4, 0.4);

				// Compute image area in square meters
				float img_area = ocv::laserpoints::getImageAreaInSquareMeters(img.size(), px_per_sqm.val());

				std::cout << "Px/m2: " << px_per_sqm.val() << " image area [m^2]: " << img_area << " error-margin: " << img_area * px_per_sqm.dev() << " no. measurements: " << px_per_sqm.num() << std::endl;

				for(int i = 0; i < lps.size(); i++) {
					cv::circle(img,lps[i],25,cv::Vec3b(255,255,255));
				}
				cv::imwrite("/home/tschoening/Desktop/DELPHI/" + img_path,img);

			}

		}

	}

}
