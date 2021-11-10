#pragma once

#include <iostream>
#include <numeric>

#include "oceancv/ml/vec_pair.h"

namespace ocv {

	/**
	 * A pair of two cv::Mat_<T>'s to enrich one data mat with additional information
	 * Each vector in the input *has to have* a partner in the output
	 */
	template <class T1, class T2 = T1>
	class MatPair {
	public:
		
		/**
		 * Creates an empty MatPair
		 */
		MatPair();
		
		/**
		 * Creates a MatPair and resizes to the given amount of items in the
		 * input and output vector.
		 */
		MatPair(int rows, int i_cols, int o_cols, T1 val_i = T1(0), T2 val_o = T2(0));
		
		/**
		 * Constructs a MatPair from two cv::Mats.
		 * They need to be single channel and have the same number of rows
		 */
		MatPair(const cv::Mat_<T1>& mat_i, const cv::Mat_<T2>& mat_o);
		
		/**
		 * Returns the number of rows
		 */
		int rows() const;
		
		/**
		 * Returns the dimension of the input vectors.
		 */
		int iCols() const;
		
		/**
		 * Returns the dimension of the output vectors.
		 */
		int oCols() const;
		
		/**
		 * Returns a copy of the input cv::Mat
		 */
		cv::Mat_<T1> i() const;
		
		/**
		 * Returns a reference to the input cv::Mat
		 */
		cv::Mat_<T1>& i();
		
		/**
		 * Returns a copy of the output cv::Mat
		 */
		cv::Mat_<T2> o() const;
		
		/**
		 * Returns a reference to the output cv::Mat
		 */
		cv::Mat_<T2>& o();
		
		/**
		 * Returns a copy of the i-th input vector.
		 */
		cv::Mat_<T1> iRow(int row) const;
		
		/**
		 * Returns a copy of the i-th output vector.
		 */
		cv::Mat_<T2> oRow(int row) const;
		
		/**
		 * Returns an iterator to the begin of the row-th row
		 */
		cv::MatConstIterator_<T1> iBegin(int row) const;
		
		/**
		 * Returns a copy of the i-th begin of the output vector.
		 */
		cv::MatConstIterator_<T2> oBegin(int row) const;
		
		/**
		 * Returns an iterator to the end of the row-th row
		 */
		cv::MatConstIterator_<T1> iEnd(int row) const;
		
		/**
		 * Returns a copy of the i-th end of the output vector.
		 */
		cv::MatConstIterator_<T2> oEnd(int row) const;
		
		/**
		 * Returns a copy of the j-th dimension of the i-th input vector.
		 */
		T1 i(int row, int col) const;
		
		/**
		 * Returns a copy of the j-th dimension of the i-th output vector.
		 */
		T2 o(int row, int col) const;
		
		/**
		 * Returns a reference to j-th dimension of the i-th input vector.
		 */
		T1& i(int row, int col);
		
		/**
		 * Returns a reference to the j-th dimension of the i-th output vector.
		 */
		T2& o(int row, int col);
		
		/**
		 * Sets the j-th dimension of the i-th input vector to the given value.
		 */
		void i(int row, int col, const T1 value);
		
		/**
		 * Sets the j-th dimension of the i-th output vector to the given value.
		 */
		void o(int row, int col, const T2 value);
		
		/**
		 * Gets one VecPair from the MatPair
		 */
		ocv::VecPair<T1,T2> pair(int row) const;
		
		/**
		 * Sets one VecPair in the MatPair by the given VecPair (data is copied)
		 */
		void pair(int row, const ocv::VecPair<T1,T2>& vp);
		
		/**
		 * Sets one VecPair in the MatPair by the given pair of Vecs(data is copied)
		 */
		void pair(int row, const cv::Mat_<T1>& vec_i, const cv::Mat_<T2>& vec_o);

		
	private:

		cv::Mat_<T1> _mat_i;
		cv::Mat_<T2> _mat_o;

	};
	
}
