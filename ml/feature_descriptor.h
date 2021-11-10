#pragma once

#include <map>
#include <iostream>

#include "opencv2/core.hpp"

namespace ocv {

// Virtual base class interface for extracting features from cv::Mats
template<class T>
class FeatureDescriptor {

public:

	FeatureDescriptor();

	/**
	 * Computes the feature vector for the given area, potentially masked by the optional mask area.
	 * When using a mask, it needs to be CV_8UC1. Set background pixels to 0 and foreground pixels to 255.
	 */
	virtual void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const = 0;

	// Checks whether the given image is valid for feature extraction
	virtual bool valid(const cv::Mat& img) const = 0;

	// Returns the size of a resulting feature vector
	virtual int size() const = 0;

	/**
	 * Returns the semantic setup of the feature vector
	 * as feature vectors from one descriptor can still contain
	 * groups of different semantics. To be able to discriminate these, e.g.
	 * for normalization, it has to be known, which vector components
	 * belong together.
	 */
	virtual std::vector<std::string> setup() const = 0;

};

}
