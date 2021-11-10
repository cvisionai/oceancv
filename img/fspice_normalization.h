#pragma once

#include "opencv2/opencv.hpp"

namespace ocv {

	/**
	 * cv::Mat version of the fspice normalization. Not a wrapper but really executed only on the CPU!
	 * See documentation of the GPU function for parameter details.
	 */
	cv::Mat fspice(const cv::Mat& src, int gauss_size, bool stretch_hist = true);
	
}
