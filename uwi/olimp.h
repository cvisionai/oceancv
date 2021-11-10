#pragma once

#include <iostream>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

#include "oceancv/cudaimg/fspice_normalization.h"

namespace ocv {

std::vector<std::pair<int,double>> olimp(std::string video_path, const cv::Rect& roi, int max_pixel_dist = 20, int pixel_dist_bin_width = 2, int angle_bin_width = 30) {

	int angle_size = 360/angle_bin_width;
	int dist_size = max_pixel_dist/pixel_dist_bin_width;

	// Try to open video file
	cv::VideoCapture vid_capture(video_path);
	if(!vid_capture.isOpened()) {
		std::cout << "Could not open video " << video_path << std::endl;
		return {};
	}

	cv::Mat frame, gray, fsp;
	std::vector<std::vector<cv::Point2f>> points = {{},{}};
	cv::Point2f p1,p2;

	long max_val;
	int max_dir, max_dist;
	float dist, dir;
	int min_dir, min_dist, second;

	std::vector<std::vector<std::vector<std::pair<float,float>>>> dir_dist_histogram = {};

	std::vector<std::pair<int,double>> ret = {};

	// Create a progress bar
	int fps = vid_capture.get(cv::CAP_PROP_FPS);
	int num = vid_capture.get(cv::CAP_PROP_FRAME_COUNT);
	int duration = num/fps;

	std::vector<std::string> msgs = {};
	for(int i = 0; i < duration; i++)
		msgs.push_back("Second: " + std::to_string(i));
	//ocv::progress_bar pg(msgs);
	int idx = 0;

	while(true) {

		// Grab a frame
		vid_capture >> frame;
		if(frame.empty())
				break;

		// Select only the ROI, convert it to grayscale, blur it strongly to remove noise and then select feature points
		fsp = frame(roi);
		cv::cvtColor(fsp, gray, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(gray, frame, cv::Size(11,11), 0, 0 );
		cv::goodFeaturesToTrack(frame, points[idx%2], 100, 0.1, 50, cv::noArray(), 31);

		// The current second of the video
		second = idx / fps;

		// Every second, collect the distance estimates and dump them to the result file
		if(idx % fps == 0) {

			if(idx > 0) {

				// Find maximum in histogram
				max_val = 0;
				max_dir = -1;
				max_dist = -1;
				for(int i = 0; i < 360; i+=angle_size) {
					for(int j = 0; j < max_pixel_dist; j+= dist_size) {
						if(dir_dist_histogram[i/angle_size][j/dist_size].size() > max_val) {
							max_dir = i;
							max_dist = j;
							max_val = dir_dist_histogram[i/angle_size][j/dist_size].size();
						}
					}
				}

				// Pick average direction and distance
				float avg_dir = 0.0;
				float avg_dist = 0.0;
				for(auto p : dir_dist_histogram[max_dir/angle_size][max_dist/dist_size]) {
					avg_dir += p.first;
					avg_dist += p.second;
				}

				avg_dir /= dir_dist_histogram[max_dir/angle_size][max_dist/dist_size].size();
				avg_dist /= dir_dist_histogram[max_dir/angle_size][max_dist/dist_size].size();
				ret.push_back(std::make_pair(avg_dir,avg_dist));

				// Advance the progress bar
				//pg.advance(second);

			}

			// Reset the histogram
			dir_dist_histogram = {};
			for(int i = 0; i < 360; i+=angle_size) {
				dir_dist_histogram.push_back({});
				for(int j = 0; j < max_pixel_dist; j+= dist_size) {
					dir_dist_histogram[i/angle_size].push_back({});
				}
			}

		}

		if(idx > 0) {

			// Find closest previous point for each point
			for(int i = 0; i < points[(idx+1)%2].size(); i++) {
				p1 = points[(idx+1)%2][i];
				min_dist = frame.cols*frame.rows;
				min_dir = 0;
				for(int j = 0; j < points[idx%2].size(); j++) {
					p2 = points[idx%2][j];
					dist = cv::norm(p1 - p2);
					if(dist > 0 && dist < max_pixel_dist) {
						if(dist < min_dist) {
							min_dist = dist;
							min_dir = acos((p1.x-p2.x)/dist)*180/3.14159;
						}
					}
				}
				if(min_dist < max_pixel_dist) {
					dir_dist_histogram[min_dir/angle_size][min_dist/dist_size].push_back(std::make_pair(min_dir,min_dist));
				}
			}

			// For visualization puposes only
			for(auto p : points[idx%2]) {
				cv::circle(frame,p,5,cv::Vec3b(0,0,0));
			}
			for(auto p : points[(idx+1)%2]) {
				cv::circle(frame,p,5,cv::Vec3b(127,127,127));
			}
			cv::imwrite("/shipcc/nas/3/MSM96/viz_"+std::to_string(idx)+".png",frame);

		}

		idx++;

		if(second > 150)
			break;
	}

	return ret;

}

}
