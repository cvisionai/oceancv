#include "oceancv/ml/h2som.h"

namespace ocv {

	template<class T>
	H2SOM<T>::H2SOM(const cv::Mat_<T>& features, size_t iterations, ocv::Metric<T>* met, ocv::LearnRate<T>* alpha, ocv::LearnRate<T>* sigma, const unsigned short rings, const unsigned short neighbours) : _iterations(iterations), _met(met), _alpha(alpha), _sigma(sigma), _num_rings(rings), _num_neighbours(neighbours) {

		assert(_num_neighbours > 3);
		assert(_num_rings > 0);
		
		// Setup the ring boundaries, the prototype neighbourhood relations and the poincare positions
		this->_createH2SOMTopology();
		
		// Total number of nodes in the entire H2SOM
		const int num_som_nodes = _neighbours.size();

		// Compute required size of poincare space
		this->_map_width = _calcMapWith(_num_neighbours);
		

		// Initialize the cv::Mat for the centroids with random feature vectors from the training set
		this->_centroids = cv::Mat_<T>(num_som_nodes,features.cols);
		
		// Calculate the root prototype which is the mean of all features
		ocv::malg<T>::mean(features).copyTo(this->_centroids.row(0));
		
		// Initialize the prototypes with random data points
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> randInt(0, features.rows - 1);
		for(int i = 1; i < num_som_nodes; i++) {
			features.row(randInt(gen)).copyTo(this->_centroids.row(i));
		}
		
		// Execute the clustering
		this->_cluster(features);
		
	}
	
	template<class T>
	size_t H2SOM<T>::beamBMUSearch(const cv::Mat_<T>& vec, bool with_neighbours) {
	
		if(_num_rings == 0)
			return 0;
		
		std::vector<size_t> candidates(_num_neighbours);
		std::iota(candidates.begin(),candidates.end(),1);
		
		size_t bmu = 0;
		T min_dist, tmp_dist;
		
		for(int ring = 1; ring <= _num_rings; ring++) {
			
			// Find closest BMU among candidates
			bmu = candidates[0];
			min_dist = _met->distance(vec, this->_centroids.row(candidates[0]));
			for(size_t j = 1; j < candidates.size(); j++) {
				tmp_dist = _met->distance(vec, this->_centroids.row(candidates[j]));
				if(tmp_dist < min_dist) {
					min_dist = tmp_dist;
					bmu = j;
				}
			}
			
			// Update candidates
			candidates = {bmu};
			if(with_neighbours) {
				candidates.push_back(std::get<0>(_same_ring_neighbours[bmu]));
				candidates.push_back(std::get<1>(_same_ring_neighbours[bmu]));
			}
			
		}
		
		return bmu;
	
	}

	template<class T>
	cv::Mat_<T>& H2SOM<T>::centroids() {
		return this->_centroids;
	}

	template<class T>
	unsigned short H2SOM<T>::numRings()	{
		return this->_num_rings;
	}

	template<class T>
	unsigned short H2SOM<T>::numNeighbours() {
		return this->_num_neighbours;
	}

	template<class T>
	Metric<T>* H2SOM<T>::metric() {
		return this->_met;
	}

	template<class T>
	std::vector<std::tuple<cv::Point2d,int,std::vector<int>>> H2SOM<T>::topology() {
		
		// Group poincare position, ring index and prototype neighbors to one tuple per prototype
		std::vector<std::tuple<cv::Point2d,int,std::vector<int>>> ret;
		for(int i = 0; i < _centroids.rows; i++) {
			ret.push_back(std::make_tuple(_poincare[i],_prototype_to_ring[i],_neighbours[i]));
		}
		
		return ret;
		
	}

	template<class T>
	void H2SOM<T>::_cluster(const cv::Mat_<T>& features) {
		
		// Create random integer generator
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> randInt(0, features.rows-1);
		
		int bmu;
		T minv;
		
		for(int ring = 1; ring < (_num_rings + 1); ring++) {
			
			std::cout << "RING " << ring << std::endl;
			
			for(size_t i = 0; i < _iterations; i++) {
				
				// Get a random feature vector
				const int signal_idx = randInt(gen);
				
				// Find closest prototype vector in this ring -> BMU
				bmu = 0;
				minv = std::numeric_limits<T>::max();
				for(int j = this->_bounds[ring - 1]; j < this->_bounds[ring]; j++) {
					T tmpval = _met->distance(features.row(signal_idx), this->_centroids.row(j));
					if(tmpval < minv) {
						minv = tmpval;
						bmu = j;
					}
				}
				
				// Adapt BMU
				_adapt(bmu, signal_idx, ring, _alpha->operator()(), _sigma->operator()(), features);
				
			}
		
			// Initialize next ring
			if(ring != _num_rings) {
				
				// Find all prototypes on the current ring
				for(size_t p = 0; p < _prototype_to_ring.size(); p++) {

					if(_prototype_to_ring[p] != ring)
						continue;
					
					// And copy their prototype vector to all their children
					for(size_t j = 0; j < this->_neighbours[p].size(); j++) {
						if(_prototype_to_ring[j] == ring+1) {
							this->_centroids.row(p).copyTo(this->_centroids.row(this->_neighbours[p][j]));
						}
					}

				}
				
				// Reset the learn rates
				_alpha->reset();
				_sigma->reset();
				
			}
		}
		
	}

