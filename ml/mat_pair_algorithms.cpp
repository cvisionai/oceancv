#include "oceancv/ml/mat_pair_algorithms.h"

namespace ocv {

	template<class T1, class T2>
	size_t mpalg<T1,T2>::bestMatchIndexOutput(const ocv::MatPair<T1,T2>& mp, const cv::Mat_<T2>& vec, const ocv::Metric<T2>& metric, int start_row, int end_row) {
		
		if(end_row < 0)
			end_row = mp.i().rows;
		
		T2 tmp_dist, min_dist = metric.distance(vec, mp.o().row(start_row));
	    int best_id = 0;
	    for(int i = start_row+1; i < end_row; i++) {
			
	        tmp_dist = metric.distance(vec, mp.o().row(i));
	        if(tmp_dist < min_dist) {
	            min_dist = tmp_dist;
	            best_id = i;
	        }
	    }
	    return best_id;
		
	}
	
	template<class T1, class T2>
	int mpalg<T1,T2>::covarianceMat(const ocv::MatPair<T1,T2>& mp, cv::Mat_<T1>& dst, T2 class_label, int idx) {
		
	    assert(mp.rows() > 0);
	    assert(mp.iCols() > 0);
	    assert(mp.rows() < idx);
		
	    size_t counter = 0;
	    dst = cv::Mat_<T1>(mp.iCols(), mp.iCols(), (T1) 0);
	    cv::Mat_<T1> allmean = ocv::malg<T1>::mean(mp.i());

	    for(int i = 0; i < mp.rows(); i++) {
	        if(mp.o(i,idx) == class_label) {
	        	counter++;
		        for(int j = 0; j < mp.iCols(); j++) {
	    	        for(int k = 0; k < mp.iCols(); k++) {
	    	        	dst(j,k) = (1. - 1.f / counter) * dst(j,k) + 1.f / counter * (mp.i(i,j) - allmean(j)) * (mp.i(i,k) - allmean(k));
	    	        }
	    	    }
	    	}
	    }
		
		return counter;
	
	}
	
	template<class T1, class T2>
	int mpalg<T1,T2>::mean(const ocv::MatPair<T1,T2>& mp, cv::Mat_<T1>& mean, T2 class_label, int o_col_idx) {
		
		assert(mp.oCols() > o_col_idx);
		
	    size_t counter = 0;
	    mean = cv::Mat_<T1>(1, mp.iCols(), static_cast<T1>(0));
	    for(int i = 0; i < mp.rows(); i++) {
	        if(mp.o(i,o_col_idx) == class_label) {
	        	counter++;
		        mean *= (T1) (1. - 1.f / counter);
		        std::transform(mean.begin(),mean.end(),mp.iBegin(i),mean.begin(),[&counter](T1 a, T1 b) -> T1 { return a + T1(1. / counter) * b; });
//	    	    dst += (mp.i(i) * (T1) (1.f / counter));
	    	}
	    }
	    return counter;
		
	}

	template<class T1, class T2>
	int mpalg<T1,T2>::zscore(const ocv::MatPair<T1,T2>& mp, cv::Mat_<T1>& mean, cv::Mat_<T1>& variance, T2 class_label, int o_col_idx) {
		
		size_t counter = mpalg<T1,T2>::mean(mp, mean, class_label, o_col_idx);
		
		std::vector<T1> tmp(mp.iCols(),T1(0));
		variance = cv::Mat_<T1>(1,mp.iCols(),T1(0));
		
	    for(int i = 0; i < mp.rows(); i++) {
	    	if(class_label == mp.o(i,o_col_idx)) {
	    		std::transform(mp.iBegin(i), mp.iEnd(i), mean.begin(), tmp.begin(), std::minus<T1>());
	    		std::transform(variance.begin(), variance.end(), tmp.begin(), variance.begin(), [](T1 a, T1 b) -> T1 { return a + (b * b); });
			}
	    }
		
	    std::transform(variance.begin(), variance.end(), variance.begin(), std::bind2nd(std::divides<T1>(), T1(counter)));
		
		return counter;
		
	}

