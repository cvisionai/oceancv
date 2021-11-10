#pragma once

#include "opencv2/core.hpp"

namespace ocv {

	/**
	 * This class combines two cv::Mat_<T> objects into one object.
	 * It features the same getter/setter as a mat_pair.
	 * The vectors would be copies of the ones stored in a mat_pair.
	 * The cv::Mats have to be one channel, one row, N cols!
	 */
	template <class T1, class T2 = T1>
	class VecPair {
	public:
	
		/**
		 * Creates a new VecPair from two vectors
		 */
		VecPair(cv::Mat_<T1> vec_i, cv::Mat_<T2> vec_o);
		
		/**
		 * Returns a reference to the input vector
		 */
		cv::Mat_<T1>& i();
		
		/**
		 * Returns a reference to the output vector
		 */
		cv::Mat_<T2>& o();
		
		/**
		 * Returns a copy of the input vector.
		 */
		cv::Mat_<T1> i() const;
		
		/**
		 * Returns a copy of the output vector.
		 */
		cv::Mat_<T2> o() const;
		
		/**
		 * Returns the dimension of the input vector.
		 */
		int iCols() const;
		
		/**
		 * Returns the dimension of the output vector.
		 */
		int oCols() const;
		
		/**
		 * Sets the input vector to the given vector.
		 */
		void i(const cv::Mat_<T1>& vec_i);
		
		/**
		 * Sets the output vector to the given vector.
		 */
		void o(const cv::Mat_<T2>& vec_o);
		
		/**
		 * Assignment operator
		 */
		ocv::VecPair<T1,T2> operator=(const ocv::VecPair<T1,T2>& vi) {
			_vec_i = vi._vec_i;
			_vec_o = vi._vec_o;
			return *this;
		}
		
		/**
		 * Returns a reference to the i-th dimension of the input vector.
		 */
		T1& operator[](int i) ;
		
		/**
		 * Returns a copy of the i-th input dimension.
		 */
		T1 operator[](int i) const;
		
		/**
		 * Returns a reference to the i-th dimension of the input vector.
		 */
		T1& i(int i);
		
		/**
		 * Returns a copy of the i-th input dimension.
		 */
		T1 i(int i) const;
		
		/**
		 * Returns a reference to the i-th dimension of the output vector.
		 */
		T2& o(int i);
		
		/**
		 * Returns a copy of the i-th output dimension.
		 */
		T2 o(int i) const;
		
	private:
	
		cv::Mat_<T1> _vec_i;
		cv::Mat_<T2> _vec_o;
		
	};
	
}
