#pragma once

#include <iostream>

#include "curl/curl.h"

#include "opencv2/opencv.hpp"

namespace ocv {

/**
 * Loads an image over the internet frmo a given URL and stores
 * it in the cv::Mat object passed by reference.
 * @param url a string url to download the image from
 * @param img a cv::Mat to store the remote image in
 * @return true on success, false on fail
 **/
bool loadFromUrl(std::string url,cv::Mat& img);
	
}
