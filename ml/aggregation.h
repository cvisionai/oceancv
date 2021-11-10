#pragma once

namespace ocv {

	enum AggregationType {
		MEAN,
		MEDIAN
	};

	enum DeviationType {
		VARIANCE,
		STDDEV
	};

	/**
	 * This class is wraps three numerical values to quantify aggregated
	 * measurements like mean, variance, and number of samples for their
	 * computation.
	 */
	template <class T>
	class Aggregation {
	public:
		Aggregation(T val = 0, T dev = 0, int num = 0, AggregationType at = AggregationType::MEAN, DeviationType dt = DeviationType::VARIANCE);
		T val() const;
		T dev() const;
		int num() const;
		AggregationType atype() const;
		DeviationType dtype() const;

	protected:
		T _val;
		T _dev;
		int _num;
		AggregationType _at;
		DeviationType _dt;

	};

}
