#pragma once

#include <cassert>

#include "opencv2/opencv.hpp"
#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudawarping.hpp"
#include "opencv2/cudafilters.hpp"
#include "opencv2/cudaarithm.hpp"

namespace ocv {
namespace cuda {

/**
 * GPU version of the normalized entropy function for full images (see CPU version doc).
 */
double entropy(const cv::cuda::GpuMat& mat);

cv::Mat fullImageEntropy(const cv::cuda::GpuMat& mat, size_t half_kernel);

cv::Mat fillImageEntropyIntegral(const cv::cuda::GpuMat& mat);

}
}
