#pragma once

#include "opencv2/opencv.hpp"

#include "oceancv/ml/mat_algorithms.h"

namespace ocv {

/**
 * Calculates the regression function for a given given dataset (X = mat,y).
 * Each row of X (i.e. each feature vector x) contains one observation of the covariables.
 * The i-th component of y contains the response for the i-th row of X.
 * Therefore X.rows == y.cols must hold (y.rows = 1).
 **/
template<class T>
class LinearRegression {
public:
	
	LinearRegression(const cv::Mat_<T> mat, cv::Mat_<T> y);
	
	~LinearRegression();
	
	// Predict the y-value for the given feature vector or matrix of vectors
	T predict(cv::Mat_<T> vec);
	cv::Mat_<T> predictMat(cv::Mat_<T> mat);
	
	// Getter functions
	T determination() const;
	T sumOfSquaredDifferences() const;
	T yIntersect() const;
	cv::Mat_<T> regressionCoefficients() const;
	
protected:
	
	T _determination;
	T _ssd;
	T _intersect;
	
	cv::Mat_<T> _coefficients;
	
	cv::Mat_<T> _x_centered;
	cv::Mat_<T> _y_centered;
	
	cv::Mat_<T> _x_mean;
	T _y_mean;
	
};

}
