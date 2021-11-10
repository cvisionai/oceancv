#include "oceancv/cudaimg/image_entropy.h"

namespace ocv {
namespace cuda {

float BASE2_LOG = 1.f / std::log(2);
float H256_MAX = 1.f / std::log2(256);

double entropy(const cv::cuda::GpuMat& mat) {

	assert(mat.type() == CV_8UC1);

	// Prepare histogram
	cv::cuda::GpuMat hist(1,256,CV_32SC1);
	cv::cuda::GpuMat prob(1,256,CV_32FC1);

    // Compute the histograms:
    cv::cuda::calcHist(mat, hist);

	// Compute probabilities
	cv::cuda::add(hist,cv::Scalar(1),hist);
	cv::cuda::multiply(hist,cv::Scalar(1.f/(mat.rows*mat.cols)),prob,1,CV_32FC1);

	// Compute log of probabilities
    cv::cuda::GpuMat logP;
    cv::cuda::log(prob,logP);

	// Compute the
	cv::cuda::multiply(prob,logP,hist);

    return -1*cv::cuda::sum(hist).val[0]*H256_MAX*BASE2_LOG;

}

cv::Mat fullImageEntropy(const cv::cuda::GpuMat& mat, size_t half_kernel) {

	cv::cuda::GpuMat roi;
	cv::Mat ret = cv::Mat(mat.size(),CV_32FC1,cv::Scalar(0));
	size_t full_kernel = 2 * half_kernel + 1;
	for(size_t y = half_kernel; y < mat.rows - half_kernel; y++) {
		for(size_t x = half_kernel; x < mat.cols - half_kernel; x++) {
			roi = cv::cuda::GpuMat(mat,cv::Rect(x-half_kernel,y-half_kernel,full_kernel,full_kernel));
			ret.at<float>(y,x) = ocv::cuda::entropy(roi);
		}
	}

	return ret;

}

cv::Mat fillImageEntropyIntegral(const cv::cuda::GpuMat& mat) {

	assert(mat.type() == CV_8UC1);

	std::vector<cv::cuda::GpuMat> integrals(256);
	cv::cuda::GpuMat g_tmp_1;

	// Compute integral images
	for(size_t c = 0; c < 256; c++) {

		// Pick those pixels that are equal to the current color
		cv::cuda::compare(mat,cv::Scalar(c),g_tmp_1,cv::CMP_EQ);

		// Compute integral image
		cv::cuda::integral(g_tmp_1,integrals[c]);

	}

	cv::Mat ret(mat.size(), CV_32FC1);

	// TODO

	return ret;

}


}
}
