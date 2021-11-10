#include "oceancv/ml/vec_pair.h"

namespace ocv {
	
	template <class T1,class T2>
	VecPair<T1,T2>::VecPair(cv::Mat_<T1> vec_i, cv::Mat_<T2> vec_o) {
		assert(vec_i.rows == 1 && vec_i.channels() == 1 && vec_o.rows == 1 && vec_o.channels() == 1);
		_vec_i = vec_i;
		_vec_o = vec_o;
	}
	template <class T1,class T2>
	cv::Mat_<T1> VecPair<T1,T2>::i() const {
		return _vec_i;
	}
	template <class T1,class T2>
	cv::Mat_<T2> VecPair<T1,T2>::o() const {
		return _vec_o;
	}
	template <class T1, class T2>
	cv::Mat_<T1>& VecPair<T1,T2>::i() {
		return _vec_i;
	}
	template <class T1, class T2>
	cv::Mat_<T2>& VecPair<T1,T2>::o() {
		return _vec_o;
	}
	template <class T1, class T2>
	int VecPair<T1,T2>::iCols() const {
		return _vec_i.cols;
	}
	template <class T1, class T2>
	int VecPair<T1,T2>::oCols() const {
		return _vec_o.cols;
	}
	template <class T1, class T2>
	void VecPair<T1,T2>::i(const cv::Mat_<T1>& vec_i) {
		assert(vec_i.rows == 1 && vec_i.channels() == 1);
		this->_vec_i = vec_i;
	}
	template <class T1, class T2>
	void VecPair<T1,T2>::o(const cv::Mat_<T2>& vec_o) {
		assert(vec_o.rows == 1 && vec_o.channels() == 1);
		this->_vec_o = vec_o;
	}
	template <class T1, class T2>
	T1& VecPair<T1,T2>::operator[](int i) {
		assert(i < _vec_i.cols);
		return _vec_i(i);
	}
	template <class T1, class T2>
	T1 VecPair<T1,T2>::operator[](int i) const {
		assert(i < _vec_i.cols);
		return _vec_i(i);
	}
	template <class T1, class T2>
	T1& VecPair<T1,T2>::i(int i) {
		assert(i < _vec_i.cols);
		return _vec_i(i);
	}
	template <class T1, class T2>
	T1 VecPair<T1,T2>::i(int i) const {
		assert(i < _vec_i.cols);
		return _vec_i(i);
	}
	template <class T1, class T2>
	T2& VecPair<T1,T2>::o(int i) {
		assert(i < _vec_o.cols);
		return _vec_o(i);
	}
	template <class T1, class T2>
	T2 VecPair<T1,T2>::o(int i) const {
		assert(i < _vec_o.cols);
		return _vec_o(i);
	}

	
	template class VecPair<double,double>;
	template class VecPair<double,float>;
	template class VecPair<double,int>;
//	template class VecPair<double,size_t>;
	
	template class VecPair<float,double>;
	template class VecPair<float,float>;
	template class VecPair<float,int>;
//	template class VecPair<float,size_t>;
	
	template class VecPair<int,double>;
	template class VecPair<int,float>;
	template class VecPair<int,int>;
//	template class VecPair<int,size_t>;
	
//	template class VecPair<size_t,double>;
//	template class VecPair<size_t,float>;
//	template class VecPair<size_t,int>;
//	template class VecPair<size_t,size_t>;

	
}
