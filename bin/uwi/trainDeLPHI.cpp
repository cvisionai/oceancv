#include "opencv2/opencv.hpp"

#include "oceancv/util/cli_args.h"
#include "oceancv/util/cli_arg_provenance.h"
#include "oceancv/util/file_parser.h"
#include "oceancv/util/load_remote_image.h"

#include "oceancv/uwi/delphi.h"
#include "oceancv/uwi/laser.h"

/**
 * Trains the DeLPHI algorithm based on a set of images and manual annotations (exported from BIIGLE).
 * See command line arguments for details on how to parameterize.
 */
int main(int argc, char** argv) {

	// Read command line arguments
	ocv::cli_args args = ocv::cli_args(argc,argv,{
			{"i","Input folder where the images reside"},
			{"a","Input file of laser point annotations (Full Annotation Export from BIIGLE!)"},
			{"o","Output folder"},
			{"n","Output name"},
			{"t1","Theta_1 of DeLPHI","25"},
			{"t2","Theta_2 of DeLPHI","3"},
			{"t3","Theta_3 of DeLPHI","0.75"},
			{"t4","Theta_4 of DeLPHI","10"}
		 });

	// Set paths to image data and annotation files
	std::string biigle_csv_annotation_report_file_path = args.s("a");
	std::string image_folder = args.s("i");

	ocv::forceFileExists(args.s("a"));
	ocv::forceFileExists(args.s("i"));


	// Create output folder following data structure convention
	ocv::data_file_structure dfs(args.s("o"));
	std::string dst_folder = dfs.products();

	// Store provenance
	ocv::cli_arg_provenance prov(args,args.s("o"));



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
	ocv::DeLPHI delphi(image_paths, lp_coordinates, args.i("t1"), args.i("t2"), args.f("t3"), args.i("t4"));

	// Write DeLPHI model to disk
	delphi.writeModel(dst_folder,args.s("n"));

}
