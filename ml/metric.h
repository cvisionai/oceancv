#pragma once

#include <map>
#include <cmath>
#include <cstdio>
#include <cassert>

#include "oceancv/ml/vec_algorithms.h"

namespace ocv {

	enum METRIC_TYPES {
		EUCLIDEAN,
		EUCLIDEAN_SQUARED,
		SCALAR,
		MANHATTAN,
		MAXIMUM,
		ANGLE,
		DIVERGENCE,
		MPEG7_CLD,
		MPEG7_EHD,
		MPEG7_HTD
	};
	
	/**
	 * Metrics are used to measure distances between vectors. The base class does nothing.
	 * The different derived classes implement distance/similarity measures.
	 */
	template<class T>
	class Metric {

	public:

		/**
		 * Calculates the distance between two one-dimensional (!) vectors vec_1 and vec_2
		 */
		virtual T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const = 0;
		
		/**
		 * Computes the similarity between two vectors. Normally this is 1-distance() but it could potentially be overloaded
		 */
		virtual T similarity(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;

		/**
		 * Returns the ocv::DataType of a metric
		 */
		virtual ocv::METRIC_TYPES type() const = 0;

	};

	/**
	 * Calculates the euclidean distance between vec_1 and vec_2.
	 */
	template<class T>
	class EuclideanMetric : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		ocv::METRIC_TYPES type() const;
	};

	/**
	 * Calculates the squared euclidean distance between vec_1 and vec_2.
	 */
	template<class T>
	class EuclideanMetricSquared : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		ocv::METRIC_TYPES type() const;
	};
	
	/**
	 * Calculates the manhattan distance (sum of component differences) between vec_1 and vec_2.
	 */
	template<class T>
	class ManhattanMetric : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		ocv::METRIC_TYPES type() const;
	};

	/**
	 * Calculates the maximum distance between vec_1 and vec_2.
	 */
	template<class T>
	class MaximumMetric : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		ocv::METRIC_TYPES type() const;
	};

	/**
	 * Calculates the inner product between vec_1 and vec_2 - 1.0.
	 * (resulting in a range from [0.0 .. -2.0] for normalized data)
	 */
	template<class T>
	class ScalarMetric : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		ocv::METRIC_TYPES type() const;
	};

	/**
	 * Calculates the angle (0-PI) between vec_1 and vec_2.
	 */
	template<class T>
	class AngleMetric : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		ocv::METRIC_TYPES type() const;
	};
	
	/**
	 * Calculates the divergence between vec_1 and vec_2.
	 */
	template<class T>
	class DivergenceMetric : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		ocv::METRIC_TYPES type() const;
	};
	
	/**
	 * Calculates the distance between two MPEG7 CLD vectors
	 */
	template<class T>
	class CLDMetric : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2, int num_y_coefficients) const;
		ocv::METRIC_TYPES type() const;
	};
	
	/**
	 * Calculates the distance between two MPEG7 EHD vectors
	 */
	template<class T>
	class EHDMetric : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		ocv::METRIC_TYPES type() const;
	};
	
	/**
	 * Calculates the distance between two MPEG7 HTD vectors
	 */
	template<class T>
	class HTDMetric : public Metric<T> {
	public:
		T distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const;
		ocv::METRIC_TYPES type() const;
	};

}
