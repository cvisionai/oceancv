#include "oceancv/ml/linear_regression.h"

namespace ocv {

template<class T>
LinearRegression<T>::LinearRegression(const cv::Mat_<T> mat, cv::Mat_<T> y) {
	
	assert(mat.rows == y.rows && y.cols == 1);
	
	// It is assumed that the data is not centered
	this->_x_mean = ocv::malg<T>::mean(mat);
	this->_x_centered = mat;
	ocv::malg<T>::applyMean(this->_x_centered,this->_x_mean);
		
	this->_y_mean = ocv::valg<T>::mean(y);
	this->_y_centered = y - this->_y_mean;
	
	// Calculate coefficients
	cv::Mat_<T> x_trans, inv;
	ocv::malg<T>::transpose(this->_x_centered,x_trans);
	cv::invert(x_trans * this->_x_centered,inv);
	this->_coefficients = inv * x_trans * this->_y_centered;
	
	// Calculate intersect
	this->_intersect = this->_y_mean - ocv::valg<T>::sumElements(this->_coefficients * this->_x_mean);
	
	// Calculate sum of squared differences
	this->_ssd = 0;
	cv::Mat_<T> tmp_y = this->_y_centered + this->_y_mean;
	for(int i = 0; i < this->_x_centered.rows; i++) {
		T approx = ocv::valg<T>::sumElements((this->_x_centered(i) + this->_x_mean) * this->_coefficients) + this->_intersect;
		this->_ssd += std::pow(approx-tmp_y(i),2);
	}
	
	// Calculate determination
	T denominator = 0;
	for(int i = 0; i < this->_y_centered.rows; i++) {
		denominator += pow(this->_y_centered(i),2);
	}
	this->_determination = 1 - this->_ssd/denominator;
	
}

template<class T>
T LinearRegression<T>::predict(cv::Mat_<T> x) {
	assert(x.rows == this->_x_mean.rows && x.cols == 1);
	return ocv::valg<T>::sumElements(this->_coefficients*x) + this->_intersect;
}

template<class T>
cv::Mat_<T> LinearRegression<T>::predictMat(cv::Mat_<T> x) {
	assert(x.rows == this->_x_mean.rows);
	return x*this->_coefficients + this->_intersect;
}


template<class T>
LinearRegression<T>::~LinearRegression() {}

template<class T>
T LinearRegression<T>::determination() const {
	return this->_determination;
}

template<class T>
T LinearRegression<T>::sumOfSquaredDifferences() const {
	return this->_ssd;
}

template<class T>
T LinearRegression<T>::yIntersect() const {
	return this->_intersect;
}

template<class T>
cv::Mat_<T> LinearRegression<T>::regressionCoefficients() const {
	return this->_coefficients;
}


template class LinearRegression<float>;
template class LinearRegression<double>;

}
