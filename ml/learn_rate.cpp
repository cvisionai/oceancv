#include "oceancv/ml/learn_rate.h"

namespace ocv {

template<class T>
inline void LearnRate<T>::rate0(T rate_0) {
    this->_rate_0 = rate_0;
}

template<class T>
inline void LearnRate<T>::rateN(T rate_n) {
    this->_rate_n = rate_n;
}

template<class T>
inline void LearnRate<T>::maxT(size_t max_t) {
    this->_max_t = max_t;
}

template<class T>
inline void LearnRate<T>::reset() {
    this->_cur_t = 0;
    this->_rate_t = _rate_0;
}

template<class T>
inline void LearnRate<T>::curT(size_t cur_t) {
    this->_cur_t = cur_t;
}

template<class T>
inline T LearnRate<T>::rate0() const {
    return this->_rate_0;
}

template<class T>
inline T LearnRate<T>::rateN() const {
    return this->_rate_n;
}

template<class T>
inline T LearnRate<T>::rateT() const {
    return this->_cur_t;
}

template<class T>
inline size_t LearnRate<T>::maxT() const {
    return this->_max_t;
}

template<class T>
inline size_t LearnRate<T>::curT() const {
    return this->_cur_t;
}


template<class T>
ConstantRate<T>::ConstantRate(T rate_0) {
	assert(rate_0 >= 0.0);
	this->_cur_t = 0;
	this->_max_t = -1;
	this->_rate_0 = rate_0;
	this->_rate_t = rate_0;
}

template<class T>
inline T ConstantRate<T>::operator()(void) {
	this->_cur_t++;
	return this->_rate_t;
}

template<class T>
inline T ConstantRate<T>::operator()(size_t iteration) const {
	return this->_rate_t;
}

template<class T>
inline 	ocv::RATE_TYPES ConstantRate<T>::isA() const {
	return CONSTANT;
}


template<class T>
HarmonicRate<T>::HarmonicRate(T rate_0) {
	assert(rate_0 > 0.0);
	this->_rate_0 = rate_0;
	this->_cur_t = 0;
	this->_max_t = -1;
}

template<class T>
inline T HarmonicRate<T>::operator()(void) {
	this->_cur_t++;
	this->_rate_t = this->_rate_0 / static_cast<T>(this->_cur_t+1);
	return this->_rate_t;
}

template<class T>
inline T HarmonicRate<T>::operator()(size_t cur_t) const {
	return this->_rate_0 / static_cast<T>(cur_t+1);
}

template<class T>
inline ocv::RATE_TYPES HarmonicRate<T>::isA() const {
	return HARMONIC;
}


template<class T>
LinearRate<T>::LinearRate(T rate_0, T rate_n, size_t max_t) {
	assert(rate_0 > 0.0);
	assert(rate_n >= 0.0);
	assert(max_t > 0);
	this->_rate_0 = rate_0;
	this->_rate_n = rate_n;
	this->_max_t = max_t;
	this->_cur_t = 0;
}

template<class T>
inline T LinearRate<T>::operator()(void) {
	assert(this->_cur_t < this->_max_t);
	this->_rate_t = this->operator()(this->_cur_t);
	this->_cur_t++;
	return this->_rate_t;
}

template<class T>
inline T LinearRate<T>::operator()(size_t cur_t) const {
	assert(cur_t < this->_max_t);
	return ((this->_rate_n - this->_rate_0) / static_cast<T>(this->_max_t-1)) * static_cast<T>(cur_t) + this->_rate_0;
}

template<class T>
inline ocv::RATE_TYPES LinearRate<T>::isA() const {
	return LINEAR;
}


template<class T>
ExponentialRate<T>::ExponentialRate(T rate_0, T rate_n, size_t max_t) {
	assert(rate_0 > 0.0);
	assert(rate_n > 0.0);
	assert(max_t > 0);
	this->_rate_0 = rate_0;
	this->_rate_t = rate_0;
	this->_rate_n = rate_n;
	this->_cur_t = 0;
	this->_max_t = max_t;
}

template<class T>
inline T ExponentialRate<T>::operator()(void) {
	assert(this->_cur_t < this->_max_t);
	if(this->_rate_0 > this->_rate_n)
		this->_rate_t *= std::pow(static_cast<double>(this->_rate_n/this->_rate_0),(1.0/static_cast<double>(this->_max_t)));
	this->_cur_t++;
	return this->_rate_t;
}

template<class T>
inline T ExponentialRate<T>::operator()(size_t cur_t) const {
	assert(cur_t < this->_max_t);
	return this->_rate_0*pow((double)(this->_rate_n/this->_rate_0), static_cast<double>(cur_t)/static_cast<double>(this->_max_t-1));
}

template<class T>
inline ocv::RATE_TYPES ExponentialRate<T>::isA() const {
	return EXPONENTIAL;
}


template class LearnRate<float>;
template class LearnRate<double>;

template class ConstantRate<float>;
template class ConstantRate<double>;

template class HarmonicRate<float>;
template class HarmonicRate<double>;

template class LinearRate<float>;
template class LinearRate<double>;

template class ExponentialRate<float>;
template class ExponentialRate<double>;

}
