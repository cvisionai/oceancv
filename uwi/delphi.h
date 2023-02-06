#pragma once

#include <numeric>

#include "opencv2/opencv.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafilters.hpp"

#include "oceancv/util/file_system.h"
#include "oceancv/util/progress_bar.h"

#include "oceancv/ml/metric.h"
#include "oceancv/ml/mat_algorithms.h"
#include "oceancv/ml/aggregation.h"
#include "oceancv/img/pixel_blob.h"
#include "oceancv/uwi/laser.h"

namespace ocv {

/**
 * @brief: This class implements the algorithm behind the paper:
 * "DELPHI - fast and adaptive computational laser point detection and
 * visual footprint quantification for arbitrary underwater image collections"
 * DOI: https://doi.org/10.3389/fmars.2015.00020
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2020-07-01
 */
class DeLPHI {

public:

	/**
	 * @brief: Constructs the DeLPHI object from a set of given image paths and the corresponding annotated
	 * laser point coordinates. You can set the values for theta_1 (margin around annotated lps to
 	 * look for random non-lp colors) and theta_2 (size of lps) as well. Theta_3 represents how pure the color
	 * clusters need to be (in 0..1). Theta_4 encodes how far a curated color vector
	 * needs to be away from another to be incorporated into the set.
	 * @author: Timm Schoening - tschoening [at] geomar [dot] de
	 * @date: 2020-07-01
	 */
	DeLPHI(const std::vector<std::string>& image_paths, const std::vector<std::vector<cv::Point>>& lp_coordinates, int theta_1 = 25, int theta_2 = 3, float theta_3 = 0.75, int theta_4 = 5) {

		assert(image_paths.size() == lp_coordinates.size());

		setTheta1(theta_1);
		setTheta2(theta_2);
		setTheta3(theta_3);
		setTheta4(theta_4);

		pickLaserPointColorsFromImages(image_paths, lp_coordinates);

		std::cout << "[DeLPHI] Training laser point detection algorithms with " << _laser_point_colors.size() << " colour samples" << std::endl;
		if(_laser_point_colors.size() == 0 || !trainDeLPHI())
			std::cerr << "[DeLPHI] Training failed" << std::endl;

	}

	/**
	 * @brief: Constructs the DeLPHI object from a pre-trained object that was stored to disk.
	 * It expects two files (..._delphi_mask.png and ..._delphi_params.json) at the given
	 * path_to_delphi.
	 * @author: Timm Schoening - tschoening [at] geomar [dot] de
	 * @date: 2020-07-14
	 */
	DeLPHI(std::string path_to_delphi_model) {

		ocv::forceFileExists(path_to_delphi_model + "_delphi_mask.png");
		ocv::forceFileExists(path_to_delphi_model + "_delphi_params.json");

		// Read mask
		cv::Mat tmp;
		tmp = cv::imread(path_to_delphi_model + "_delphi_mask.png",cv::IMREAD_GRAYSCALE);
		tmp.convertTo(_mask,CV_32FC1);

		// Open json file
		cv::FileStorage file(path_to_delphi_model + "_delphi_params.json", cv::FileStorage::READ);

		// Read parameters
		file["theta_1"] >> _theta_1;
		file["theta_2"] >> _theta_2;
		file["theta_3"] >> _theta_3;
		file["theta_4"] >> _theta_4;
		file["gamma"] >> _dist_gamma;

		// Read curated colors
		_curated_colors = {};
		file["curated_colors"] >> tmp;
		for(int i = 0; i < tmp.rows; i++)
			_curated_colors.push_back(tmp.at<cv::Vec3b>(i,0));

		// Read laser point coordinates
		file["lp_coordinates"] >> tmp;
		_laser_point_coordinates = {};
		std::vector<cv::Point> tmp_v;
		int prev_c = -1;
		for(int i = 0; i < tmp.rows; i++) {
			if(prev_c != tmp.at<ushort>(i,0)) {
				if(prev_c >= 0) {
					_laser_point_coordinates.push_back(tmp_v);
				}
				tmp_v = {};
			}
			tmp_v.push_back(cv::Point(tmp.at<ushort>(i,1),tmp.at<ushort>(i,2)));
			prev_c = tmp.at<ushort>(i,0);
		}

		file.release();
		_is_trained = true;

	}

