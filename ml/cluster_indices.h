#pragma once

#include <algorithm>

#include "oceancv/ml/mat_algorithms.h"
#include "oceancv/ml/mat_pair_algorithms.h"

namespace ocv {

	enum CI_TYPES {
		CALINSKI_HARABASZ,
		INDEX_I,
		DAVIES_BOULDIN,
		INTER_CLASS_VARIANCE,
		INTRA_CLASS_VARIANCE
	};
	
	enum SC_TYPES {
		BETWEEN_CLASS_SCATTER,
		WITHIN_CLASS_SCATTER
	};
	
	
template <class T1, class T2 = T1>
class ci {

public:

	/**
	 * Generic function to call one of the specialized cluster index functions.
	 * Source data needs to be given as a vector of separate cv::Mat_s. Each
	 * Mat represents one class.
	 */
	static T1 compute(const std::vector<cv::Mat_<T1>>& mv, ocv::CI_TYPES ci_type);
	static cv::Mat_<T1> compute(const std::vector<cv::Mat_<T1>>& mv, ocv::SC_TYPES sc_type);
	
	/**
	 * Generic function to call one of the specialized cluster index functions.
	 * Source data needs to be given as one MatPair, which will be split according to
	 * the class labels in the label_index-th output column of the MatPair.
	 */
	static T1 compute(const ocv::MatPair<T1,int>& mp, ocv::CI_TYPES ci_type, size_t o_col_idx);
	static cv::Mat_<T1> compute(const ocv::MatPair<T1,int>& mp, ocv::SC_TYPES sc_type, size_t o_col_idx);
	
	/**
	 * @brief Computes the normalized scalar distance between two Vectors.
	 * @param vec_1 first vector
	 * @param vec_2 second vector
	 * @return the distance
	 **/
	static T1 normalizedScalarDist(const cv::Mat_<T1>& vec_1, const cv::Mat_<T1>& vec_2);


private:

	static void computeAllMean(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means, cv::Mat_<T1>& all_mean);
	
	/**
     * @brief Computes the between class scatter matrix.
     * @param class_mats the data separated by the cluster label
	 * @param class_means the means of each cluster
	 * @return Mat containing the dimension-wise scatter
     **/
	static cv::Mat_<T1> betweenClassScatter(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means);
	
	/**
     * @brief Computes the within class scatter matrix.
     * @param class_mats the data separated by the cluster label
	 * @param class_means the means of each cluster
	 * @return Mat containing the dimension-wise scatter
     **/
	static cv::Mat_<T1> withinClassScatter(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means);
	
	/**
	 * @brief Computes the Chalinski-Harabasz cluster index.
	 * @param class_mats the data separated by the cluster label
	 * @param class_means the means of each cluster
	 * @return the computed cluster index
	 **/
	static T1 chalinskiHarabasz(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means);
	
	/**
	 * @brief Computes the Index-I cluster index.
	 * @param class_mats the data separated by the cluster label
	 * @param class_means the means of each cluster
	 * @return the computed cluster index
	 **/
	static T1 indexI(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means);
	
	/**
	 * @brief Computes the Davies Boudlin cluster index.
	 * @param class_mats the data separated by the cluster label
	 * @param class_means the means of each cluster
	 * @return the computed cluster index
	 **/
	static T1 daviesBouldin(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means);
	
	/**
	 * @brief Computes the Davies Boudlin cluster index.
	 * @param class_mats the data separated by the cluster label
	 * @param class_means the means of each cluster
	 * @return the computed cluster index
	 **/
	static T1 intraClassVariance(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means);
	
	/**
	 * @brief Computes the inter class variance.
	 * @param class_means the means of each cluster
	 * @return the computed cluster index
	 **/
	static T1 interClassVariance(const std::vector<cv::Mat_<T1>>& class_means);
	
};

}