	template<class T1, class T2>
	bool mpalg<T1,T2>::isNaN(const ocv::MatPair<T1,T2>& mp) {
		if(!cv::checkRange(mp.i()))
			return true;
		return !cv::checkRange(mp.o());
	}

	template<class T1, class T2>
	void mpalg<T1,T2>::selectInputColumns(const ocv::MatPair<T1,T2> mp, ocv::MatPair<T1,T2>& dst, const std::vector<int> selection_vector) {
		
		assert(mp.iCols() == (int)selection_vector.size());
		
		// Indicator vector contains one for all dimensions to keep and zeros for all ro remove
		size_t new_dim = 0;
		std::vector<int> positions;
		for(int i = 0; i < mp.iCols(); i++) {
			if(selection_vector[i] != 0) {
				positions.push_back(i);
				new_dim++;
			}
		}
		
		cv::Mat_<T1> input(mp.rows(),new_dim);
		
	    for(int i = 0; i < mp.rows(); i++) {
		    for(size_t j = 0; j < positions.size(); j++) {
		    	input(i,j) = mp.i(i,positions[j]);
		    }
	    }
		
	    dst = ocv::MatPair<T1,T2>(input,mp.o());
		
    }

    template<class T1, class T2>
    int mpalg<T1,T2>::selectRows(const ocv::MatPair<T1,T2> mp, ocv::MatPair<T1,T2>& dst, const std::vector<int> selection_vector) {
		
		assert(mp.rows() == (int)selection_vector.size());
		
		// Count rows to be retained
		int counter = std::accumulate(selection_vector.begin(),selection_vector.end(),static_cast<T1>(0));
		
		cv::Mat_<T1> input(counter, mp.iCols());
		cv::Mat_<T2> output(counter, mp.oCols());

		size_t next_idx = 0;
	    for(int i = 0; i < mp.rows(); i++) {
		    if(selection_vector[i] == 1) {
		    	std::copy(mp.iBegin(i),mp.iEnd(i),input.begin()+next_idx*input.cols);
				std::copy(mp.oBegin(i),mp.oEnd(i),output.begin()+next_idx*output.cols);
				next_idx++;
		    }
	    }
		
		dst = ocv::MatPair<T1,T2>(input,output);
	    return counter;
		
    }


    template<class T1, class T2>
    void mpalg<T1,T2>::appendCols(ocv::MatPair<T1,T2>& mp, const cv::Mat_<T1>& to_append) {
		
    	assert(mp.rows() == to_append.rows);
		
		cv::Mat_<T1> input(mp.rows(),mp.iCols()+to_append.cols);
		
    	for(int i = 0; i < mp.rows(); i++) {
    		std::copy(mp.iBegin(i),mp.iEnd(i),input.begin()+i*input.cols);
    		std::copy(to_append.begin()+i*to_append.cols,to_append.begin()+(i+1)*to_append.cols,input.begin()+i*input.cols+mp.iCols());
    	}
		
		mp = ocv::MatPair<T1,T2>(input,mp.o());
		
	}

	template<class T1, class T2>
	void mpalg<T1,T2>::appendRows(ocv::MatPair<T1,T2>& mp, const ocv::MatPair<T1,T2>& to_append) {
		
		assert(mp.iCols() == to_append.iCols());
		
		// Prepare temporary cv::Mat for input and output
		cv::Mat_<T1> input(mp.rows()+to_append.rows(),mp.iCols());
		cv::Mat_<T1> output(mp.rows()+to_append.rows(),mp.oCols());
		
		// Copy the input data
		cv::Mat_<T1> tmp = input(cv::Rect(0,0,mp.iCols(),mp.rows()));
		mp.i().copyTo(tmp);
		tmp = input(cv::Rect(0,mp.rows(),to_append.iCols(),to_append.rows()));
		to_append.i().copyTo(tmp);
		
		// Copy the output data
		tmp = output(cv::Rect(0,0,mp.oCols(),mp.rows()));
		mp.o().copyTo(tmp);
		tmp = output(cv::Rect(0,mp.rows(),to_append.oCols(),to_append.rows()));
		to_append.o().copyTo(tmp);
		
    	mp = ocv::MatPair<T1,T2>(input,output);
		
	}

