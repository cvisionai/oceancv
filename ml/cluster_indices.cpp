#include "oceancv/ml/cluster_indices.h"

namespace ocv {

	template<class T1, class T2>
	T1 ci<T1,T2>::normalizedScalarDist(const cv::Mat_<T1>& vec_1, const cv::Mat_<T1>& vec_2) {
		
		ocv::ScalarMetric<T1> met;
		
		/*T1 length = ocv::valg<T1>::euclideanLength(vec_1);
		cv::Mat_<T1> tmp_v1(1,v1_end-v1_begin);
		std::transform(v1_begin,v1_end,tmp_v1.begin(),[&length](auto a) -> T1 {return a / length; });
		
		length = ocv::valg<T1>::euclideanLength(v2_begin,v2_begin + (v1_end - v1_begin));
		cv::Mat_<T1> tmp_v2(1,v1_end-v1_begin);
		std::transform(v2_begin,v2_begin+(v1_end-v1_begin),tmp_v2.begin(),[&length](auto a) -> T1 {return a / length; });*/
		
		return met.distance(vec_1 / ocv::valg<T1>::euclideanLength(vec_1), vec_2 / ocv::valg<T1>::euclideanLength(vec_2));
	}
	
	template<class T1, class T2>
	void ci<T1,T2>::computeAllMean(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means, cv::Mat_<T1>& all_mean) {
		assert(class_mats.size() > 0 && class_mats.size() == class_means.size());
		
		all_mean = cv::Mat_<T1>(1,class_mats[0].cols,static_cast<T1>(0));
		size_t total_items = 0;
		for(size_t i = 0; i < class_mats.size(); i++) {
			all_mean += class_means[i] * (T1) class_mats[i].rows;
			total_items += class_mats[i].rows;
		}
		all_mean /= total_items;
	}

	template <class T1, class T2>
	cv::Mat_<T1> ci<T1,T2>::betweenClassScatter(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means) {
	
		// Compute all_mean
		cv::Mat_<T1> all_mean;
		computeAllMean(class_mats,class_means,all_mean);
		
	    // Set between class scatter Mat
	    bool first = true;
	    size_t dim = 0;
	    cv::Mat_<T1> between_scatter;
	    for(size_t i = 0; i < class_means.size(); i++) {
	    	if(first) {
	    		dim = class_mats[i].cols;
	    		between_scatter = cv::Mat_<T1>(dim, dim, static_cast<T1>(0));
		    	first = false;
	    	}
	        for(size_t j = 0; j < dim; j++) {
	            for(size_t k = 0; k < dim; k++) {
	                between_scatter(j,k) += (class_means[i](j) - all_mean(j)) * class_means[i](k) - all_mean(k);
	            }
	        }
	    }
		
	    return between_scatter;
	
	}
	
	template <class T1, class T2>
	cv::Mat_<T1> ci<T1,T2>::withinClassScatter(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means) {
	
		bool first = true;
	    size_t dim = 0;
	    cv::Mat_<T1> within_scatter;
		
	    for(size_t i = 0; i < class_mats.size(); i++) {
			
	    	if(first) {
	    		dim = class_mats[i].cols;
	    		within_scatter = cv::Mat_<T1>(dim, dim, static_cast<T1>(0));
		    	first = false;
	    	}
			
	        for(int j = 0; j < class_mats[i].rows; j++) {
				
                for(size_t k = 0; k < dim; k++) {
                    for(size_t l = 0; l < dim; l++) {
                        within_scatter(k,l) += (class_mats[i](j,k) - class_means[i](k))*(class_mats[i](j,l) - class_means[i](l));
                    }
                }

	        }
	    }
	    return within_scatter;
		
	}

	template<class T1, class T2>
	T1 ci<T1,T2>::chalinskiHarabasz(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means) {
	
		assert(class_mats.size() == class_means.size());
		
		// Compute all_mean
		cv::Mat_<T1> all_mean;
		computeAllMean(class_mats,class_means,all_mean);
		
		int N = 0,n,k = class_mats.size();
		T1 a = 0.0, b = 0.0;
		for(size_t j = 0; j < class_mats.size(); j++) {
			n = class_mats[j].rows;
			N += n;
			a += n * pow(normalizedScalarDist(class_means[j],all_mean),2);
			for(int i = 0; i < n; i++) {
				b += pow(normalizedScalarDist(class_mats[j].row(i),class_means[j]),2);
			}
		}
		
		return a / b * (N - k) / (k - 1);
		
	}

	template<class T1, class T2>
	T1 ci<T1,T2>::indexI(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means) {
		
		// Compute all_mean
		cv::Mat_<T1> all_mean;
		computeAllMean(class_mats,class_means,all_mean);
		
		size_t n,kms = class_mats.size();
		T1 a = 0.0, b = 0.0, maxd = 0.0;
		for(size_t j = 0; j < class_mats.size(); j++) {
			n = class_mats[j].rows;
			for(size_t i = 0; i < n; i++) {
				a += normalizedScalarDist(class_mats[j].row(i),all_mean);
				b += normalizedScalarDist(class_mats[j].row(i),class_means[j]);
			}
			for(size_t k = 0; k < class_mats.size(); k++) {
				maxd = std::max(maxd,normalizedScalarDist(class_means[j],class_means[k]));
			}
		}
		return 1.0 / kms * a / b * maxd;
	}
	
