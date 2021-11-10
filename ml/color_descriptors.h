#pragma once

#include <map>

#include "oceancv/ml/feature_descriptor.h"
#include "oceancv/img/image_entropy.h"

namespace ocv {

/**
 * (C)olor (V)alue (D)escriptor
 * Does not cumulate the amount of colors (as a histogram)
 * but returns the plain color values. Can be setup to return values
 * from various regular distributed pixels in the image patch.
 */
template<class T>
class ColorValueDescriptor : public FeatureDescriptor<T> {

public:
	
	/**
	 * For each ring i, all values with a manhattan distance
	 * of offset * i to the centroid are considered. For even images
	 * sizes, the centroid is the top/left pixel of the centroid.
	 * Channels holds the number of channels to expect (1 for CV_8UC1, 3 for CV_8UC3)
	 * Masked values will receive a value of -1
	 */
	ColorValueDescriptor(int channels, int offset, int rings);
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	int _channels;
	int _offset;
	int _rings;

};


/**
 * (C)olor (S)tatistic (D)escriptor
 * Computes statitics for the color values in the given image
 * Possible results are mean, median, variance, entropy.
 * Only works on CV_8UC1 and CV_8UC3 images.
 */
template<class T>
class ColorStatisticDescriptor : public FeatureDescriptor<T> {

public:
	ColorStatisticDescriptor(int channels, bool use_minmax, bool use_mean, bool use_median, bool use_stddev, bool use_entropy);
	
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	int _channels;
	bool _use_minmax;
	bool _use_mean;
	bool _use_median;
	bool _use_stddev;
	bool _use_entropy;

};


/**
 * (H)istogram (C)olor (D)escriptor
 * Descriptor for histograms from CV_8UC3 or CV_8UC1 images.
 */
template<class T>
class HistogramColorDescriptor : public FeatureDescriptor<T> {

public:
	HistogramColorDescriptor(int channels, int num_bins, T max_value = 255);
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	int _channels;
	int _num_bins;
	int _bin_size;
	T _max_value;
	
};


/**
 * (H)aralick (T)exture (D)escriptor
 * Computes various (not all) Haralick features from a CV_8UC1 cv::Mat
 
 */
template<class T>
class HaralickTextureDescriptor : public FeatureDescriptor<T> {

public:
	
	/**
	 * Creates a new Haralick descriptor For details on the Haralick measures see:
	 * Haralick: "Textural Features for Image Classification"
	 * @param bins the amount of bins to map the color values to
	 * @param offset the Euclidean distance between two pixels
	 * @param angular_second_moment whether this feature shall be used, default: true
	 * @param contrast whether this feature shall be used, default: true
	 * @param correlation whether this feature shall be used, default: true
	 * @param sum_of_squares_variance whether this feature shall be used, default: true
	 * @param inverse_difference_moment whether this feature shall be used, default: true
	 * @param sum_average whether this feature shall be used, default: true
	 * @param sum_variance whether this feature shall be used, default: true
	 * @param sum_entropy whether this feature shall be used, default: true
	 * @param entropy whether this feature shall be used, default: true
	 * @param difference_variance whether this feature shall be used, default: true
	 * @param difference_entropy whether this feature shall be used, default: true
	 **/
	HaralickTextureDescriptor(int bins, int offset = 1, bool angular_second_moment = true, bool contrast = true, bool correlation = true, bool sum_of_squares_variance = true, bool inverse_difference_moment = true, bool sum_average = true, bool sum_variance = true, bool sum_entropy = true, bool entropy = true, bool difference_variance = true, bool difference_entropy = true);
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	
	inline T _llog(T val) const;
	
	std::vector<bool> _subdescriptor_activity;
	
	int _num_bins;
	int _offset;
	
};

}
