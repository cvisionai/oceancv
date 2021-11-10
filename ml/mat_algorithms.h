#pragma once

#include <typeinfo>

#include "opencv2/core.hpp"
#include "oceancv/ml/vec_pair.h"
#include "oceancv/ml/metric.h"

namespace ocv {

template<class T>
class malg {

public:

	/**
     * Calculates the Mat item where the input vector is closest to a vector given as an argument.
     */
	static size_t bestMatchIndex(const cv::Mat_<T>& mat, const cv::Mat_<T>& vec, const ocv::Metric<T>& metric);

	/**
     * Gets the auto-correlation Mat.
     * @param src Reference to the input data Mat
     * @param dst Reference to the result Mat
     */
	static void autoCorrelation(const cv::Mat_<T>& src, cv::Mat_<T>& dst);

	/**
     * Calculate covariance Mat of elements in m
     */
	static void covarianceMat(const cv::Mat_<T>& m, cv::Mat_<T>& dst);
	
	/**
     * Calculate mean vector of elements (the center point)
     */
	static cv::Mat_<T> mean(const cv::Mat_<T>& m);
	
	/**
	 * Returns the column-wise maximum of all vectors in the matrix.
	 */
	static cv::Mat_<T> maxElements(const cv::Mat_<T>& m);
	
	/**
	 * Returns the column-wise minimum of all vectors in the matrix.
	 */
	static cv::Mat_<T> minElements(const cv::Mat_<T>& m);
	
	/**
	 * Centers the matrix (subtracts the mean)
	 */
	static void applyMean(cv::Mat_<T>& m, const cv::Mat_<T>& mean);
	
	/**
	 * Computes the column-wise variance of all vectors
	 */
	static cv::Mat_<T> variance(const cv::Mat_<T>& m, const cv::Mat_<T>& mean);
	
	/**
	 * Applies the given column-wise mean and variance to the matrix.
	 */
	static void applyZScore(cv::Mat_<T>& m, const cv::Mat_<T>& mean, const cv::Mat_<T>& variance);

	/**
	 * Computes the shift and scaling vectors to scale all data to the value interval
	 * given by new_min & new_max (as Vectors, thus different intervals
	 * can be defined for each dimension). Shift and scalie factors are returned by reference
	 */
	static ocv::VecPair<T> scaleRange(const cv::Mat_<T>& m, const cv::Mat_<T> new_min, const cv::Mat_<T> new_max);
	
	/**
	 * Computes the shift and scaling vectors to scale all data to the value interval given by new_min & new_max (as values, thus every dimension
	 * will be set to the same new minimum & maximum. Those shift and scalie factors are returned by reference and eventually applied.
	 */
	static ocv::VecPair<T> scaleRange(const cv::Mat_<T>& m, T new_min, T new_max);
	
	/**
	 * Applies the given shift and scale factors to the matrix.
	 */
	static void applyScaleRange(cv::Mat_<T>& m, const cv::Mat_<T>& scale, const cv::Mat_<T>& shift);
	
	/**
	 * Normalizes each vector according to the given metric. The vector is divided by the metric-specific distance towards the 0-vector.
	 * E.g. utilizing the EuclideanMetric results in vectors that have the euclidean length 1.
	 */
	static void normalize(cv::Mat_<T>& m, const ocv::Metric<T>& metric);
	
	/**
	 * Returns a transposed copy of the matrix.
	 */
	static void transpose(const cv::Mat_<T>& m, cv::Mat_<T>& dst);

	/**
	 * Compute individual means for each Mat in the given std::vector or ocv::Mats
	 */
	static void means(const std::vector<cv::Mat_<T>>& single_mats, std::vector<cv::Mat_<T>>& ret);
	
};

}

