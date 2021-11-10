#include "oceancv/img/image_entropy.h"

namespace ocv {

float BASE2_LOG = 1.f / std::log(2);
float H256_MAX = 1.f / std::log2(256);

double entropyForHistogram(cv::Mat& hist, size_t num_px) {

	if(num_px == 0)
		num_px = hist.total();

	cv::add(hist,1,hist);
	hist /= num_px;

    cv::Mat logP;
    cv::log(hist,logP);

    return -1*sum(hist.mul(logP)).val[0]*BASE2_LOG;

}

double entropyForHistogram(size_t* hist, size_t hist_size, size_t num_px) {

	float ret = 0;
	float fac = 1.f / num_px;
	for(size_t i = 0; i < hist_size; i++) {
		ret += fac * hist[i] * std::log(fac*(hist[i]+1));
	}

	return -1*BASE2_LOG*ret;

}

double entropy(const cv::Mat& img, const cv::Mat& mask) {

	assert(img.type() == CV_8UC1);

	// Prepare histogram
	int hist_size = 256;
	cv::Mat hist;

    // Set the range
    float range[] = {0, 256};
    const float* hist_range = {range};

    // Compute the histograms:
    cv::calcHist(&img, 1, 0, mask, hist, 1, &hist_size, &hist_range, true, true);

	// Compute probabilities (add one first to counter log(0) problem)
	cv::add(hist,1,hist);
	hist /= img.total();

	// Compute log of probabilities
    cv::Mat logP;
    cv::log(hist,logP);

    // Change log base to 2 and return the sum over the product
    // of all logs and probs and scale it to interval 0..1
	return -1*sum(hist.mul(logP)).val[0]*H256_MAX*BASE2_LOG;

}

cv::Mat fullImageEntropy(const cv::Mat& mat, size_t half_kernel) {

	cv::Mat roi,ret = cv::Mat(mat.size(),CV_32FC1,cv::Scalar(0));
	size_t full_kernel = 2 * half_kernel + 1;
	for(size_t y = half_kernel; y < mat.rows - half_kernel; y++) {
		for(size_t x = half_kernel; x < mat.cols - half_kernel; x++) {
			roi = mat(cv::Rect(x-half_kernel,y-half_kernel,full_kernel,full_kernel));
			ret.at<float>(y,x) = ocv::entropy(roi);
		}
	}

	return ret;
}

}
