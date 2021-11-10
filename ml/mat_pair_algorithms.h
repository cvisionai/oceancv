#pragma once

#include <map>
#include <random>

#include "oceancv/ml/vec_algorithms.h"
#include "oceancv/ml/mat_algorithms.h"
#include "oceancv/ml/mat_pair.h"
#include "oceancv/ml/metric.h"


namespace ocv {

template<class T1, class T2 = T1>
class mpalg {

public:

	/**
     * Calculates the Mat vector where the output vector is closest to a vector given as an argument.
     * @param mp Reference to the MatPair
     * @param vec Reference to the Vector
     * @param metric Reference to a Metric
     * @param start_row First row index to consider in the search
     * @param end_row Last row index to consider in the search
     * @return Index of the best match vector, always relative to the full MatPair, even when start_row is set
     */
	static size_t bestMatchIndexOutput(const ocv::MatPair<T1,T2>& mp, const cv::Mat_<T2>& vec, const ocv::Metric<T2>& metric, int start_row = 0, int end_row = -1);
	
	/**
     * Calculate covariance Mat of elements with class label class_label
     * @param mp Data Mat
     * @param dst a reference to the result MatPair
     * @param class_label Only size with this class label are taken into account
     * @param idx The index where to look for the class_label. Default is 0.
     */
	static int covarianceMat(const ocv::MatPair<T1,T2>& mp, cv::Mat_<T1>& dst, T2 class_label, int idx = 0);
	
	/**
     * Calculate mean vector of elements with class label class_label, that is looked for in output column idx
     */
	static int mean(const ocv::MatPair<T1,T2>& mp, cv::Mat_<T1>& mean, T2 class_label, int o_col_idx = 0);

	/**
	 * Computes the column-wise mean and variance of all vectors with class label class_label, that is looked for in column idx and returns them by reference.
	 */
	static int zscore(const ocv::MatPair<T1,T2>& mp, cv::Mat_<T1>& mean, cv::Mat_<T1>& variance, T2 class_label, int col_idx = 0);
	
	/**
	 * Checks whether any value within the meta matrix is "Not a Number".
	 */
	static bool isNaN(const ocv::MatPair<T1,T2>& mp);
	
	/**
	 * Create a subset of a MatPair, containing all elements but only the input vector components for which
	 * the indicator_vector component is 1 (i.e. removes all the colums in mm, the indicator for which
	 * is set to 0).
	 */
	static void selectInputColumns(const ocv::MatPair<T1,T2> mp, ocv::MatPair<T1,T2>& dst, const std::vector<int> selection_vector);
	
    /**
	 * Create a subset of a MatPair, containing all components of an input vector but only those items for which
	 * the selection_vector component is 1 (i.e. removes all the rows in mp, the indicator for which
	 * is set to 0). Values in selection_vector other than 0 or 1 will lead to failure.
	 */
	static int selectRows(const ocv::MatPair<T1,T2> mp, ocv::MatPair<T1,T2>& dst, const std::vector<int> selection_vector);
	
	/**
	 * Returns a random subset of the matrix.
	 * @param mp the MatPair from wich the subset is genrated
	 * @param dst the result MatPair to store the subset in
	 * @param items the size ob the subset
	 */
	static void randomSubset(const ocv::MatPair<T1,T2>& mp, ocv::MatPair<T1,T2>& dst, const size_t items);
	
    /**
	 * Append cols to a Mat (or MatPair) from another MatPair with an equal size. The (input) Vectors of the "append" Mat(Pair) are appended
	 * Vector-wise wo the input Vectors of the MatPair mm.
	 */
	static void appendCols(ocv::MatPair<T1,T2>& mp, const cv::Mat_<T1>& to_append);
	
	/**
	 * Append rows to a MatPair from another MatPair with an equal dim. The items of the append MatPair are pushed to the back of
	 * the first MatPair.
	 */
	static void appendRows(ocv::MatPair<T1,T2>& mp, const ocv::MatPair<T1,T2>& to_append);
	
    /**
	 * Splits a MatPair to pieces by creating a set of MatPairs that are discriminated
	 * by the value stored in the pattern_index-th component of the output vector.
	 * The order of the MatPairs is by increasing value occurring in the o_col_idx
	 */
	static void splitMatPair(const ocv::MatPair<T1,int>& mp, std::vector<ocv::MatPair<T1,int>>& dst, int o_col_idx = 0);
	
	/**
	 * Splits a MatPair to pieces by creating a set of MatPairs that are discriminated
	 * by the value stored in the pattern_index-th component of the output vector.
	 * The order of the MatPairs is by increasing value occurring in the o_col_idx
	 */
	static void splitMatPairToMat(const ocv::MatPair<T1,int>& mp, std::vector<cv::Mat_<T1>>& dst, int o_col_idx = 0);


};

}
