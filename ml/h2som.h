#pragma once

#include <random>

#include "oceancv/ml/mat_pair_algorithms.h"
#include "oceancv/ml/learn_rate.h"
#include "oceancv/ml/metric.h"

namespace ocv {
	
	template <class T>
	class H2SOM {
	public:
		
		/**
		 * Constructor.
		 * @param features the training feature vectors
		 * @param iterations number of iterations (its a good idea to give a number in relation to the feature set size, e.g. 100 * features.rows)
		 * @param metric the distance metric to use to compute distances between feature vectors
		 * @param alpha the learn rate that incluences how much a prototype vector will be adapted (try ExponentialMetric<T>(0.99,0.01,iterations) if you have no clue)
		 * @param sigma how many neighbours on the current ring will be adapted, next to the best matching prototype (values will be rounded, so anything below 0.5 will have no effect).
		 * @param rings the number of rings to construct in the hyperbolic topology. Usually 2 or 3.
		 * @param neighbours the number of neighbours each prototype has. Usually 7 or 8.
		 */
		H2SOM(const cv::Mat_<T>&features, size_t iterations, ocv::Metric<T>* metric, ocv::LearnRate<T>* alpha, ocv::LearnRate<T>* sigma, const unsigned short rings = 3, const unsigned short neighbours = 8);
		
		/**
		 * Finds the best matching unit for a given feature vector by conducting a beam search.
		 * On each ring, the BMU is determined and only within its children (or its and its neighbours children)
		 * will the search be continued until the overall BMU is returned as the index of a
		 * centroid on the outermost ring.
		 */
		size_t beamBMUSearch(const cv::Mat_<T>& vec, bool with_neighbours = false);
		
		// Returns the topology of the H2SOM (One tuple per prototype: 0 -> poincare position, 1 -> ring number, 2 -> vector of neighbors)
		std::vector<std::tuple<cv::Point2d,int,std::vector<int>>> topology();
		
		// Getter
		cv::Mat_<T>& centroids();
		unsigned short numRings();
		unsigned short numNeighbours();
		ocv::Metric<T>* metric();

	protected:

		// Starts the clustering / trainig of the HSOM with the given features.
		void _cluster(const cv::Mat_<T>& features);
		
		// Addapts one bmu prototype and its neighbours.
		void _adapt(const int bmu, const int signal_idx, int ring, T cur_alpha, T cur_sigma, const cv::Mat_<T>& features);

		// Calculates how much hyperbolic space is needed to accomodate the given neighbours / rings settings
		T _calcMapWith(unsigned short neighbours);

		// This function is used by createH2SOMTopology to find the poincare positions of a new prototype
		cv::Point2d _rotateNode(cv::Point2d p1, cv::Point2d p2, T fSin, T fCos);

		// Calculates the topology of an H2SOM as well as the Poincare-Coordinates
		void _createH2SOMTopology();

		// Calculates the hyperbolic distance of two nodes using the projections on the Poincare-disk
		T _hyperdist(const size_t node_1, const size_t node_2)  const;
		
		
		// Index vector to the childs of each prototype.
		std::vector<std::vector<int>> _children;

		// Index vector to the neighbours of each prototype. Pairwise relation a <-> b ! Each connection is contained twice!
		std::map<int,std::vector<int>> _neighbours;
		
		// Vector of neighbourhood relations within one ring (node -> [prev,next])
		std::map<int,std::pair<int,int>> _same_ring_neighbours;
		
		// Index vector to the ring of each prototype
		std::vector<int> _prototype_to_ring;
		
		// Index vector to the poincare positions of the prototypes
		std::vector<cv::Point2d> _poincare;
		
		// Number of iterations
		size_t _iterations;

		ocv::Metric<T>* _met;

		// Learnrate alpha
		ocv::LearnRate<T>* _alpha;

		// Metric
		ocv::LearnRate<T>* _sigma;

		// Number of rings
		unsigned short _num_rings;

		// Number of neighbours per node
		unsigned short _num_neighbours;

		// The centroid / prototype vectors
		cv::Mat_<T> _centroids;

		// The indices of the prototypes belongin to a ring
		std::vector<int> _bounds;
		
		// Size of the used poincera space
		T _map_width;

	};
}
