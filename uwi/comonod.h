#pragma once

#include <numeric>

#include "opencv2/opencv.hpp"

#include "oceancv/img/pixel_blob.h"
#include "oceancv/cudaimg/fspice_normalization.h"

namespace ocv {

/**
 * @brief: Computes a Gray Level Co-occurrence matrix
 * Expects a CV_8UC3 image but analyses only the first (usually blue) channel.
 * Compares a pixel to the two adjacent pixels with higher x and y value only.
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 */
std::vector<std::vector<uint>> computeGLCM(const cv::Mat& tmp) {

    assert(tmp.type() == CV_8UC3);

    // Initialize GLCM
    std::vector<std::vector<uint>> glcm(256,std::vector<uint>(256,0));

    // Fill GLCM and histogram
    for(int y = 0; y < tmp.rows-1; y++) {
        for(int x = 0; x < tmp.cols-1; x++) {
            glcm[tmp.at<uchar>(y,x,0)][tmp.at<uchar>(y,x+1,0)]++;
            glcm[tmp.at<uchar>(y,x+1,0)][tmp.at<uchar>(y,x,0)]++;
            glcm[tmp.at<uchar>(y,x,0)][tmp.at<uchar>(y+1,x,0)]++;
            glcm[tmp.at<uchar>(y+1,x,0)][tmp.at<uchar>(y,x,0)]++;
        }
    }

    return glcm;

}

/**
 * @brief: Adaptive threshold for the CoMoNoD algorithm
 * Computes an adaptive threshold from a given CV_8UC3 image in a way to create
 * compact objects in the result image. Therefore, first a cooccurrence matrix
 * of pixel intensities is computed. Did you note how elegantly i changed from three
 * channels to intesity? Correct, we actually use only the first channel.
 * After the cooccurrence is computed (1px distance Moore-neighbour GLCM if you like),
 * that threshold is picked for which the least transitions between black and white
 * pixels would result.
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 * **/
uint adaptiveThreshold(const cv::Mat& tmp, float adaptive_threshold) {

    // Compute Gray Level Co-occurrence matrix
    auto glcm = ocv::computeGLCM(tmp);

    // Compute initial compactness between the two classes
    uint cc[2][2];
    cc[0][0] = glcm[0][0];
    cc[0][1] = 0;
    cc[1][0] = 0;
    cc[1][1] = (tmp.rows-1) * (tmp.cols-1) * 4 - glcm[0][0];
    for(int t = 1; t < 256; t++) {
        cc[0][1] += glcm[0][t];
        cc[1][0] += glcm[t][0];
        cc[1][1] -= glcm[0][t];
        cc[1][1] -= glcm[t][0];
    }


    float gammas[256];
    float gam, gam_chg, max_gamma = 0;
    float prev_gam = 1.0 * (cc[0][0] + cc[1][1]) / (cc[0][0] + cc[0][1] + cc[1][0] + cc[1][1]);
    uint max_gamma_t = 0;

    // Increase threshold and compute the according compactness
    for(int t = 1; t < 256; t++) {

        // Move cooc's of current threshold from negative to positive class
        cc[0][0] += glcm[t][t];
        cc[1][1] -= glcm[t][t];

        // Move cooc's of current threshold and t1 (<t) from mixed to positive class
        for(int t1 = 0; t1 < t; t1++) {
            cc[0][0] += glcm[t][t1];
            cc[0][0] += glcm[t1][t];
            cc[0][1] -= glcm[t1][t];
            cc[1][0] -= glcm[t][t1];
        }

        // Move cooc's of current threshold and t1 (>t) from negative to mixed class
        for(int t1 = t + 1; t1 < 256; t1++) {
            cc[0][1] += glcm[t][t1];
            cc[1][0] += glcm[t1][t];
            cc[1][1] -= glcm[t][t1];
            cc[1][1] -= glcm[t1][t];
        }

        // Compactness gamma (small for noisy images, larger for separate clusters of similar class)
        gam = 1.0 * (cc[0][0] + cc[1][1]) / (cc[0][0] + cc[0][1] + cc[1][0] + cc[1][1]);

        // Find maximum slope of gamma ("first derivative")
        gam_chg = prev_gam - gam;
        if(gam_chg > max_gamma) {
            max_gamma = gam_chg;
            max_gamma_t = t;
        }
        gammas[t] = gam_chg;

        prev_gam = gam;

    }

    // Start from the compactness slope maximum and move to the left until the slope is smaller than the given adaptive_threshold
    for(int t = max_gamma_t; t > 0; t--) {
        if(gammas[t] < adaptive_threshold * gammas[max_gamma_t]) {
            return t;
        }
    }

    return 0;

}


/**
 * @brief: Contrast maximization of the CoMoNoD algorithm
 * Takes an OpenCV Mat and performs several image processing steps on the GPU.
 * Essential is a colour normalization step using the fSpice algorithm
 * (see see https://doi.org/10.1371/journal.pone.0038179). The function expects
 * an input CV_8UC3 image and the top left and bottom right coordinates of the region
 * of interest to analyse. theta_gamma defines the limit until which pixels are
 * assigned to the nodule class (0 < theta_gamma < 1). Try 0.1 for starters.
 * Scale_fac defines how much the images needs to be resized to fit the median area
 * in the data set. Sigma is the fSpice parameter (try 701 for starters).
 * Binary, blob_index and contours are return values for the next CoMoNoD phase.
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 */
uint contrastMaximization(std::string filename, const cv::Mat& input, cv::Point top_left, cv::Point bottom_right, float theta_gamma, float scale_fac, uint sigma, cv::Mat& binary, cv::Mat& blob_index, std::vector<std::vector<cv::Point>>& contours) {

    cv::Mat tmp_1;
    cv::cuda::GpuMat input_g, tmp_1_g, tmp_2_g;
    std::vector<cv::Vec4i> hierarchy;

    // Median
    cv::medianBlur(input,tmp_1,5);

    // Switch to Gpu
    input_g.upload(tmp_1);

    // Scale to uniform size (pixel-to-cm ratio)
    cv::cuda::resize(input_g, tmp_1_g, cv::Size(), scale_fac, scale_fac, cv::INTER_CUBIC);

    // Gaussian
    ocv::cuda::Gauss(tmp_1_g,tmp_2_g,3);

    // Grayscale
    cv::cuda::cvtColor(tmp_2_g,tmp_1_g,cv::COLOR_BGR2GRAY);
    cv::cuda::merge(std::vector<cv::cuda::GpuMat>({tmp_1_g,tmp_1_g,tmp_1_g}),tmp_2_g);

    // fspice
    ocv::cuda::fspice(tmp_2_g,tmp_1_g,sigma);

    // Resize back so that sx and ex make sense in the new scaled coordinate system!!
    cv::cuda::resize(tmp_1_g, tmp_2_g, input.size(), -1, -1, cv::INTER_CUBIC);

    // Pick center
    tmp_1_g = tmp_2_g(cv::Rect(top_left.x,top_left.y,bottom_right.x-top_left.x-1,bottom_right.y-top_left.y-1));
    tmp_1_g.download(tmp_1);

    // Determine the binarization threshold adaptively through a compactness criterion
    uint threshold_used = ocv::adaptiveThreshold(tmp_1,theta_gamma);

    // Scale to uniform size again (pixel-to-cm ratio)
    cv::cuda::resize(tmp_1_g, tmp_2_g, cv::Size(), scale_fac, scale_fac, cv::INTER_CUBIC);

    // Convert to gray again
    cv::cuda::cvtColor(tmp_2_g,tmp_1_g,cv::COLOR_BGR2GRAY);

    // Binarize
    cv::cuda::threshold(tmp_1_g, tmp_2_g, threshold_used, 255, cv::THRESH_BINARY_INV);

    // Return from Gpu
    tmp_2_g.download(tmp_1);

    // Plot contours in binary mat (these are the external contours where holes have been filled!)
    cv::findContours(tmp_1,contours,hierarchy,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_NONE);

    binary = cv::Mat(tmp_1.size(),CV_8UC1,cv::Scalar(0));
    blob_index = cv::Mat(tmp_1.size(),CV_32SC1,cv::Scalar(0));
    for(size_t n = 0; n < contours.size(); n++) {
        cv::drawContours(binary,contours,n,cv::Scalar(255),cv::FILLED);
        cv::drawContours(blob_index,contours,n,cv::Scalar(n),cv::FILLED);
    }

    return threshold_used;

}


/**
 * @brief: Cuts pixel blobs at contour bottlenecks
 * Determines peaks within the distance image computed from the given binary image.
 * Then splits up blobs along blob contour bottlenecks to separate peaks into
 * distinct blobs.
 * Expects the contour from the first CoMoNoD phase and returns an index image where
 * each pixel is encoded by an increasing number identifying the blob it belongs to
 * and a binary image encoding blob / non-blob assignment.
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 */
void cutBlobBottlenecks(std::string filename, cv::Mat& peak_img, const std::vector<std::vector<cv::Point>> contours, cv::Mat& blob_index, cv::Mat& binary, std::string debug_dir = "") {

    std::vector<std::vector<cv::Point>> peaks, new_contours, tmp_contours;
    std::vector<cv::Vec4i> hierarchy, defects;
    std::map<uint,std::vector<cv::Point>> tmp_blob_peaks, new_blob_peaks;
    std::vector<std::pair<cv::Point,cv::Point>> cuts;
    bool all_separated;
    std::vector<int> hull2,in_v1,in_v2;
    std::vector<cv::Point> virtual_contour_1,virtual_contour_2,min_v1,min_v2;
    int sep_tries, sep_limit = 100;

    int min_d1,min_d2;
    float min_dist, tmp_dist;
    cv::Point pp1,pp2;
    std::cout << peak_img.size() << " " << peak_img.total() << std::endl;
    if (debug_dir.empty() == false) {
        cv::imwrite(debug_dir + filename + "_peak_img_input_in_bottlenecks.png",peak_img);
    }
    // Get peak markers
    cv::findContours(peak_img,peaks,hierarchy,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_NONE);
    std::cout << "have contours" << std::endl;
    cv::Mat tmp;// = cv::Mat(peak_img.rows,peak_img.cols,CV_8UC1,cv::Scalar(0));
    binary.copyTo(tmp);

    // Find blob index for each peak
    std::map<uint,std::vector<cv::Point>> blob_peaks = {};
    std::cout << peaks.size() << std::endl;
    for(uint n = 0; n < peaks.size(); n++) {
        blob_peaks[blob_index.at<uint32_t>(peaks[n][0].y,peaks[n][0].x)].push_back(peaks[n][0]);
        cv::circle(tmp,peaks[n][0],5,127);
    }
    if (debug_dir.empty() == false) {
        cv::imwrite(debug_dir + filename + "_peak_img_bottlenecks.png",tmp);
    }

    // Find contour bottlenecks such that the peaks are separated
    for(uint n = 0; n < contours.size(); n++) {

        if(blob_peaks[n].size() < 2)
            continue;

        tmp_contours = {contours[n]};
        tmp_blob_peaks = {{0,blob_peaks[n]}};

        cuts = {};

        all_separated = false;
        sep_tries = 0;
        while(!all_separated) {

            if(sep_tries++ > sep_limit)
                break;

            all_separated = true;
            new_contours = {};

            for(uint m = 0; m < tmp_contours.size(); m++) {

                // Check whether this tmp blob is split already
                if(tmp_blob_peaks[m].size() < 2) {
                    new_contours.push_back(tmp_contours[m]);
                    continue;
                }

                // Get convex hull and the defects (contour points that are far away from the hull)
                cv::convexHull(tmp_contours[m],hull2);
                cv::convexityDefects(tmp_contours[m],hull2,defects);
                std::sort(defects.begin(),defects.end(),[](auto a,auto b) {
                    return a[2] < b[2];
                });

                // Find shortest line between two defects that would separate two peaks into separate blobs
                min_dist = 10000000;
                min_d1 = min_d2 = 0;

                for(uint d1 = 0; d1 < defects.size(); d1++) {

                    cv::line(tmp,tmp_contours[m][defects[d1][0]],tmp_contours[m][defects[d1][1]],84);
                    cv::line(tmp,tmp_contours[m][defects[d1][2]],tmp_contours[m][defects[d1][1]],84);
                    cv::line(tmp,tmp_contours[m][defects[d1][0]],tmp_contours[m][defects[d1][2]],84);
                    cv::circle(tmp,tmp_contours[m][defects[d1][2]],2,84);

                    for(uint d2 = d1 + 1; d2 < defects.size(); d2++) {

                        // Create two virtual blobs by slicing the current contour at the two defect locations
                        virtual_contour_1 = std::vector<cv::Point>(tmp_contours[m].begin(),tmp_contours[m].begin() + defects[d1][2]);
                        virtual_contour_2 = std::vector<cv::Point>(tmp_contours[m].begin() + defects[d1][2],tmp_contours[m].begin() + defects[d2][2]);
                        std::copy(tmp_contours[m].begin() + defects[d2][2],tmp_contours[m].end(),std::back_inserter(virtual_contour_1));

                        if(virtual_contour_1.size() == 0)
                            continue;

                        // Check which peaks lie in the first virtual blob
                        in_v1 = std::vector<int>(tmp_blob_peaks[m].size(),0);
                        for(uint p1 = 0; p1 < tmp_blob_peaks[m].size(); p1++) {
                            in_v1[p1] = (cv::pointPolygonTest(virtual_contour_1,tmp_blob_peaks[m][p1],false) > 0);
                        }

                        for(uint p1 = 0; p1 < tmp_blob_peaks[m].size(); p1++) {
                            for(uint p2 = p1+1; p2 < tmp_blob_peaks[m].size(); p2++) {

                                // Check whether two peaks really lie in separate blobs
                                if((in_v1[p1] && !in_v1[p2]) || (!in_v1[p1] && in_v1[p2])) {

                                    pp1 = tmp_contours[m][defects[d1][2]];
                                    pp2 = tmp_contours[m][defects[d2][2]];

                                    // Find shortest cut to split the blob
                                    tmp_dist = pow(pp1.x-pp2.x,2)+pow(pp1.y-pp2.y,2);
                                    if(tmp_dist < min_dist) {
                                        min_dist = tmp_dist;
                                        min_d1 = d1;
                                        min_d2 = d2;
                                        min_v1 = virtual_contour_1;
                                        min_v2 = virtual_contour_2;
                                    }

                                }

                            }
                        }

                    }
                }

                // Check if there is a valid cut
                if(min_d1 != min_d2) {

                    // Update contours
                    new_contours.push_back(min_v1);
                    new_contours.push_back(min_v2);

                    // Store cut
                    cuts.push_back(std::pair<cv::Point,cv::Point>(tmp_contours[m][defects[min_d1][2]],tmp_contours[m][defects[min_d2][2]]));

                }

            }

            tmp_contours = new_contours;

            // Update peak assignments
            new_blob_peaks = {};
            for(uint m = 0; m < tmp_blob_peaks.size(); m++) {
                for(uint m2 = 0; m2 < tmp_blob_peaks[m].size(); m2++) {
                    for(uint m1 = 0; m1 < tmp_contours.size(); m1++) {
                        if(tmp_contours[m1].size() == 0)
                            continue;
                        if(cv::pointPolygonTest(tmp_contours[m1],tmp_blob_peaks[m][m2],false) >= 0) {

                            new_blob_peaks[m1].push_back(tmp_blob_peaks[m][m2]);

                            // Check if we have to do the splitting again
                            if(new_blob_peaks[m1].size() > 1) {
                                all_separated = false;
                            }

                            break;

                        }
                    }
                }
            }
            tmp_blob_peaks = new_blob_peaks;

        }

        // Draw cut lines in image
        for(auto cut : cuts) {
            cv::line(binary,cut.first,cut.second,cv::Scalar(0),2);
        }

    }
    if (debug_dir.empty() == false) {
        cv::imwrite(debug_dir + filename + "_peak_img_bottlenecks_2.png",tmp);
    }

}

// Create a list of neighbour pixel offsets to create a circular neighbourhood that can be traversed linearly
std::vector<cv::Point> createSearchNeighborList(uint theta_r) {

    std::vector<cv::Point> search_neighbors = {};
    //uint max_neighbor_search_dist = 2 * theta_r;

    for(uint y = -theta_r; y <= theta_r; y++) {
        for(uint x = -theta_r; x <= theta_r; x++) {
            if(x == 0 && y == 0)
                continue;
            if(y*y + x*x > theta_r)
                continue;
            search_neighbors.push_back(cv::Point(x,y));
        }
    }

    return search_neighbors;

}

/**
 * @brief: Second phase of the CoMoNoD algorithm for nodule delineation
 * Takes a binary image from the first phase, cuts up connected pixel blobs,
 * fuses small blobs in close vicinity and returns a binary image with final
 * pixel classification in nodule-positive (255) and nodule-negative (0).
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 */
void noduleDelineation(std::string filename,cv::Mat& binary, cv::Mat& blob_index, const std::vector<std::vector<cv::Point>>& contours, uint theta_r, float scale_fac, const cv::Ptr<cv::cuda::Filter>& erode, const cv::Ptr<cv::cuda::Filter>& dilate, std::string debug_dir, std::vector<cv::Point> search_neighbors = {}) {

    cv::Mat tmp_1, tmp_2, distance_img, peak_img, blob_sizes;
    cv::cuda::GpuMat tmp_1_g, tmp_2_g, peaks_g;
    std::vector<std::vector<cv::Point>> hull(1);
    std::vector<cv::Vec4i> hierarchy;

    std::map<uint,std::vector<cv::Point>> fuse_sets = {};
    std::vector<cv::Point> contour_info;

    int min_peak_dist = 5 * theta_r;
    uint max_neighbor_fuse_size = 3.14159 * theta_r * theta_r;
    uint min_neighbor_fuse_size = 4 * max_neighbor_fuse_size;
    std::cout << "max_neighbor_fuse_size:" << max_neighbor_fuse_size << std::endl;
    std::cout << "min_neighbor_fuse_size:" << min_neighbor_fuse_size << std::endl;

    int tx,ty;
    uint max_neighbor_index, max_neighbor_size;

    if(search_neighbors.size() == 0)
        search_neighbors = createSearchNeighborList(theta_r*2);

    // Split connected nodules
    cv::distanceTransform(binary,tmp_1,cv::DIST_L2,3);

    // Get briefly back to GPU to find peaks in distance image (200x speedup)
    tmp_2_g.upload(tmp_1);
    cv::cuda::threshold(tmp_2_g,tmp_1_g,theta_r,255,cv::THRESH_TOZERO);
    tmp_1_g.download(distance_img);
    tmp_1_g.convertTo(tmp_2_g,CV_8UC1);

    dilate->apply(tmp_2_g,peaks_g);
    cv::cuda::compare(tmp_2_g,peaks_g,peaks_g,cv::CMP_GE);

    erode->apply(tmp_2_g,tmp_1_g);
    cv::cuda::compare(tmp_2_g,tmp_1_g,tmp_1_g,cv::CMP_GT);
    cv::cuda::bitwise_and(peaks_g,tmp_1_g,peaks_g);

    // Final return to CPU
    peaks_g.download(peak_img);


    cv::Mat tmp_13;
    binary.copyTo(tmp_13);

    std::vector<std::vector<cv::Point>> tmp_peaks;
    cv::findContours(peak_img,tmp_peaks,hierarchy,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_NONE);
    std::cout << tmp_peaks.size() << std::endl;
    std::vector<int> active_peaks(tmp_peaks.size(),1);

    float line_length,dx,dy;
    bool is_outside_blob;

    for(int i = 0; i < tmp_peaks.size(); i++) {

        tmp_13.at<uchar>(tmp_peaks[i][0].y,tmp_peaks[i][0].x) = 50;

        if(active_peaks[i] == 0)
            continue;

        // Find other peaks in the same blob
        for(int j = 0; j < tmp_peaks.size(); j++) {

            if(i == j)
                continue;
            if(blob_index.at<uint32_t>(tmp_peaks[i][0].y,tmp_peaks[i][0].x) != blob_index.at<uint32_t>(tmp_peaks[j][0].y,tmp_peaks[j][0].x))
                continue;
            if(active_peaks[j] == 0)
                continue;

            //cv::line(tmp_13,tmp_peaks[i][0],tmp_peaks[j][0],84);

            // Check on the line between the point whether there is a point outside the blob
            line_length = std::sqrt(std::pow(tmp_peaks[i][0].y-tmp_peaks[j][0].y,2) + std::pow(tmp_peaks[i][0].x-tmp_peaks[j][0].x,2));
            dy = (tmp_peaks[j][0].y - tmp_peaks[i][0].y)/line_length;
            dx = (tmp_peaks[j][0].x - tmp_peaks[i][0].x)/line_length;
            //std::cout << line_length << " " << dx << " " << dy << std::endl;
            //exit;

            is_outside_blob = false;
            for(int k = 0; k < line_length; k++) {
                if(binary.at<uchar>(tmp_peaks[i][0].y+k*dy,tmp_peaks[i][0].x+k*dx) < 127) {
                    //cv::circle(tmp_13,cv::Point(tmp_peaks[i][0].x+k*dx,tmp_peaks[i][0].y+k*dy),3,200);
                    is_outside_blob = true;
                    //cv::line(tmp_13,tmp_peaks[i][0],tmp_peaks[j][0],30);
                    break;
                }
            }
            if(!is_outside_blob) {
                if(distance_img.at<float>(tmp_peaks[i][0].y,tmp_peaks[i][0].x) > distance_img.at<float>(tmp_peaks[j][0].y,tmp_peaks[j][0].x))
                    active_peaks[j] = 0;
                else {
                    active_peaks[i] = 0;
                    break;
                }
            }
        }
    }
    std::cout << "here" << std::endl;
    peak_img = cv::Scalar(0);
    std::cout << "there" << std::endl;
    int num_active = 0;
    for(int i = 0; i < active_peaks.size(); i++) {
        if(active_peaks[i] == 1) {
            peak_img.at<uchar>(tmp_peaks[i][0].y,tmp_peaks[i][0].x) = 255;
            //} else {
            cv::circle(tmp_13,tmp_peaks[i][0],3,127,-1);
            num_active++;

        }
    }
    std::cout << num_active << " " << peak_img.size() << std::endl;





    // Remove all peaks that have a larger neighbour in close vicinity
    /*for(int y = 0; y < peak_img.rows; y++) {
    	for(int x = 0; x < peak_img.cols; x++) {

            // Peak exists at x,y
    		if(peak_img.at<uchar>(y,x) > 127) {
    cv::circle(tmp_13,cv::Point(x,y),5,127,-1);
    			// Check pixel neighborhood for higher peaks
    			for(int dy = std::max(0,y-min_peak_dist); dy <= std::min(peak_img.rows-1,y+min_peak_dist); dy++) {
    				for(int dx = std::max(0,x-min_peak_dist); dx <= std::min(peak_img.cols-1,x+min_peak_dist); dx++) {
    0
                        // Peak exists at dx,dy that is in the same blob (by blob_index) and has a higher peak than the one at x,y
    					if(blob_index.at<uint32_t>(y,x) == blob_index.at<uint32_t>(dy,dx) && distance_img.at<float>(dy,dx) >= distance_img.at<float>(y,x) && peak_img.at<uchar>(dy,dx) > 127 && (y != dy || x != dx)) {
    						peak_img.at<uchar>(y,x) = 0;
    cv::circle(tmp_13,cv::Point(x,y),5,0,-1);
                        }
    				}
    			}

    		}

    	}
    }*/

    std::cout << "done peak finding" << std::endl;
    if (debug_dir.empty() == false) {
        cv::imwrite(debug_dir + filename + "_peaks.png",tmp_13);
        cv::imwrite(debug_dir + filename + "_peak_img_input.png",peak_img);
    }
    // Cut connected blobs along bottlenecks0
    ocv::cutBlobBottlenecks(filename,peak_img,contours,blob_index,binary);

    if (debug_dir.empty() == false) {
        cv::imwrite(debug_dir + filename + "_bin_cut.png",binary);
    }
    std::cout << "done cutting bottlenecks" << std::endl;

    // Fuse small blobs with locally biggest
    std::vector<std::vector<cv::Point>> tmp_contours;
    cv::findContours(binary,tmp_contours,hierarchy,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_NONE);

    // Redo blob index & size images
    blob_sizes = cv::Mat(binary.size(),CV_32SC1,cv::Scalar(0));
    blob_index = cv::Mat(binary.size(),CV_32SC1,cv::Scalar(0));
    for(size_t n = 0; n < tmp_contours.size(); n++) {
        cv::drawContours(blob_sizes,tmp_contours,n,cv::Scalar(cv::contourArea(tmp_contours[n])),cv::FILLED);
        cv::drawContours(blob_index,tmp_contours,n,cv::Scalar(n),cv::FILLED);
    }

    for(size_t n = 0; n < tmp_contours.size(); n++) {

        // For small nodules check if there is a bigger one close by
        if(tmp_contours[n].size() < max_neighbor_fuse_size) {

            // Get position and min/max of blob
            contour_info = ocv::contourInfo(tmp_contours[n],true);

            max_neighbor_size = 0;
            max_neighbor_index = 0;

            // Search around this nodule's mass center whether another nodule occurs (up to 2 * min_nodule_pixel_radius distance)
            for(cv::Point tp : search_neighbors) {
                tx = contour_info[3].x+tp.x;
                ty = contour_info[3].y+tp.y;
                if(tx < 0 || ty < 0 || tx >= binary.cols || ty >= binary.rows)
                    continue;
                if(blob_sizes.at<uint32_t>(ty,tx) > max_neighbor_size) {
                    max_neighbor_size = blob_sizes.at<uint32_t>(ty,tx);
                    max_neighbor_index = blob_index.at<uint32_t>(ty,tx);
                }
            }

            // Eventually copy the contour pixels to the bigger neighbours contour
            if(max_neighbor_size > min_neighbor_fuse_size) {
                copy(tmp_contours[n].begin(),tmp_contours[n].end(),std::back_inserter(fuse_sets[max_neighbor_index]));
            } else {
                copy(tmp_contours[n].begin(),tmp_contours[n].end(),std::back_inserter(fuse_sets[n]));
            }

        } else {
            copy(tmp_contours[n].begin(),tmp_contours[n].end(),std::back_inserter(fuse_sets[n]));
        }

    }

    // Compute convex hulls of fused blobs and plot all
    binary = cv::Scalar(0);
    for(auto pp : fuse_sets) {

        cv::convexHull(cv::Mat(pp.second),hull[0]);

        // Paint it black (only the border, to prevent re-overlaps)
        cv::drawContours(binary,hull,0,cv::Scalar(0),4);

        // Paint it white
        cv::drawContours(binary,hull,0,cv::Scalar(255),cv::FILLED);

    }

    std::cout << "done contours" << std::endl;

    // Rescale image back to original size
    cv::resize(binary, tmp_1, cv::Size(), 1.0/scale_fac, 1.0/scale_fac, cv::INTER_CUBIC);
    cv::threshold(tmp_1,tmp_2,127,255,cv::THRESH_BINARY);

    // Final blob detection, filter by size and store results
    cv::erode(tmp_2,binary,cv::Mat());

}


class CoMoNoDule : public cv::RotatedRect {
public:
    float cm_area;
    float px_area;
    CoMoNoDule(const cv::RotatedRect& r) : RotatedRect(r) {
        this->cm_area = 0;
        this->px_area = 0;
    }
    CoMoNoDule() {}
    ~CoMoNoDule() { }
};

/**
 * @brief: filters nodule detections
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2010-01-28
 */
void getSingleNodulesAndFilter(const cv::Mat& binary, std::vector<ocv::CoMoNoDule>& nodules, uint theta_r, float cm_per_pix, float min_nodule_size = 0, float max_nodule_size = -1, float max_ellipse_distortion = -1, float max_blob_distortion = -1) {

    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point>> contours;
    ocv::CoMoNoDule ellipse;
    double nodule_area;//, ellipse_area, ellipse_size;

    cv::findContours(binary,contours,hierarchy,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_NONE);

    nodules = {};

    for(size_t n = 0; n < contours.size(); n++) {

        nodule_area = cv::contourArea(contours[n]);

        if(nodule_area < theta_r)
            continue;

        // Fit ellipse needs at least 5 points!
        if(contours[n].size() < 5)
            continue;

        //nodule_size = 1.0 * nodule_area * cm_per_pix;

        // Fit contour with an ellipse
        ellipse = cv::fitEllipse(cv::Mat(contours[n]));

        if(max_ellipse_distortion > 0 && (ellipse.size.width > max_ellipse_distortion*ellipse.size.height || max_ellipse_distortion*ellipse.size.width < ellipse.size.height))
            continue;

        ellipse.px_area = 0.25 * 3.14159 * ellipse.size.width * ellipse.size.height;
        ellipse.cm_area = 1.0 * cm_per_pix * ellipse.px_area;

        if(ellipse.cm_area < min_nodule_size || (max_nodule_size > 0 && ellipse.cm_area > max_nodule_size))
            continue;

        if(max_blob_distortion > 0 && (ellipse.px_area > max_blob_distortion*nodule_area || ellipse.px_area*max_blob_distortion < nodule_area))
            continue;

        nodules.push_back(ellipse);

    }

    // Sort nodules by size
    sort(nodules.begin(), nodules.end(), [](const ocv::CoMoNoDule& a, const ocv::CoMoNoDule& b) -> bool { return a.px_area < b.px_area; });

}

/**
 * @brief: Executes the CoMoNoD algorithm on one image
 * Main function for the "Compact Morphology Nodule Detection" (CoMoNoD) algorithm.
 * Uses no machine learning, no pattern recognition, no deep learning, only image
 * processing, fSpice (see https://doi.org/10.1371/journal.pone.0038179), and a
 * compactness criterion as in ES4C (see http://doi.org/10.1016/j.mio.2016.04.002).
 * @author: Timm Schoening
 * @date: 2017-04-20
 **/
bool runCoMoNoD(const cv::Mat& input, std::vector<ocv::CoMoNoDule>& nodules, float theta_gamma, uint theta_r, float median_area, float image_area, cv::Point top_left, cv::Point bottom_right, float min_nodule_size = 0, float max_nodule_size = -1, float max_ellipse_distortion = -1, float max_blob_distortion = -1, const std::vector<double> orig_target_values = {0.01,0.10,0.25,0.50,0.75,0.90,0.99}) {

    // Check input data
    if(input.cols < 1 || input.rows < 1)
        return false;
    if(image_area <= 0)
        return false;

    // The scaling factor to make all images in the set have the same px2cm ratio
    float scale_fac = sqrt(median_area / image_area);

    // If the scale_fac is too far off the original pixel size, this could lead to memeory issues otherwise
    if(scale_fac < 0.0 || scale_fac > 2.1)
        return false;
    if(input.cols * input.rows * scale_fac * scale_fac > 30000000)
        return false;

    // Get size of region to be processed
    bottom_right.x = ((bottom_right.x < 0) ? input.cols : std::min(input.cols,bottom_right.x));
    bottom_right.y = ((bottom_right.y < 0) ? input.rows : std::min(input.rows,bottom_right.y));

    // Compute real world scaling factor (cm_per_pix)
    float cm_per_pix = 10000.0 * image_area / (input.cols * input.rows);

    // First phase of CoMoNoD
    cv::Mat binary, blob_index;
    std::vector<std::vector<cv::Point>> contours;
    ocv::contrastMaximization("foo",input, top_left, bottom_right, theta_gamma, scale_fac, 701, binary, blob_index, contours);

    // Second phase of CoMoNoD
    cv::Ptr<cv::cuda::Filter> erode = cv::cuda::createMorphologyFilter(cv::MORPH_ERODE,CV_8UC1,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));
    cv::Ptr<cv::cuda::Filter> dilate = cv::cuda::createMorphologyFilter(cv::MORPH_DILATE,CV_8UC1,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));
    std::vector<cv::Point> search_neighbors = ocv::createSearchNeighborList(theta_r*2);
    ocv::noduleDelineation("foo",binary, blob_index, contours, theta_r, scale_fac, erode, dilate, "", search_neighbors);

    // Get single nodules
    ocv::getSingleNodulesAndFilter(binary, nodules, theta_r, cm_per_pix, min_nodule_size, max_nodule_size, max_ellipse_distortion, max_blob_distortion);

    return true;

}