	template<class T1, class T2>
	T1 ci<T1,T2>::daviesBouldin(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means) {
		
		int kms = class_mats.size();
		T1 a,maxsum,frac,maxfrac;
		
		// Compute intracluster distances
		std::vector<T1> intradist(class_mats.size());
		
		for(size_t j = 0; j < class_mats.size(); j++) {
			a = 0.0;
			for(int i = 0; i < class_mats[j].rows; i++) {
				a += normalizedScalarDist(class_mats[j].row(i),class_means[j]);
			}
			intradist[j] = a / class_mats[j].rows;
		}
		
		// Find maxima
		maxsum = 0.0;
		for(size_t j = 0; j < class_mats.size(); j++) {
			maxfrac = 0.0;
			for(size_t k = 0; k < class_mats.size(); k++) {
				if(j != k) {
					frac = (intradist[j] + intradist[k]) / normalizedScalarDist(class_means[j],class_means[k]);
					maxfrac = std::max(maxfrac,frac);
				}
			}
			maxsum += maxfrac;
		}
		
		return 1.0 / kms * maxsum;
	
	}

	template<class T1, class T2>
	T1 ci<T1,T2>::intraClassVariance(const std::vector<cv::Mat_<T1>>& class_mats, const std::vector<cv::Mat_<T1>>& class_means) {
		T1 allsum = 0.0;
		for(size_t j = 0; j < class_mats.size(); j++) {
			for(int i = 0; i < class_mats[j].rows; i++) {
				allsum += 1.0 / class_mats[j].rows * normalizedScalarDist(class_mats[j].row(i),class_means[j]);
			}
		}
		return allsum;
	}

	template<class T1, class T2>
	T1 ci<T1,T2>::interClassVariance(const std::vector<cv::Mat_<T1>>& class_means) {
		
		T1 allsum = 0.0;
		for(auto tmp_mean_1 : class_means) {
			for(auto tmp_mean_2 : class_means) {
				allsum += normalizedScalarDist(tmp_mean_1,tmp_mean_2);
			}
		}
		return allsum;
	
	}
	
	
	template<class T1, class T2>
	T1 ci<T1,T2>::compute(const std::vector<cv::Mat_<T1>>& mv, ocv::CI_TYPES ci_type) {
		
		// Compute mean for each MatPair
		std::vector<cv::Mat_<T1>> class_means;
		std::cout << "pre means" << std::endl;
		ocv::malg<T1>::means(mv,class_means);
		std::cout << "have means" << std::endl;
		
	    switch(ci_type) {
			case ocv::CI_TYPES::CALINSKI_HARABASZ:
				return chalinskiHarabasz(mv,class_means);
			break;
			case ocv::CI_TYPES::INDEX_I:
				return daviesBouldin(mv,class_means);
			break;
			case ocv::CI_TYPES::DAVIES_BOULDIN:
				return indexI(mv,class_means);
			break;
			case ocv::CI_TYPES::INTER_CLASS_VARIANCE:
				return interClassVariance(class_means);
			break;
			case ocv::CI_TYPES::INTRA_CLASS_VARIANCE:
				return intraClassVariance(mv,class_means);
			break;
			default:
				return 0;
			break;
		}
		
	}
	
	template<class T1, class T2>
	T1 ci<T1,T2>::compute(const ocv::MatPair<T1,int>& mp, ocv::CI_TYPES ci_type, size_t label_index) {
	
		// Split to single-class Mats
		std::vector<cv::Mat_<T1>> single_mats;
		std::cout << "pre split" << std::endl;
		ocv::mpalg<T1,T2>::splitMatPairToMat(mp, single_mats, label_index);
		std::cout << "psot split" << std::endl;
		return compute(single_mats,ci_type);
		
	}
	
	template<class T1, class T2>
	cv::Mat_<T1> ci<T1,T2>::compute(const std::vector<cv::Mat_<T1>>& mv, ocv::SC_TYPES sc_type) {
		
		// Compute mean for each MatPair
		std::vector<cv::Mat_<T1>> class_means;
		ocv::malg<T1>::means(mv,class_means);
		
	    switch(sc_type) {
			case ocv::SC_TYPES::BETWEEN_CLASS_SCATTER:
				return betweenClassScatter(mv,class_means);
			break;
			case ocv::SC_TYPES::WITHIN_CLASS_SCATTER:
				return betweenClassScatter(mv,class_means);
			break;
			default:
				return cv::Mat_<T1>(mv.size(),mv.size(),static_cast<T1>(0));
			break;
		}
		
	}
	
	template<class T1, class T2>
	cv::Mat_<T1> ci<T1,T2>::compute(const ocv::MatPair<T1,int>& mv, ocv::SC_TYPES sc_type, size_t label_index) {
	
		// Split to single-class MatPairs
		std::vector<cv::Mat_<T1>> single_mats;
		ocv::mpalg<T1,T2>::splitMatPairToMat(mv, single_mats, label_index);
		
		return compute(single_mats,sc_type);
		
	}
	
	template class ci <int,int>;
//	template class ci <int,size_t>;
	
//	template class ci <size_t,int>;
//	template class ci <size_t,size_t>;
	
	template class ci <float,int>;
//	template class ci <float,size_t>;
	
	template class ci <double,int>;
//	template class ci <double,size_t>;
	
}