	template<class T>
	void H2SOM<T>::_adapt(const int bmu, const int signal_idx, int ring, T cur_alpha, T cur_sigma, const cv::Mat_<T>& features) {
		
		// Update BMU
		this->_centroids.row(bmu) += cur_alpha * (features.row(signal_idx) - this->_centroids.row(bmu));
		
		// How far within the current ring you move towards the "left"/"right"
		T adaption_factor;
		const int range = int(round(cur_sigma));
		
		int prev = bmu - 1;
		if(prev < _bounds[ring])
			prev = _bounds[ring+1]-1;
		int next = bmu + 1;
		if(next >= _bounds[ring+1])
			next = _bounds[ring];
		
		for(int i = 0; i < range; i++) {
			
			adaption_factor = _map_width * _hyperdist(next, bmu);
			adaption_factor = cur_alpha * exp(-(adaption_factor * adaption_factor) / (2.0 * cur_sigma * cur_sigma));
			this->_centroids.row(next) += adaption_factor * (features.row(signal_idx) - this->_centroids.row(next));
			
			adaption_factor = _map_width * _hyperdist(prev, bmu);
			adaption_factor = cur_alpha * exp(-(adaption_factor * adaption_factor) / (2.0 * cur_sigma * cur_sigma));
			this->_centroids.row(prev) += adaption_factor * (features.row(signal_idx) - this->_centroids.row(prev));
			
			prev--;
			if(prev < _bounds[ring])
				prev = _bounds[ring+1]-1;
			next++;
			if(next >= _bounds[ring+1])
				next = _bounds[ring];
			
		}

	}

	template<class T>
	T H2SOM<T>::_calcMapWith(unsigned short neighbours) {
		return acosh(1.0 + 2.0 * (std::sqrt(1.0 - 4.0 * pow(sin(M_PI / neighbours), 2.0)) * cos(2.0 * M_PI / neighbours) * std::sqrt(1.0 - 4.0 * pow(sin(M_PI / neighbours), 2.0)) * cos(2.0 * M_PI / neighbours) + std::sqrt(1.0 - 4.0 * pow(sin(M_PI / neighbours), 2.0)) * cos(2.0 * M_PI / neighbours) * std::sqrt(1.0 - 4.0 * pow(sin(M_PI / neighbours), 2.0)) * cos(2.0 * M_PI / neighbours)) / (1.0 - (std::sqrt(1.0 - 4.0 * pow(sin(M_PI / neighbours), 2.0)) * cos(2.0 * M_PI / neighbours) * std::sqrt(1.0 - 4.0 * pow(sin(M_PI / neighbours), 2.0)) * cos(2.0 * M_PI / neighbours) + std::sqrt(1.0 - 4.0 * pow(sin(M_PI / neighbours), 2.0)) * cos(2.0 * M_PI / neighbours) * std::sqrt(1.0 - 4.0 * pow(sin(M_PI / neighbours), 2.0)) * cos(2.0 * M_PI / neighbours))));
	}

	template <class T>
	cv::Point2d H2SOM<T>::_rotateNode(cv::Point2d p1, cv::Point2d p2, T fSin, T fCos) {

		T fX1 = p1.x;
		T fY1 = p1.y;
		T fX2 = p2.x;
		T fY2 = p2.y;
		
		T fU, fV, fQ, fx, fy, fT;

	    // Apply transformation to move the center into origin of H2
	    fU = 1. - fX1*fX2 - fY1*fY2;
	    fV =      fX2*fY1 - fX1*fY2;
	    fQ = fU*fU + fV*fV;
	    fx = fX2 - fX1;
	    fy = fY2 - fY1;
	    fX2 = ( fx*fU + fy*fV) / fQ;
	    fY2 = (-fx*fV + fy*fU) / fQ;

	    // Rotate point 2 by angle around origin
	    fT  = fCos*fX2 - fSin*fY2;
	    fY2 = fSin*fX2 + fCos*fY2;
	    fX2 = fT;

	    // Backtransform (replace point 1 by mirror image)
	    fU = 1. + fX1*fX2 + fY1*fY2;
	    fV =     -fX2*fY1 + fX1*fY2;
	    fQ = fU*fU + fV*fV;
	    fx = fX2 + fX1;
	    fy = fY2 + fY1;

	    // Finally set the new coordinate
	    T v0 = ( fx*fU + fy*fV) / fQ;
	    T v1 = (-fx*fV + fy*fU) / fQ;

	    return cv::Point2d(v0,v1);
		
	}

