#include "oceancv/img/pixel_blob.h"

namespace ocv {

cv::Point contourMassCenter(const std::vector<cv::Point>& contour, int minx, int maxx, int miny, int maxy) {

	size_t wx = 0;
	size_t wy = 0;
	size_t mm = 0;
	for(int y = miny; y <= maxy; y++) {
		for(int x = minx; x <= maxx; x++) {
			if(cv::pointPolygonTest(contour,cv::Point(x,y),false) >= 0) {
				wx += x;
				wy += y;
				mm++;
			}
		}
	}
	return cv::Point(wx / mm,wy / mm);

}

double contourArea(std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Vec4i>& hierarchy, size_t i) {

	if(i > contours.size())
		return 0;

	int child, first_child;

	double area = cv::contourArea(contours[i]);

	// Has this contour children?
	if(hierarchy[i][2] != -1) {

		first_child = hierarchy[i][2];

		// Direct child has to be subtracted!
		area -= contourArea(contours,hierarchy,first_child);

		// Search next contours on same level
		child = hierarchy[first_child][0];
		while(child >= 0) {
 			area -= cv::contourArea(contours[child]);//,hierarchy,child);
			child = hierarchy[child][0];
		}

		// Search perv contours on same level
		child = hierarchy[first_child][1];
		while(child >= 0) {
 			area -= cv::contourArea(contours[child]);//,hierarchy,child);
			child = hierarchy[child][0];
		}

	}

	return area;

}

std::vector<int> getChildren(std::vector<cv::Vec4i>& hierarchy, size_t i) {

	if(hierarchy[i][2] == -1)
		return {};

	std::vector<int> children = {hierarchy[i][2]};

	int child;

	// Search next contours on same level
	child = hierarchy[children[0]][0];
	while(child >= 0) {
 		children.push_back(child);
		child = hierarchy[child][0];
	}

	// Search prev contours on same level
	child = hierarchy[children[0]][1];
	while(child >= 0) {
 		children.push_back(child);
		child = hierarchy[child][0];
	}

	return children;

}

std::vector<size_t> getObjectContours(std::vector<cv::Vec4i>& hierarchy) {

	std::vector<size_t> ret = {};
	std::vector<int> parents = {},children,grand_children,tmp_parents;

	// Get root objects
	for(size_t i = 0; i < hierarchy.size(); i++) {
		if(hierarchy[i][3] == -1) {
			parents.push_back(i);
		}
	}

	while(parents.size() > 0) {

		tmp_parents = {};

		for(int p : parents) {

			// Parents belong to objects
			ret.push_back(p);

			// Get children
			children = getChildren(hierarchy,p);

			for(int c : children) {

				// Get grand children
				grand_children = getChildren(hierarchy,c);

				for(int g : grand_children) {
					tmp_parents.push_back(g);
				}

			}

		}

		parents = tmp_parents;

	}

	return ret;

}

std::vector<cv::Point> contourInfo(const std::vector<cv::Point>& contour, bool with_barycenter) {

	std::vector<cv::Point> ret(4,cv::Point(-1,-1));
	int minx,maxx,miny,maxy;

	minx = maxx = contour[0].x;
	miny = maxy = contour[0].y;
	for(size_t hh = 1; hh < contour.size(); hh++) {
		minx = std::min(minx,contour[hh].x);
		miny = std::min(miny,contour[hh].y);
		maxx = std::max(maxx,contour[hh].x);
		maxy = std::max(maxy,contour[hh].y);
	}
	ret[0] = cv::Point(minx,miny);
	ret[1] = cv::Point(maxx,maxy);
	ret[2] = cv::Point(minx + (maxx-minx)/2,miny + (maxy-miny)/2);

	if(with_barycenter) {
		ret[3] = ocv::contourMassCenter(contour,minx,maxx,miny,maxy);
	}

	return ret;

}


void getNeighboursInCircle(int inner_radius, int outer_radius, std::vector<cv::Point>& pixels) {

	float dt,d1 = 2*outer_radius*outer_radius,d2 = 2*inner_radius*inner_radius;
	pixels = std::vector<cv::Point>();

	for(int y = -outer_radius; y <= outer_radius; y++) {
		for(int x = -outer_radius; x <= outer_radius; x++) {
			dt = y*y+x*x;
			if(dt <= d1 && dt >= d2)
				pixels.push_back(cv::Point(x,y));
		}
	}

}

}
