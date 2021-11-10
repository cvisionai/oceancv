#include "oceancv/ml/aggregation.h"

namespace ocv {

	template <class T>
	Aggregation<T>::Aggregation(T val, T dev, int num, AggregationType at, DeviationType dt) : _val(val), _dev(dev), _num(num), _at(at), _dt(dt) {}

	template <class T>
	T Aggregation<T>::val() const {
		return _val;
	}

	template <class T>
	T Aggregation<T>::dev() const {
		return _dev;
	}

	template <class T>
	int Aggregation<T>::num() const {
		return _num;
	}

	template <class T>
	AggregationType Aggregation<T>::atype() const {
		return _at;
	}

	template <class T>
	DeviationType Aggregation<T>::dtype() const {
		return _dt;
	}


	template class Aggregation<double>;
	template class Aggregation<float>;

}
