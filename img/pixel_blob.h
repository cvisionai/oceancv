#pragma once

#include "opencv2/opencv.hpp"

namespace ocv {

 /**
 * @brief: Computes the mass center of a contour.
 * Computes the mass center of all pixels contained in a contour.
 * Pixel intensities do not contribute!
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 */
cv::Point contourMassCenter(const std::vector<cv::Point>& contour, int minx, int maxx, int miny, int maxy);

/**
 * Computes the area of a pixel blob contour, as computed by cv::findContour.
 * Takes holes into account and eventually subtracts contours within contours.
 * @param contours the contours as computed by cv::findContours
 * @param hierarchy the contour hierarchy as computed by cv::findContours
 * @param i index of the contour to be measured
 * @return the size of the pixel contour (number of pixels)
 */
double contourArea(std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Vec4i>& hierarchy, size_t i);

/**
 * Returns an index list of the children contours of a given contour.
 * Contours are computed by cv::findContours and the parent-child relation
 * has to be given by the hierarchy variable computed by that function.
 * @param hierarchy the contour hierarchy as computed by cv::findContours
 * @param i the index of the contour to find the children of
 * @return vector of contour indices that are contour children of the given contour
 */
std::vector<int> getChildren(std::vector<cv::Vec4i>& hierarchy, size_t i);

/**
 * TODO
 */
std::vector<size_t> getObjectContours(std::vector<cv::Vec4i>& hierarchy);

/**
 * Computes morphological statistics on a pixel blob contour
 * (min/max x/y values, middle of min/max x/y values and - if requested -
 * the true barycenter of the pixels (each pixel in the blob contributes a
 * weight of 1.
 * @param contour A single pixel blob contour as computed by cv::findContours
 * @param with_barycenter whether the barycenter shall be computed as well
 * @return vector of cv::Points {{minx,miny},{maxx,maxy},{maxx-minx/2,maxy-miny/2}[,{bary_x,bary_y}]}
 */
std::vector<cv::Point> contourInfo(const std::vector<cv::Point>& contour, bool with_barycenter = true);

/**
 * Returns all relative pixel coordinates of neighbours within an euclidean
 * distance of >= inner_radius and <= outer_radius.
 * @param inner_radius the smallest distance from the centroid to consider
 * @param outer_radius the largest distance from the centroid to consider
 * @param pixels the pixel positions
 */
void getNeighboursInCircle(int inner_radius, int outer_radius, std::vector<cv::Point>& pixels);

}