	/**
	 * @brief: Writes a trained model to disk.
	 * @author: Timm Schoening - tschoening [at] geomar [dot] de
	 * @date: 2020-07-14
	 */
	bool writeModel(std::string path, std::string name) {

		if(!_is_trained)
			return false;
		ocv::forceFileExists(path);
		assert(name != "");

		// Store mask
		cv::Mat tmp;
		_mask.convertTo(tmp,CV_8UC1);
		cv::imwrite(path + name + "_delphi_mask.png",tmp);

		// Store parameters (theta_1,theta_2,gamma)
		cv::FileStorage out(path + name + "_delphi_params.json", cv::FileStorage::WRITE);
		out << "theta_1" << _theta_1;
		out << "theta_2" << _theta_2;
		out << "theta_3" << _theta_3;
		out << "theta_4" << _theta_4;
		out << "gamma" << _dist_gamma;

		// Store curated colors
		out << "curated_colors" << cv::Mat(_curated_colors);

		// Store lp_coordinates
		int count = 0, count1 = 0;
		for(int i = 0; i < _laser_point_coordinates.size(); i++)
			count += _laser_point_coordinates[i].size();
		tmp = cv::Mat(count,3,CV_16UC1);
		count = 0;
		for(int i = 0; i < _laser_point_coordinates.size(); i++) {
			for(int j = 0; j < _laser_point_coordinates[i].size(); j++) {
				tmp.at<ushort>(count1,0) = count;
				tmp.at<ushort>(count1,1) = _laser_point_coordinates[i][j].x;
				tmp.at<ushort>(count1,2) = _laser_point_coordinates[i][j].y;
				count1++;
			}
			count++;
		}
		out << "lp_coordinates" << tmp;

		out.release();

		std::cout << "[DeLPHI] Wrote model to " << path << name << "[_delphi_mask.png,_delphi_params.json]" << std::endl;
		return true;

	}

	/**
	 * @brief: Picks laser point colors from the provided image paths and annotations and
	 * stores them internally. You can also do that yourself and feed the object prior training
	 * using the functions setCoordinates(...) and setColors(...) and not use this function.
	 * After setting the colors and coordinates (either way) you have to run trainDeLPHI().
	 * @author: Timm Schoening - tschoening [at] geomar [dot] de
	 * @date: 2020-07-01
	 */
	bool pickLaserPointColorsFromImages(const std::vector<std::string>& image_paths, const std::vector<std::vector<cv::Point>>& lp_coordinates) {

		// Reset coordinates and color values
		_laser_point_coordinates = lp_coordinates;
		_laser_point_colors = {};
		_not_laser_point_colors = {};
		_is_trained = false;

		cv::Mat img;
		int added,rnd_x,rnd_y;
		double min_dist;

		ocv::progress_bar pb(image_paths);

		for(int i = 0; i < image_paths.size(); i++) {

			// Read image and make sure data has been acquired
			ocv::forceFileExists(image_paths[i]);
			img = cv::imread(image_paths[i],cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION);
			assert(img.total() > 0 && img.channels() == 3);

			if(i == 0) {
				_img_size = img.size();
				std::cout << "[DeLPHI] Reading colors from images (of size: " << img.size() << ")" << std::endl;
			} else
				assert(_img_size == img.size());

			pb.advance(i);

			// Add annotated laser points as "positive" training samples
			added = 0;
			for(auto p : _laser_point_coordinates[i]) {
				for(int tmp_x = std::max(0,p.x-_theta_2); tmp_x < std::min(p.x+_theta_2,img.cols); tmp_x++) {
					for(int tmp_y = std::max(0,p.y-_theta_2); tmp_y < std::min(p.y+_theta_2,img.rows); tmp_y++) {
						if(pow(tmp_x-p.x,2)+pow(tmp_y-p.y,2) < pow(_theta_2,2)) {
							_laser_point_colors.push_back(img.at<cv::Vec3b>(tmp_y,tmp_x));
							added++;
						}
					}
				}
			}

			// Add random points away from annotated points as "negative" training samples
			while(added > 0) {
				rnd_x = rand() % img.cols;
				rnd_y = rand() % img.rows;
				min_dist = pow(img.rows,2)+pow(img.cols,2);
				for(auto p : _laser_point_coordinates[i])
					min_dist = std::min(min_dist,pow(rnd_x-p.x,2)+pow(rnd_y-p.y,2));
				if(min_dist < pow(_theta_1,2)) {
					_not_laser_point_colors.push_back(img.at<cv::Vec3b>(rnd_y,rnd_x));
					added--;
				}
			}

		}

	}

