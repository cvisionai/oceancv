#pragma once

#include "opencv2/cudawarping.hpp"
#include "opencv2/cudafilters.hpp"

#include "oceancv/cudaimg/misc_func.h"

namespace ocv {
namespace cuda {

	/**
	 * @brief Computes a set of Gabor responses for an CV_8UC3 image
	 * @details The given image will be converted to floating point precision.
	 * Depending on the parameters useplain, usesig, usemu, the plain Gabor response, and/or the Deviation
	 * and/or mean for a hyperbolized Gabor response will be returned.
	 * See Ontrup/Ritter "Perceptual Grouping in a Neural Model: Reproducing Human Texture Perception"
	 * section 3.3 for details.
	 * @param img the image that shall be filtered
	 * @param useplain whether the plain Gabor response shall be returned
	 * @param usesig whether the Deviation for the hyperbolized Gabor shall be returned
	 * @param usemu whether the Mean for the hyperbolized Gabor shall be returned
	 * @param size the size of the Gabor filter image
	 * @param jetsize the size of the Gabor jet itself (sigma = jetsize * omega * scale)
	 * @param scales Number of scales to compute the Gabor jets for
	 * @param phis Number of rotations to compute the Gabor jets for
	 * @return std::vector<cv::Mat> constains all the response images depending on the use* settings
	 * The order is: (scale = 1, orientation = 0.2*pi, plain), (s = 1, o = 0.2*pi, sig), (s = 1, o = 0.2*pi, mu)
	 * (s = 1, o = 0.4 * pi, plain), ....., (s = 2, o = 0.2*pi,plain), ...
	 **/
	std::vector<cv::cuda::GpuMat> gaborBank(const cv::cuda::GpuMat& img, bool useplain = false, bool usesig = true, bool usemu = false, int size = 15, float jetsize = 2, int scales = 3, int phis = 5);

	/**
	 * cv::Mat wrapper of the actual cv::cuda::GpuMat function that does the gabor bank computation.
	 * See documentation of that function for parameter details.
	 */
	std::vector<cv::Mat> gaborBank(const cv::Mat& img, bool useplain = false, bool usesig = true, bool usemu = false, int size = 15, float jetsize = 2, int scales = 3, int phis = 5);

}
}
