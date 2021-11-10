#pragma once

#include "opencv2/core.hpp"

#include "oceancv/ml/aggregation.h"
#include "oceancv/ml/vec_algorithms.h"

namespace ocv {

	/**
	 * @brief: Essentially a struct for a 2D coordinate (x,y) that is
	 * stored as the parent class (cv::Point) x,y coordinate
	 * combined with a weigth value that can store any kind of float
	 * info connected to this point.
	 * @author: Timm Schoening - tschoening [at] geomar [dot] de
	 * @date: 2020-07-01
	 */
	class LaserPoint : public cv::Point {
	public:
		LaserPoint(int x, int y, float weight) : _weight(weight) {
			this->x = x;
			this->y = y;
		}
		float _weight;
	};


class laserpoints {

public:

	static ocv::Aggregation<float> getPixelsPerMeterEqualDistances(const std::vector<cv::Point>& laser_points, float laser_distance_meters) {

		assert(laser_points.size() > 0);

		float px_dist,mean;
		std::vector<float> values = {};
		for(int i = 0; i < laser_points.size(); i++) {
			for(int j = i+1; j < laser_points.size(); j++) {
				px_dist = std::sqrt(std::pow(laser_points[i].x-laser_points[j].x,2)+std::pow(laser_points[i].y-laser_points[j].y,2));
				if(px_dist > 0)
					values.push_back(1.f * laser_distance_meters / px_dist);
			}
		}
		cv::Mat_<float> tmp(values);

		mean = ocv::valg<float>::mean(tmp);
		return ocv::Aggregation<float>(mean,ocv::valg<float>::deviation(tmp,mean),values.size(),ocv::AggregationType::MEAN,ocv::DeviationType::STDDEV);

	}

	/**
	 * The Sonne OFOS has three laser pointers for the still camera. One is to the left, two are to the right. The distance between the
	 * two right ones are different to the distance between the left and the other two.
	 */
	static ocv::Aggregation<float> getPixelsPerMeterSonneOFOS(const std::vector<cv::Point>& laser_points, float laser_distance_meters_lr = 0.45, float laser_distance_meters_rr = 0.4) {

		assert(laser_points.size() == 2 || laser_points.size() == 3);

		// Find leftmost lp
		int min_x = 100000;
		cv::Point left_lp;
		for(auto lp : laser_points) {
			if(lp.x < min_x) {
				min_x = lp.x;
				left_lp = lp;
			}
		}

		float tmp_d, mean;
		if(laser_points.size() == 2) {
			// One lp was not found, check whether it was the left one
			tmp_d = cv::norm(laser_points[0]-laser_points[1]);
			if(std::abs(laser_points[0].x-laser_points[1].x) < 100) {
				// We have two right lps
				return ocv::Aggregation<float>(laser_distance_meters_rr/tmp_d,0.f,1);
			} else {
				// We have a left and a right
				return ocv::Aggregation<float>(laser_distance_meters_lr/tmp_d,0.f,1);
			}
		} else {
			std::vector<float> values = {};
			for(int i = 0; i < 3; i++) {
				tmp_d = cv::norm(laser_points[i]-laser_points[(i+1)%3]);
				if(tmp_d == 0)
					continue;
				if(laser_points[i] == left_lp || laser_points[(i+1)%3] == left_lp)
					values.push_back(1.f * laser_distance_meters_lr / tmp_d);
				else
					values.push_back(1.f * laser_distance_meters_rr / tmp_d);
			}
			cv::Mat_<float> tmp(values);

			mean = ocv::valg<float>::mean(tmp);
			if(values.size() == 1) {
				return(ocv::Aggregation<float>(mean,0.f,1,ocv::AggregationType::MEAN,ocv::DeviationType::STDDEV));
			} else {
				return ocv::Aggregation<float>(mean,ocv::valg<float>::deviation(tmp,mean),values.size(),ocv::AggregationType::MEAN,ocv::DeviationType::STDDEV);
			}
		}

	}

	static ocv::Aggregation<float> getPixelsPerMeterTwoFixedOneMovingLP(const std::vector<cv::Point>& laser_points, float laser_distance_meters) {
		// TODO Find outer LPs
		// TODO Return scale factor based on outer two values
		return ocv::Aggregation<float>(0,0,-1);
	}

	static float getImageAreaInSquareMeters(const cv::Size& size, float px_per_sqm) {
		return size.width * px_per_sqm * size.height * px_per_sqm;
	}

};

}