	/**
	 * @brief: Executes the training of the (kmeans and knn) algorithms. Called by the contructor.
	 * Computes one DeLPHI iteration, conduct as many as you like (with increasing training set size)
	 * Uses the list of laser point coordinates and a list of laser point colors as an input.
	 * @author: Timm Schoening - tschoening [at] geomar [dot] de
	 * @date: 2020-07-01
	 */
	bool trainDeLPHI() {
		if(_laser_point_colors.size() == 0 || _laser_point_coordinates.size() == 0)
			return false;
		_is_trained = trainDeLPHIMask();
		_is_trained &= trainDeLPHIColors();
		return _is_trained;
	}


	// Below are getters and setters for parameters and internal states

	bool isTrained() const {
		return _is_trained;
	}
	void setCoordinates(const std::vector<std::vector<cv::Point>>& lp_coordinates) {
		_laser_point_coordinates = lp_coordinates;
		_is_trained = false;
	}
	void setColors(const std::vector<cv::Vec3b>& lp_colors, const std::vector<cv::Vec3b>& not_lp_colors) {
		_laser_point_colors = lp_colors;
		_not_laser_point_colors = not_lp_colors;
		_is_trained = false;
	}
	void setTheta1(int theta_1) {
		_theta_1 = theta_1;
		_is_trained = false;
	}
	void setTheta2(int theta_2) {
		_theta_2 = theta_2;
		_is_trained = false;
	}
	void setTheta3(float theta_3) {
		_theta_3 = theta_3;
		_is_trained = false;
	}
	void setTheta4(int theta_4) {
		_theta_4 = theta_4;
		_is_trained = false;
	}

	int getTheta1() const {
		return _theta_1;
	}
	int getTheta2() const {
		return _theta_2;
	}
	float getTheta3() const {
		return _theta_3;
	}
	int getTheta4() const {
		return _theta_4;
	}
	const std::vector<std::vector<cv::Point>>& getLaserPointCoordinates() const {
		return _laser_point_coordinates;
	}

	cv::Mat getMask() const {
		return _mask;
	}
	cv::Size getMaskSize() const {
		return _mask.size();
	}
	std::vector<cv::Vec3b> getCuratedColors() const {
		return _curated_colors;
	}
	float getGamma() const {
		return _dist_gamma;
	}

protected:

	/** A list of per-image coordinates of annotated lps */
	std::vector<std::vector<cv::Point>> _laser_point_coordinates;

	/** A list of colors picked within distance theta_2 around the annotated lp coordinates */
	std::vector<cv::Vec3b> _laser_point_colors;

	/** A list of colors randomly picked outside of distance theta_1 around the annotated lp coordinates */
	std::vector<cv::Vec3b> _not_laser_point_colors;

	/** Distance parameters of the DeLPHI algorithm */
	int _theta_1;
	int _theta_2;
	float _theta_3;
	int _theta_4;

	/** Number of clusters to use for kMeans color clustering */
	int _k = 7;		// TODO: Make a parameter or optimize with cluster indices???

