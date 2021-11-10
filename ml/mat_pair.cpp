#include "oceancv/ml/mat_pair.h"

namespace ocv {
	
	template<class T1, class T2>
	MatPair<T1,T2>::MatPair() {}
	
	template<class T1, class T2>
	MatPair<T1,T2>::MatPair(int rows, int i_cols, int o_cols, T1 val_i, T2 val_o) {
		this->_mat_i = cv::Mat_<T1>(rows,i_cols,val_i);
		this->_mat_o = cv::Mat_<T2>(rows,o_cols,val_o);
	}
	
	template<class T1, class T2>
	MatPair<T1,T2>::MatPair(const cv::Mat_<T1>& mat_i, const cv::Mat_<T2>& mat_o) {
		assert(mat_i.rows == mat_o.rows && mat_i.channels() == 1 && mat_o.channels() == 1);
		this->_mat_i = mat_i;
		this->_mat_o = mat_o;
	}
	
	template <class T1, class T2>
	int MatPair<T1,T2>::rows() const {
		return _mat_i.rows;
	}
	
	template <class T1, class T2>
	int MatPair<T1,T2>::iCols() const {
		return _mat_i.cols;
	}
	
	template <class T1, class T2>
	int MatPair<T1,T2>::oCols() const {
		return _mat_o.cols;
	}
	
	template <class T1, class T2>
	cv::Mat_<T1> MatPair<T1,T2>::i() const {
		return _mat_i;
	}
	
	template <class T1, class T2>
	cv::Mat_<T1>& MatPair<T1,T2>::i() {
		return _mat_i;
	}

	template <class T1, class T2>
	cv::Mat_<T2> MatPair<T1,T2>::o() const {
		return _mat_o;
	}

	template <class T1, class T2>
	cv::Mat_<T2>& MatPair<T1,T2>::o() {
		return _mat_o;
	}

	template <class T1, class T2>
	cv::Mat_<T1> MatPair<T1,T2>::iRow(int row) const {
		assert(row < _mat_i.rows);
		cv::Mat_<T1> ret(1,_mat_i.cols);
		std::copy(_mat_i.begin()+(row*_mat_i.cols),_mat_i.begin()+((row+1)*_mat_i.cols),ret.begin());
		return ret;
	}
	
	template <class T1, class T2>
	T1& MatPair<T1,T2>::i(int row, int col) {
		assert(row < _mat_i.rows);
		assert(col < _mat_i.cols);
		return _mat_i(row,col);
	}
	
	template <class T1, class T2>
	T1 MatPair<T1,T2>::i(int row, int col) const {
		assert(row < _mat_i.rows);
		assert(col < _mat_i.cols);
		return _mat_i(row,col);
	}
	
	template <class T1, class T2>
	void MatPair<T1,T2>::i(int row, int col, const T1 value) {
		assert(row < _mat_i.rows);
		assert(col < _mat_i.cols);
		_mat_i(row,col) = value;
	}
	
	template <class T1, class T2>
	cv::Mat_<T2> MatPair<T1,T2>::oRow(int row) const {
		assert(row < _mat_o.rows);
		cv::Mat_<T2> ret(1,_mat_o.cols);
		std::copy(_mat_o.begin()+(row*_mat_o.cols),_mat_o.begin()+((row+1)*_mat_o.cols),ret.begin());
		return ret;
	}
	
	template <class T1, class T2>
	T2& MatPair<T1,T2>::o(int row, int col) {
		assert(row < _mat_o.rows);
		assert(col < _mat_o.cols);
		return _mat_o(row,col);
	}
	
	template <class T1, class T2>
	T2 MatPair<T1,T2>::o(int row, int col) const {
		assert(row < _mat_o.rows);
		assert(col < _mat_o.cols);
		return _mat_o(row,col);
	}
	
	template <class T1, class T2>
	void MatPair<T1,T2>::o(int row, int col, const T2 value) {
		assert(row < _mat_o.rows);
		assert(col < _mat_o.cols);
		_mat_o(row,col) = value;
	}
	
	template <class T1, class T2>
	cv::MatConstIterator_<T1> MatPair<T1,T2>::iBegin(int row) const {
		assert(row < _mat_i.rows);
		return _mat_i.begin() + row * _mat_i.cols;
	}
	
	template <class T1, class T2>
	cv::MatConstIterator_<T2> MatPair<T1,T2>::oBegin(int row) const {
		assert(row < _mat_o.rows);
		return _mat_o.begin() + row * _mat_o.cols;
	}
	
	template <class T1, class T2>
	cv::MatConstIterator_<T1> MatPair<T1,T2>::iEnd(int row) const {
		assert(row < _mat_i.rows);
		return _mat_i.begin() + (row + 1) * _mat_i.cols;
	}
	
	template <class T1, class T2>
	cv::MatConstIterator_<T2> MatPair<T1,T2>::oEnd(int row) const {
		assert(row < _mat_o.rows);
		return _mat_o.begin() + (row + 1) * _mat_o.cols;
	}
	
	
	template <class T1, class T2>
	ocv::VecPair<T1,T2> MatPair<T1,T2>::pair(int row) const {
		assert(row < _mat_i.rows && row < _mat_o.rows);
		return ocv::VecPair<T1,T2>(this->iRow(row),this->oRow(row));
	}
	
	template <class T1, class T2>
	void MatPair<T1,T2>::pair(int row, const ocv::VecPair<T1,T2>& vp) {
		assert(row < _mat_i.rows && row < _mat_o.rows);
		assert(vp.iCols() == _mat_i.cols && vp.oCols() == _mat_o.cols);
		std::copy(vp.i().begin(),vp.i().end(),_mat_i.begin()+(row*_mat_i.cols));
		std::copy(vp.o().begin(),vp.o().end(),_mat_o.begin()+(row*_mat_o.cols));
	}
	
	template <class T1, class T2>
	void MatPair<T1,T2>::pair(int row, const cv::Mat_<T1>& vec_i, const cv::Mat_<T2>& vec_o) {
		assert(row < _mat_i.rows && row < _mat_o.rows);
		assert(vec_i.cols == _mat_i.cols && vec_o.cols == _mat_o.cols);
		assert(vec_i.channels() == 1 && vec_o.channels() == 1);
		std::copy(vec_i.begin(),vec_i.end(),_mat_i.begin()+(row*_mat_i.cols));
		std::copy(vec_o.begin(),vec_o.end(),_mat_o.begin()+(row*_mat_o.cols));
	}
	
	
	template class MatPair<double,double>;
	template class MatPair<double,float>;
	template class MatPair<double,int>;
//	template class MatPair<double,size_t>;
	
	template class MatPair<float,double>;
	template class MatPair<float,float>;
	template class MatPair<float,int>;
//	template class MatPair<float,size_t>;
	
	template class MatPair<int,double>;
	template class MatPair<int,float>;
	template class MatPair<int,int>;
//	template class MatPair<int,size_t>;
	
//	template class MatPair<size_t,double>;
//	template class MatPair<size_t,float>;
//	template class MatPair<size_t,int>;
//	template class MatPair<size_t,size_t>;
	
}
