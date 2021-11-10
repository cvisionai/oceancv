#pragma once

#include "opencv2/opencv.hpp"

#include "oceancv/img/pixel_blob.h"

namespace ocv {

/**
 * Applies the TPI concept to an image
 */
void applyTPI(const cv::Mat_<float>& src, cv::Mat_<float>& dst, int inner_radius, int outer_radius);

/**
 * Computes local variograms for each pixel
 */
void variogram(const cv::Mat_<float>& src, cv::Mat_<float>& sill, cv::Mat_<float>& range, cv::Mat_<float>& nugget, int radius);

}
