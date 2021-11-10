#pragma once

#include <random>

#include "oceancv/ml/learn_rate.h"
#include "oceancv/ml/metric.h"

namespace ocv {

	template <class T>
	class NeuralGas {
	public:
	
		// Initializes a NeuralGas clustering
		NeuralGas(const cv::Mat_<T>& features, size_t cluster_count, size_t iterations);
		
		// Performs the actual clustering step
		void cluster(ocv::Metric<T>* met, ocv::LearnRate<T>* lr_eps, ocv::LearnRate<T>* lr_lam);
		
		// Getter and setter
		void features(const cv::Mat_<T>& features);
		void iterations(size_t iterations);
		void clusterCount(size_t cluster_count);
		
		cv::Mat_<T> centroids() const;
		size_t iterations() const;
		size_t clusterCount() const;
		
	private:
		
		// The feature vectors to cluster
		const cv::Mat_<T>& _features;
		
		// The resulting prototypes
		cv::Mat_<T> _centroids;
		
		// The number of iterations to run the clustering for
		size_t _iterations;
		
		// How many clusters are expected
		size_t _cluster_count;
		
	};
		
}
