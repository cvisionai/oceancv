#pragma once

#include <cassert>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

//#include "oceancv/img/pixel_blob.h"

namespace ocv {

/**
 * Computes the normalized color entropy in a region of interest in the given image,
 * specified by the given contour. Expects a single channel image (CV_8UC1).
 * @param img the CV_8UC1 image to compute the entropy in
 * @param mask a mask of the same size as the image with the value 255 for pixels
 * to consider and 0 else (or non mask)
 * @return the entropy in the contour according to the pixel color
 */
double entropy(const cv::Mat& img, const cv::Mat& mask = cv::Mat());

/**
 * Computes the normalized entropy (in 0..1) for an ROI around each pixel within one image
 * @param mat the image to compute the entropy for
 * @param half_kernel specifies how large the ROI around each pixel is set (2*half_kernel+1)
 * @return a cv::Mat where each pixel corresponds to the entropy around that pixel in the given mat
 */
cv::Mat fullImageEntropy(const cv::Mat& mat, size_t half_kernel);

/**
 * Warning: This function alters the given histogram!
 * Warning: does not return a normalized entropy (get it by dividing the result by log2(N=maxval)
 * Computes the entropy value for a pre-computed histogram.
 * @param hist a single channel cv::Mat (i.e. a vector) containing the histogram of values
 * @param num_px the total sum of values in the histogram if available. Set to 0 to let it be computed.
 * @ return the entropy of the values in the histogram
 */
double entropyForHistogram(cv::Mat& hist, size_t num_px = 0);

double entropyForHistogram(size_t* hist, size_t hist_size, size_t num_px);

}