	template <class T>
	void H2SOM<T>::_createH2SOMTopology() {

		T fRadius = sqrt(1.0 - 4.0 * pow(sin(M_PI / _num_neighbours), 2));
		T fAngle = 2.0*M_PI/_num_neighbours;
		T fSin = sin(fAngle);
		T fCos = cos(fAngle);

		// Reset topology
		_same_ring_neighbours = {};
		_prototype_to_ring = {};
		_neighbours = {};
		_poincare = {};
		_bounds = {};
		
		// Create center node
		_bounds.push_back(0);
		_prototype_to_ring.push_back(0);
		_poincare.push_back({0,0});
		
		// Create first ring
		_bounds.push_back(1);
		for(int i = 1; i <= _num_neighbours; i++) {
			
			_prototype_to_ring.push_back(1);
			_poincare.push_back(cv::Point2d(fRadius*cos((i-1)*fAngle),fRadius*sin((i-1)*fAngle)));
			
			// Connect to parent
			_neighbours[i].push_back(0);
			_neighbours[0].push_back(i);
			
			// If not first on ring, connect to previous on this ring
			if(i > 1) {
				_neighbours[i].push_back(i-1);
				_neighbours[i-1].push_back(i);
				_same_ring_neighbours[i].first = i-1;
				_same_ring_neighbours[i-1].second = i;
			}
			
		}
		
		// Connect first to last on ring 1
		_neighbours[_num_neighbours].push_back(1);
		_neighbours[1].push_back(_num_neighbours);
		_same_ring_neighbours[_num_neighbours].first = 1;
		_same_ring_neighbours[1].second = _num_neighbours;
		
		
		// Create remaining rings
		int ring_idx_0 = 1; 				// First node index of the last ring
		int ring_idx_n = _num_neighbours+1; // Last node index of the last ring
		int cur_idx = _num_neighbours; 		// Index of the node pushed last
		int num_to_add;
		
		for(int ring = 2; ring <= _num_rings; ring++) {
			
			// Add ring
			_bounds.push_back(ring_idx_n);
			
			// Connect first on new ring to last on previous ring
			_neighbours[cur_idx+1].push_back(ring_idx_n-1);
			_neighbours[ring_idx_n-1].push_back(cur_idx+1);
			
			for(int parent = ring_idx_0; parent < ring_idx_n; parent++) {
			
				// How many nodes need to be added?
				num_to_add = _num_neighbours - _neighbours[parent].size();
				
				for(int i = 0;  i < num_to_add; i++) {
					
					cur_idx++;
					
					// Connect to parent
					_neighbours[cur_idx].push_back(parent);
					_neighbours[parent].push_back(cur_idx);
					
					// If not first on ring, connect to previous on this ring
					if(parent > ring_idx_0 || i > 0) {
						_neighbours[cur_idx].push_back(cur_idx-1);
						_neighbours[cur_idx-1].push_back(cur_idx);
						_same_ring_neighbours[cur_idx].first = cur_idx-1;
						_same_ring_neighbours[cur_idx-1].second = cur_idx;
					}
					
					// Add coordinate
					_poincare.push_back(_rotateNode(_poincare[parent],_poincare[cur_idx-1],fSin,fCos));
					
					// Store ring number
					_prototype_to_ring.push_back(ring);
					
				}
				
				// Connect last child to second parent
				if(parent < ring_idx_n) {
					_neighbours[cur_idx].push_back(parent+1);
					_neighbours[parent+1].push_back(cur_idx);
				}
				
			}
			
			// Increase ring limits for the next iteration
			ring_idx_0 = ring_idx_n;
			ring_idx_n = cur_idx+1;
			
		}
		
		// Store last ring's end
		_bounds.push_back(cur_idx+1);
		
	}
	
	template<class T>
	T H2SOM<T>::_hyperdist(const size_t node_1, const size_t node_2) const {
		T xr = _poincare[node_1].x;
		T xi = _poincare[node_1].y;
		T yr = _poincare[node_2].x;
		T yi = _poincare[node_2].y;
		return acosh(1 + 2*( (xr-yr)*(xr-yr) + (xi-yi)*(xi-yi) ) / ((1 - (xr*xr+xi*xi))*(1 - (yr*yr+yi*yi))));
	}
	
	template class H2SOM<float>;
	template class H2SOM<double>;

}
