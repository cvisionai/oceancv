#include <iomanip>
#include <chrono>

#include "opencv2/opencv.hpp"

#include "oceancv/util/cli_args.h"
#include "oceancv/util/cli_arg_provenance.h"
#include "oceancv/util/file_parser.h"
#include "oceancv/util/progress_bar.h"
#include "oceancv/util/geo_coordinates.h"

#include "oceancv/uwi/olimp.h"

/**
 * Applies the Olimp method to a video to compute the image area for each individual frame, based on the navigation data of the camera. To do so it tracks features in the frames.
 */
int main(int argc, char** argv) {

	// Read command line arguments
	ocv::cli_args args = ocv::cli_args(argc,argv,{
			{"i","Video file to compute areas for"},
			{"n","Navigation file for video"},
			{"s","Start time (UTC) of video in the format YYYYMMDDhhmmss"},
			{"o","Path to result file for storing the image area"},
			{"tf","Time format for parsing navigation file","%Y-%m-%dT%H:%M:%S"},
			{"roi","top_left_x,top_left_y,bottom_right_x,bottom_right_y (4 relative(! 0<->1) coordinates to specify ROI in frame)","0.25,0.25,0.75,0.75"},
			{"m","Magic frame factor (4K 60fps video is only 30 fps in reality, therefore m=2, otherwise 1)","1"}
		 });

	ocv::forceFileExists(args.s("n"));
	ocv::forceFileExists(args.s("i"));

	// Parse datetime
	std::tm vid_start_tm = {};
	std::stringstream ss(args.s("s"));
	ss >> std::get_time(&vid_start_tm, "%Y%m%d%H%M%S");
	auto vid_start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::from_time_t(std::mktime(&vid_start_tm)).time_since_epoch()).count();

	//std::time_t start = std::chrono::system_clock::to_time_t(tp);
	//std::tm _tm = *std::localtime(&now_c);
	std::cout << "Parsed video start time: " << std::put_time(&vid_start_tm,"%Y-%m-%d %H:%M:%S") << " " << vid_start << std::endl;

	// TODO Store provenance
	//ocv::cli_arg_provenance prov(args,args.s("o"));

	// TODO Read position file for video and extract positions starting at start time
	std::vector<std::vector<std::string>> nav_data = ocv::readASCIFile(args.s("n"));
	std::cout << "Have " << nav_data.size() << " navigation values" << std::endl;

	// Get video length
	// Try to open video file
	cv::VideoCapture vid_capture(args.s("i"));
	if(!vid_capture.isOpened()) {
		std::cout << "Could not open video " << args.s("i") << std::endl;
		return {};
	}

	int fps = vid_capture.get(cv::CAP_PROP_FPS);
	int num = vid_capture.get(cv::CAP_PROP_FRAME_COUNT);

	std::cout << "Video has " << fps << " frames per second" << std::endl;


	int duration = num/fps;
	std::cout << "Video length: " << duration << " seconds" << std::endl;

	// Get ROI Rect
	int w = vid_capture.get(cv::CAP_PROP_FRAME_WIDTH);
	int h = vid_capture.get(cv::CAP_PROP_FRAME_HEIGHT);
	std::vector<float> coords = args.vf("roi");

	cv::Rect roi(w*coords[0],h*coords[1],w*(coords[2]-coords[0]),h*(coords[3]-coords[1]));

	vid_capture.release();

	// TODO Compute end time of video
	auto vid_end = vid_start + duration*1000;

	// Extract relevant navigation data from loaded file
	std::vector<std::tuple<int,double,double>> coordinates = {};

	// Seek first navigation line in file according to timestamp
	long diff;
	double lat, lon;

	std::tm tmp_tm;
	std::stringstream tmp_ss;
	//tmp_ss >> std::get_time(&tmp_tm, "%Y-%m-%dT%H:%M:%S");
	long tmp_tp;



	for(int i = 1; i < nav_data.size(); i++) {
		tmp_tm = {};
		tmp_ss = std::stringstream(nav_data[i][0]);//+"+0000");
		tmp_ss >> std::get_time(&tmp_tm, args.s("tf").c_str());
		tmp_tp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::from_time_t(std::mktime(&tmp_tm)).time_since_epoch()).count();

		if(tmp_tp >= vid_start && tmp_tp < vid_end) {

			// Compute second offset since video start time
			diff = tmp_tp - vid_start;

			// Get coordinates
			lat = std::stod(nav_data[i][1]);
			lon = std::stod(nav_data[i][2]);
			coordinates.push_back(std::tuple<int,double,double>(diff,lat,lon));

			//std::cout << nav_data[i][0] << " " << nav_data[i][1] << " " << nav_data[i][2] << " " << std::endl;
		}
	}

	std::cout << "Found " << coordinates.size() << " relevant coordinates" << std::endl;

	std::vector<double> speeds = {};
	double avg_speed = 0.0;
	double speed;
	double dist_l, dist_t;

	for(int i = 1; i < coordinates.size(); i++) {
		dist_l = ocv::DecLatLonDistance(std::get<1>(coordinates[i]),std::get<2>(coordinates[i]),std::get<1>(coordinates[i-1]),std::get<2>(coordinates[i-1]));
		dist_t = std::get<0>(coordinates[i])-std::get<0>(coordinates[i-1]);
		speed = dist_l/dist_t*1000;
		avg_speed += speed;
		speeds.push_back(speed);
	}

	std::cout << "Average speed: " << avg_speed / coordinates.size() << " m/s (from navigation data)" << std::endl;

	// TODO Run Olimp
	std::vector<std::pair<int,double>> dir_and_vel = ocv::olimp(args.s("i"), roi);

std::cout << std::endl;
std::cout << "Computed " << dir_and_vel.size() << " pixel speed values (" << speeds.size() << " m/s values)" << std::endl;

	// Create and open output file handle and add header
	std::ofstream res;
	res.open(args.s("o"));
	res << "Second\tDirection [°]\tVelocity [px/frame]\tVelocity [px/s]\tVelocity [m/s]\tScaling Factor [px/m]\tImage Area [m^2]" << std::endl;


	for(int i = 0; i < speeds.size(); i++) {

		double pix_vel_per_frame = dir_and_vel[i].second;
		double pix_vel_per_sec = pix_vel_per_frame * fps / args.i("m");

		res << i << "\t" << dir_and_vel[i].first;
		res << "\t" << pix_vel_per_frame;
		res << "\t" << pix_vel_per_sec;

		if(speeds[i] == 0)
			res << "\t0\t\t" << std::endl;
		else {
			double pix_per_m = pix_vel_per_sec / speeds[i];	// [px/s] / [m/s] = [px/m]
			double image_area = w*h / pix_per_m / pix_per_m;
			res << "\t" << speeds[i] << "\t" << pix_per_m << "\t" << image_area << std::endl;
		}

	}


	std::cout << "Done." << std::endl;

}