float noduleCoverage(const std::vector<ocv::CoMoNoDule>& nodules, const cv::Size& image_size) {
    float sum = 0.0;
    for(auto nod : nodules)
        sum += nod.px_area;
    return round(100.0 * sum / (image_size.width * image_size.height));
}

std::vector<double> noduleSizeBins(const std::vector<ocv::CoMoNoDule>& nodules, const std::vector<double> target_fractions = {0,0.01,0.25,0.5,0.75,0.99}) {
    std::vector<double> ret(target_fractions.size()+1);
    for(uint i = 0; i < target_fractions.size(); i++)
        ret[i] = nodules[target_fractions[i]*nodules.size()].cm_area;
    ret[target_fractions.size()] = nodules[nodules.size()-1].cm_area;
    return ret;
}

std::vector<uint> noduleDirectionHistogram(const std::vector<ocv::CoMoNoDule>& nodules, uint bins) {
    float bin_width = 180 / bins;
    uint bin;
    std::vector<uint> hist(bins,0);
    for(auto nodule : nodules) {
        bin = nodule.angle / bin_width;
        hist[bin]++;
    }
    return hist;
}

std::vector<double> noduleCoverageBins(const std::vector<ocv::CoMoNoDule>& nodules, const cv::Size& image_size, const std::vector<double> target_fractions = {0.01,0.10,0.25,0.50,0.75,0.90,0.99}) {

    // Contains the number of nodule-positive pixels, not a percentage value!
    float coverage = ocv::noduleCoverage(nodules, image_size);

    std::vector<double> ret(target_fractions.size(),-1), target_values;
    double sum_cov, target_value;
    uint target_index;

    // Store nodule size and coverage bins
    if(nodules.size() > 0) {

        // Store nodule coverage bins; first find the bin thresholds (we have to pick those from the real data)
        target_values = {};
        for(double t : target_fractions)
            target_values.push_back(1.0 * t * coverage);
        sum_cov = 0.0;
        target_index = 0;
        target_value = target_values[target_index];
        for(auto nodule : nodules) {

            sum_cov += nodule.px_area;

            if(target_value <= sum_cov) {

                ret[target_index] = nodule.cm_area;

                target_index++;
                target_value = target_values[target_index];

                if(target_index == target_values.size())
                    break;

            }

        }

    }

    return ret;

}