	/** Internal state variable to document whether this algorithm is trained */
	bool _is_trained;

	/** The image size this DeLPHI algorithm was trained with (and can only work with!) */
	cv::Size _img_size;

	/** Mask within which annotated laser points occured */
	cv::Mat_<float> _mask;

	/** Curated color values where colors are removed that rather match the random negatives than the annotated positives */
	std::vector<cv::Vec3b> _curated_colors;

	/** Mean color distance of all curated color vectors to their selected best-match centroid */
	float _dist_gamma;

	/**
	 * @brief: Trains the mask image. Takes the provided lp coordinates and places a buffer of theta_1 around them to add
	 * some margin for annotation error.
	 * @author: Timm Schoening - tschoening [at] geomar [dot] de
	 * @date: 2020-05-20
	 */
	bool trainDeLPHIMask() {

		assert(_laser_point_coordinates.size() > 0);

		// Construct internal mask image
		_mask = cv::Mat(_img_size,CV_32FC1,cv::Scalar(0));

		// Fill positions of mask image at lp positions
		for(std::vector<cv::Point> vec_p : _laser_point_coordinates) {
			for(cv::Point p : vec_p) {
				cv::rectangle(_mask,cv::Point(std::max(0,p.x-_theta_1),std::max(0,p.y-_theta_1)),cv::Point(std::min(_img_size.width,p.x+_theta_1),std::min(_img_size.height,p.y+_theta_1)),cv::Scalar(255),cv::FILLED);
			}
		}

		return true;

	}

	/**
	 * @brief: Trains the lp color matching. Runs a kmeans to cluster color values,
	 * automatically selects the "purest" clusters that have more than min_purity percent of
	 * "positive" color values. Only stores color samples as those vectors that are less
	 * than min_color_distance apart from each other.
	 * @author: Timm Schoening - tschoening [at] geomar [dot] de
	 * @date: 2020-07-01
	 */
	bool trainDeLPHIColors() {

		if(_laser_point_colors.size() == 0)
			return false;

		// Combine all colors pre kmeans
		std::vector<cv::Vec3b> all_qs;
		all_qs.reserve(_laser_point_colors.size()+_not_laser_point_colors.size());
		all_qs.insert(all_qs.end(), _laser_point_colors.begin(), _laser_point_colors.end());
		all_qs.insert(all_qs.end(), _not_laser_point_colors.begin(), _not_laser_point_colors.end());

		cv::Mat points_int(all_qs), points;
		cv::Mat points_int_res = points_int.reshape(1);
		cv::Mat labels, centers;

		points_int_res.convertTo(points, CV_32F);

		std::cout << "[DeLPHI] Training kMeans with " << points.size() << " color vectors." << std::endl;
		// Run kMeans
		double compactness = cv::kmeans(points, _k, labels,
			cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 1000, 0.01),10,cv::KMEANS_PP_CENTERS,centers);

		// Find best-matching cluster for all color values (positives and negatives)
		std::vector<int> cluster_histogram_qs(_k,0);
		std::vector<int> cluster_histogram_not_qs(_k,0);

		ocv::EuclideanMetric<float> met;
		for(int i = 0; i < _laser_point_colors.size(); i++)
			cluster_histogram_qs[ocv::malg<float>::bestMatchIndex(centers, cv::Mat(_laser_point_colors[i]), met)]++;
		for(int i = 0; i < _not_laser_point_colors.size(); i++)
			cluster_histogram_not_qs[ocv::malg<float>::bestMatchIndex(centers, cv::Mat(_not_laser_point_colors[i]), met)]++;

		float tmp_purity,cluster_size_fraction;
		std::vector<int> pure_clusters = {};
		for(int tmp_k = 0; tmp_k < _k; tmp_k++) {

		 	// Omit small clusters
			cluster_size_fraction = 1.f * (cluster_histogram_qs[tmp_k] + cluster_histogram_not_qs[tmp_k]) / all_qs.size();
			if(cluster_size_fraction < 0.5 / _k)
				continue;

			// Check purity of remaining clusters
			tmp_purity = 1.f * cluster_histogram_qs[tmp_k] / (cluster_histogram_qs[tmp_k] + cluster_histogram_not_qs[tmp_k]);
			if(tmp_purity > _theta_3)
				pure_clusters.push_back(tmp_k);

		}

