#pragma once

#include <iostream>
#include <numeric>

#include "opencv2/core.hpp"

namespace ocv {

/**
 * A class for algortihms to be applied to cv::Mat_<T>s
 * follows the OpenCV paradigm that the first parameter
 * is usually the object to be processed.
 */
template<class T>
class valg {

public:

	/**
	 * Returns the smaller of two given values
	 */
	static T less(T one, T two);
	
	/**
	 * Returns the bigger of two given values
	 */
	static T greater(T one, T two);
	
	/**
	 * Returns the single highest value within the vector.
	 */
	static T maxElement(const cv::Mat_<T>& vec);
	
	/**
	 * Returns the position of the highest value (but not the value itself)
	 */
	static int maxElementIndex(const cv::Mat_<T>& vec);
	
	/**
	 * Returns the lowest value within the vector.
	 */
	static T minElement(const cv::Mat_<T>& vec);

	/**
	 * Returns the position of the lowest value (but not the value itself).
	 */
	static int minElementIndex(const cv::Mat_<T>& vec);
	
	/**
	 * Returns the column-wise maximum of two vectors.
	 */
	static cv::Mat_<T> maxElements(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2);
	
	/**
	 * Returns the column-wise minimum of two vectors.
	 */
	static cv::Mat_<T> minElements(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2);

	/**
	 * Returns the mean (aka average) of all values in the vector.
	 */
	static T mean(const cv::Mat_<T>& vec);
	
	/**
	 * Returns the median of all values in the vector.
	 */
	static T median(const cv::Mat_<T>& vec);
	
	/**
	 * Returns the standard deviation of the values within the vector.
	 */
	static T stdDev(const cv::Mat_<T>& vec);
	
	/**
	 * Returns the squared deviation of the values in the vector from the given value.
	 * In case this is the mean, this results in the same as stdDev(...).
	 */
	static T deviation(const cv::Mat_<T>& vec, T t);
	
	/**
	 * Computes the scalar mean and variance for the given vector and stores them in the given references.
	 */
	static void zscore(const cv::Mat_<T>& vec, T& mean, T& variance);
	
	/**
	 * Returns the sum of all elements within the vector.
	 */
	static T sumElements(const cv::Mat_<T>& vec);

	/**
	 * Returns the euclidean length of the vector.
	 */
	static T euclideanLength(const cv::Mat_<T>& vec);

	
	/**
	 * Computes the scalar mean and variance for the given vector and applies them t the vector
	 */
	static void applyZScore(cv::Mat_<T>& vec);
	
	/**
	 * Applies the given *vectorial* mean and variance to the vector.
	 */
	static void applyZScore(cv::Mat_<T>& vec, const cv::Mat_<T>& mean, const cv::Mat_<T>& variance);
	
	/**
	 * Applies the given shift and scale factors to the values by computing v = (v * scale) + shift.
	 * This can be used to normalize a vector to a new value range, see MatOps::scaleRange(...).
	 */
	static void applyScaleRange(cv::Mat_<T>& vec, const cv::Mat_<T>& shift, const cv::Mat_<T>& scale);

	/**
	 * Appends a vector to another
	 */
	static cv::Mat_<T> append(const cv::Mat_<T>& vec, const cv::Mat_<T>& append);
	
};
	
}
