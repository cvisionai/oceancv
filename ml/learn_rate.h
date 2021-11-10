#pragma once

#include <cmath>
#include <cassert>
#include <cstddef>

//#include "Types.hpp"

namespace ocv {

enum RATE_TYPES {
	CONSTANT,
	HARMONIC,
	LINEAR,
	EXPONENTIAL
};

/**
 * The LearnRate base class is just a virtual interface.
 * Use one of the derived spetialized classes instead!
 **/
template<class T>
class LearnRate {

protected:

	// Initial value to be returned for t = 0
	T _rate_0;
	
	// Final value to be returned for t = N
	T _rate_n;
	
	// The current rate at t
	T _rate_t;
	
	// Number of steps between start and end
	size_t _max_t;
	
	// The current time step
	size_t _cur_t;

public:
  
//	virtual ~LearnRate();
	
	// LearnRate for the next iteration
	virtual T operator()(void) = 0;
	
	// LearnRate for a given iteration
	virtual T operator()(size_t t) const = 0;

	void rate0(T rate_0);
	void rateN(T rate_n);
	void maxT(size_t max_t);
	void reset();
	void curT(size_t cur_t);

	T rate0() const;
	T rateN() const;
	T rateT() const;
	size_t maxT() const;
	size_t curT() const;
	
	virtual ocv::RATE_TYPES isA() const = 0;
    
};

// Constant learning rate that always returns the same rate, indepentend of t
template<class T>
class ConstantRate : public LearnRate<T> {

public:
	ConstantRate(T learn_rate);
	T operator ()(void);
	T operator ()(size_t t) const;
	ocv::RATE_TYPES isA() const;
	
};

// Harmonically decreasing learning rates (rate0/curT).
template<class T>
class HarmonicRate : public LearnRate<T> {

public:

	HarmonicRate(T initial_rate);
	T operator ()(void);
	T operator ()(size_t t) const;
	ocv::RATE_TYPES isA() const;
	
};

// Linearly decreasing learning rate
template<class T>
class LinearRate : public LearnRate<T> {

public:

	LinearRate(T initial_rate, T final_rate, size_t max_iterations);
	T operator ()(void);
	T operator ()(size_t t) const;
	ocv::RATE_TYPES isA() const;
	
};

// Exponentially decreasing learning rate
template<class T>
class ExponentialRate : public LearnRate<T> {

public:

	ExponentialRate(T initial_rate, T final_rate, size_t max_iterations);
	T operator ()(void);
	T operator ()(size_t iteration) const;
	ocv::RATE_TYPES isA() const;
		
};

}