		// Collect all color vectors that belong to the purest cluster centroids
		_curated_colors = {};
		_dist_gamma = 0;
		int min_k;
		float tmp_dist;
		bool add;
		for(int i = 0; i < _laser_point_colors.size(); i++) {

			// Check whether this color vector belong to one of the pure clusters
			min_k = ocv::malg<float>::bestMatchIndex(centers, cv::Mat(_laser_point_colors[i]), met);
			if(std::find(pure_clusters.begin(),pure_clusters.end(),min_k) != pure_clusters.end()) {

				// Check whether a similar color vector already exists in the curated colors
				add = true;
				for(auto v : _curated_colors) {
					tmp_dist = met.distance(cv::Mat(_laser_point_colors[i]),cv::Mat(v));
					if(tmp_dist < _theta_4) {
						add = false;
						break;
					}
				}
				if(add) {
					_curated_colors.push_back(_laser_point_colors[i]);
					_dist_gamma += met.distance(centers.row(min_k),cv::Mat(_laser_point_colors[i]));
				}
			}
		}
		_dist_gamma /= _curated_colors.size();

		return true;

	}

};

namespace cuda {

	/**
	 * @brief: applies a trianed delphi algorithm to an image. You can (optinally) provide the opencv filter
	 * that is applied after color mathcing for speedup (or in case you think opening with a 3x3 kernel is not)
	 * the best option.
	 * @author: Timm Schoening - tschoening [at] geomar [dot] de
	 * @date: 2020-07-01
	 */
	std::vector<cv::Point> applyDeLPHI(const ocv::DeLPHI& delphi, const cv::Mat& img, cv::cuda::GpuMat& mask, cv::Ptr<cv::cuda::Filter> filt_open = cv::cuda::createMorphologyFilter(cv::MORPH_OPEN,CV_32FC1,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)))
	) {

		assert(delphi.getMaskSize() == img.size());

		if(!delphi.isTrained()) {
			std::cout << "[DeLPHI] DeLPHI has not been trained yet." << std::endl;
			return {};
		}

		std::vector<cv::Vec3b> curated_colors = delphi.getCuratedColors();

		// Change image data type to float and split to three channels
		cv::Mat f_img;
		img.convertTo(f_img,CV_32FC3);
		cv::Mat channels[3];
		cv::split(f_img,channels);

		cv::cuda::GpuMat c_r,c_g,c_b;
		c_r.upload(channels[2]);
		c_g.upload(channels[1]);
		c_b.upload(channels[0]);

		// Prepare GpuMats for computation
		cv::cuda::GpuMat c_tmp,c_min,c_sum;
		c_min = cv::cuda::GpuMat(img.size(),CV_32FC1,cv::Scalar(50000000));

		// Match image against all existing cureted lp colors
		for(auto v : curated_colors) {

			// Dist within blue channel
			cv::cuda::subtract(c_b,cv::Scalar(v[0]),c_tmp);
			cv::cuda::pow(c_tmp,2.d,c_sum);

			// Dist within green channel
			cv::cuda::subtract(c_g,cv::Scalar(v[1]),c_tmp);
			cv::cuda::pow(c_tmp,2.d,c_tmp);
			cv::cuda::add(c_tmp,c_sum,c_sum);

			// Dist within red channel
			cv::cuda::subtract(c_r,cv::Scalar(v[2]),c_tmp);
			cv::cuda::pow(c_tmp,2.d,c_tmp);
			cv::cuda::add(c_tmp,c_sum,c_sum);

			// Store Euclidean distance vs. previous minima of other curated colors already tested
			cv::cuda::sqrt(c_sum,c_tmp);
			cv::cuda::min(c_min,c_tmp,c_sum);
			c_sum.copyTo(c_min);

		}

		// Get distance image to compute candidate weights later on
		cv::Mat img_min,tmp1,tmp2;
		c_min.download(img_min);

		// Get color distance value gamma from training phase and apply it as a threshold (all lp candidate pixels will now be white)
		float gamma = delphi.getGamma();
		cv::cuda::threshold(c_min,c_tmp,gamma,255,cv::THRESH_BINARY_INV);

		// Apply opening operation to remove noise
		filt_open->apply(c_tmp,c_sum);

		// Apply mask image (multiply it with the thresholded candidate image)
		cv::cuda::multiply(mask,c_sum,c_sum);
		c_sum.download(tmp1);

		// Find contours
		tmp1.convertTo(tmp2,CV_8UC1);
		std::vector<std::vector<cv::Point>> lp_candidate_contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(tmp2,lp_candidate_contours,hierarchy,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_NONE);

		// Collect lp blob center of gravity and total weight (by summing of distance image pixels belonging to this blob)
		std::vector<ocv::LaserPoint> lp_candidates;
		std::vector<cv::Point> mass_center;
		size_t wx, wy, mm;
		float integral;
		for(int i = 0; i < lp_candidate_contours.size(); i++) {

			// Omit small candidates
			if(lp_candidate_contours[i].size() < 4)
				continue;

			mass_center = ocv::contourInfo(lp_candidate_contours[i],false);
			wx = 0;
			wy = 0;
			mm = 0;
			integral = 0;
			for(int y = mass_center[0].y; y <= mass_center[1].y; y++) {
				for(int x = mass_center[0].x; x <= mass_center[1].x; x++) {
					if(cv::pointPolygonTest(lp_candidate_contours[i],cv::Point(x,y),false) >= 0) {
						wx += x;
						wy += y;
						mm++;
						integral += 255 - img_min.at<float>(y,x);
					}
				}
			}

			// Omit small candidates
			if(mm > 5)
				lp_candidates.push_back(LaserPoint(wx/mm,wy/mm,integral/mm));

		}

		// Pick max_num_candidates "best" candidates (based on the highest "weight")
		int max_num_candidates = 6;
		std::sort(lp_candidates.begin(), lp_candidates.end(), [](const auto l1, const auto l2){return l1._weight > l2._weight;});
		std::vector<int> indices(std::min(int(lp_candidates.size()),6));
		std::iota(indices.begin(), indices.end(),0);

		// Compare contours with annotated lp pairs/triangles
		std::vector<std::vector<cv::Point>> lp_coordinates = delphi.getLaserPointCoordinates();
		float tmp_distance, min_distance = 100000000;
		std::vector<int> best_indices;
		do {
			for(int i = 0; i < lp_coordinates.size(); i++) {

				if(lp_coordinates[i].size() > indices.size())
					continue;

				tmp_distance = 0;

				for(int j = 0; j < lp_coordinates[i].size(); j++)
					tmp_distance += std::pow(lp_candidates[indices[j]].x-lp_coordinates[i][j].x,2)+std::pow(lp_candidates[indices[j]].y-lp_coordinates[i][j].y,2);
				tmp_distance /= lp_coordinates[i].size();
				if(tmp_distance < min_distance) {
// TODO Currently the algorithm favours smaller number of lps (in case the training set has some images of 2 and some of 3 lps)
					min_distance = tmp_distance;
					best_indices = std::vector<int>(indices.begin(),indices.begin()+lp_coordinates[i].size());
				}
			}
		} while(std::next_permutation(indices.begin(),indices.end()));

		// Collect 2D lp coordinates and return them
		std::vector<cv::Point> ret = {};
		for(int i = 0; i < best_indices.size(); i++)
			ret.push_back(cv::Point(lp_candidates[best_indices[i]].x,lp_candidates[best_indices[i]].y));
		return ret;

	}

}
}