/**
 * @brief: Computes particle size statistics
 * Remember the sorting and skewness from geological particle size analysis?
 * Me neither. But there is literature on that (e.g. Füchtbauer "Sedimente und
 * Sedimentgesteine" 1988, Trask "Origin and environment ..." 1932).
 * Takes the Phis from CoMoNoD and returns geological valuable metrics:
 * Average (Mean or Median), Sorting, Skewness, Kurtosis
 * Different target values are required:
 *  - TRASK: {0.25,0.50,0.75}
 *  - INMAN: {0.05,0.16,0.50,0.84,0.95}
 *  - FRIEDMAN-SANDERS: {0.05,0.16,0.50,0.84,0.95}
 * @author: Timm Schoening
 * @date: 2017-04-20
 */
enum analysis_type {TRASK, INMAN, FRIEDMAN_SANDERS};
std::vector<double> particleSizeAnalysis(const std::vector<double>& sizes, analysis_type method = TRASK) {
    switch(method) {
    case INMAN:
        return {0.5*(sizes[1]+sizes[3]),0.5*(sizes[3]-sizes[1]),1.0*(sizes[3]+sizes[1]-2*sizes[2])/(sizes[3]-sizes[1]),1.0*((sizes[4]-sizes[1])-(sizes[3]-sizes[2]))/(sizes[3]-sizes[1])};
        break;
    case FRIEDMAN_SANDERS:
        return {0.3333*(sizes[1]+sizes[2]+sizes[3]),0.5*(sizes[4]-sizes[0]),sizes[4]+sizes[0]-2*sizes[2],-1};
        break;
    case TRASK:
    default:
        return {sizes[1],0.5*(sizes[2]-sizes[0]),sizes[0] + sizes[2] - 2 * sizes[1],-1};
        break;
    }
}

}