	template<class T1, class T2>
	void mpalg<T1,T2>::splitMatPair(const ocv::MatPair<T1,int>& mp, std::vector<ocv::MatPair<T1,int>>& dst, int o_col_idx) {
		
		dst.clear();
		
		std::map<int,int> indicator;
		std::map<int,size_t> class_sizes;
		
		// Count occurences to make the following access faster
		for(int i = 0; i < mp.rows(); i++) {
			class_sizes[mp.o(i,o_col_idx)]++;
		}
		
		cv::Mat_<T1> input;
		cv::Mat_<T2> output;
		size_t idx;
		
		for(auto cs : class_sizes) {
		
			input = cv::Mat_<T1>(cs.second,mp.iCols());
			output = cv::Mat_<int>(cs.second,mp.oCols());
			idx = 0;
			
			for(int i = 0; i < mp.rows(); i++) {
				if(mp.o(i,o_col_idx) == cs.first) {
					std::copy(mp.iBegin(i),mp.iEnd(i),input.begin()+idx*input.cols);
					std::copy(mp.oBegin(i),mp.oEnd(i),output.begin()+idx*output.cols);
					idx++;
				}
			}
			
			dst.push_back(ocv::MatPair<T1,int>(input,output));
		
		}
		
	}
	
	template<class T1, class T2>
	void mpalg<T1,T2>::splitMatPairToMat(const ocv::MatPair<T1,int>& mp, std::vector<cv::Mat_<T1>>& dst, int o_col_idx) {
		
		dst.clear();
		
		std::map<int,int> indicator;
		std::map<int,size_t> class_sizes;
		
		// Count occurences to make the following access faster
		for(int i = 0; i < mp.rows(); i++) {
			class_sizes[mp.o(i,o_col_idx)]++;
		}
		
		cv::Mat_<T1> input;
		size_t idx;
		
		for(auto cs : class_sizes) {
		
			input = cv::Mat_<T1>(cs.second,mp.iCols());
			idx = 0;
			
			for(int i = 0; i < mp.rows(); i++) {
				if(mp.o(i,o_col_idx) == cs.first) {
					std::copy(mp.iBegin(i),mp.iEnd(i),input.begin()+idx*input.cols);
					idx++;
				}
			}
			
			dst.push_back(input);
		
		}
		
	}
	
	template<class T1, class T2>
	void mpalg<T1,T2>::randomSubset(const ocv::MatPair<T1,T2>& mp, ocv::MatPair<T1,T2>& dst, const size_t items) {
		
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> uni(0,items);

		cv::Mat_<T1> input(items,mp.iCols());
		cv::Mat_<T2> output(items,mp.oCols());
		size_t rnd, idx = 0;
		
		for(size_t i = 0; i < items; i++) {
			rnd = uni(rng);
			std::copy(mp.iBegin(rnd),mp.iEnd(rnd),input.begin()+idx*input.cols);
			std::copy(mp.oBegin(rnd),mp.oEnd(rnd),output.begin()+idx*output.cols);
		}

		dst = ocv::MatPair<T1,T2>(input,output);
		
	}

	
	template class mpalg<double,double>;
	template class mpalg<double,float>;
	template class mpalg<double,int>;
//	template class mpalg<double,size_t>;
	
	template class mpalg<float,double>;
	template class mpalg<float,float>;
	template class mpalg<float,int>;
//	template class mpalg<float,size_t>;
	
	template class mpalg<int,double>;
	template class mpalg<int,float>;
	template class mpalg<int,int>;
//	template class mpalg<int,size_t>;
	
//	template class mpalg<size_t,double>;
//	template class mpalg<size_t,float>;
//	template class mpalg<size_t,int>;
//	template class mpalg<size_t,size_t>;
	
}

