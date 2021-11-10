#include "oceancv/ml/neural_gas.h"

namespace ocv {

	template <class T>
	NeuralGas<T>::NeuralGas(const cv::Mat_<T>& features, size_t cluster_count, size_t iterations) : _features(features), _cluster_count(cluster_count), _iterations(iterations) {}
	
	template <class T>
	void NeuralGas<T>::cluster(ocv::Metric<T>* met, ocv::LearnRate<T>* lr_eps, ocv::LearnRate<T>* lr_lam) {
		
		// Create random integer generator
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> randInt(0, _features.size()-1);
		
		// Initialize centroids with random items from the feature set
		_centroids.clear();
		for(int i = 0; i < _cluster_count; i++) {
			_centroids.push(_features[randInt(gen)]);
		}

		// Clustering
		#pragma omp parallel for
		for(size_t i = 0; i < _iterations; i++) {
			
			std::vector<std::pair<T,int>> order;
			
			int signal_idx = randInt(gen);
			
			// Find closest item
			for(int j = 0; j < _centroids.size(); j++) {
				order.push_back(std::pair<T,int>(met->distance(_features[signal_idx],_centroids[j]),j));
			}
			sort(order.begin(),order.end(),[](const std::pair<T,int> &a,const std::pair<T,int> &b) {
				return a.first<b.first;
			});
			
			// Adapt centroids
			T epsilon = lr_eps->iterate();
			T lambda = lr_lam->iterate();
			for(int j = 0; j < order.size(); j++) {
				_centroids[order[j].second] += (epsilon*T(exp(T(-j)/lambda)))*(_features[signal_idx]-_centroids[order[j].second]);
			}
			
		}
		
	}
	
	template <class T>
	void NeuralGas<T>::features(const cv::Mat_<T>& features) {
		_features = features;
	}
	
	template <class T>
	void NeuralGas<T>::iterations(size_t iterations) {
		_iterations = iterations;
	}
	
	template <class T>
	void NeuralGas<T>::clusterCount(size_t cluster_count) {
		_cluster_count = cluster_count;
	}
	
	
	template <class T>
	cv::Mat_<T> NeuralGas<T>::centroids() const {
		return _centroids;
	}
	
	template <class T>
	size_t NeuralGas<T>::iterations() const {
		return _iterations;
	}
	
	template <class T>
	size_t NeuralGas<T>::clusterCount() const {
		return _cluster_count;
	}
	
}
