#pragma once

#include "opencv2/opencv.hpp"
#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudawarping.hpp"
#include "opencv2/cudafilters.hpp"
#include "opencv2/cudaarithm.hpp"

#include "oceancv/cudaimg/gauss_smoothing.h"

namespace ocv {
namespace cuda {

	/**
	 * Applies the heuristic fSpice color normalization to an image. Google how it works please.
	 * Actually it only does part 2 of the fSpice algorithm - it does not analyse the feature space
	 * but expects a pre-computed sigma value.
	 * @param c_src the cv::cuda::Mat to apply the method to
	 * @param c_dst the cv::cuda::Mat to write the result to
	 * @param gauss_size the sigma of the Gauss filter to apply
	 * @param stretch_hist whether to stretch the histogram. Usually useful, rarely not
	 * @param convert_to_8bit ususally the result will be a CV_8UC3 but if you prefer you can also get the CV_32FC3
	 */
	void fspice(const cv::cuda::GpuMat& c_src, cv::cuda::GpuMat& c_dst, int gauss_size, bool stretch_hist = true, bool convert_to_8bit = true);

	/**
	 * cv::Mat wrapper of the actual cv::cuda::GpuMat function that does the fspice computation.
	 * See documentation of that function for parameter details.
	 */
	cv::Mat fspice(const cv::Mat& src, int gauss_size, bool stretch_hist = true);

}
}
