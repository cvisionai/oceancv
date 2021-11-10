#pragma once

#include <string>
#include <vector>
#include <math.h>
#include <iomanip>

#include "oceancv/util/misc_func.h"

namespace ocv {

	/**
	 * This class (DMS = Degree Minute Second) holds a geographic coordinate (either latitute or longitude),
	 * encoded by those three values (with decimal seconds if you like).
	 */
	class DMS {
	
	public:
		DMS();
		DMS(std::string c,double d ,double m = -1,double s = -1);
		
		// Getter and setter for the base values
		double d() const;
		void d(double d);
		double m() const;
		void m(double m);
		double s() const;
		void s(double s);
		std::string c() const;
		void c(std::string c);
		
		/**
		 * Very shorthand getter method for the different base values. Not sure why it built it
		 * like this - its horrible. Sorry.
		 * @param idx the index of the value you would like to get (0: +/-1 depending on the position
		 *            of the DMS coordinate - e.g. South/West is -1; 1: degree part; 2: minute part;
		 *            3: second part)
		 * @return the requested value of the geo coordinate or -2 if wrong parameter given
		 */
		double operator[](size_t idx);
		
		/**
		 * Converts the DMS triple to one decimal degree value
		 * @return the decimal degree version of the DMS values in this object
		 */
		double numericDecimalDegrees() const;
		
	private:
		std::string _c;
		double _d;
		double _m;
		double _s;
		
	};
	
	
	/**
	 * A class to store a decimal degree latitude & longitude pair
	 */
	class LatLon {
		
	public:
		LatLon(double lat = 0, double lon = 0);
		
		double lat() const;
		void lat(double lat);
		double lon() const;
		void lon(double lon);
		
		double distance(double lat,double lon) const;
		
	private:
		double _lat;
		double _lon;
		
	};
	
// Conversions between array data representations
ocv::DMS DMS2DecDeg(std::string c,double d,double m,double s);
ocv::DMS DMS2DecMin(std::string c,double d,double m,double s);
ocv::DMS DecDeg2DMS(std::string c,double dec);
ocv::DMS DecDeg2DecMin(std::string c,double dec);
ocv::DMS DecMin2DMS(std::string c,double deg,double min);
ocv::DMS DecMin2DecDeg(std::string c,double deg,double min);

// Conversions between array representations and string representations
std::string DMS2DMSString(std::string c,double d,double m,double s);
std::string DecDeg2DecDegString(std::string c,double d);
std::string DecMin2DecMinString(std::string c,double d,double m);
std::vector<size_t> separatorSearch(std::string str);
ocv::DMS DMSString2DMS(std::string dms);
ocv::DMS DecDegString2DecDeg(std::string dec_deg);
ocv::DMS DecMinString2DecMin(std::string dec_min);

// Arbitrary string to string representation
std::string toGeoString(std::string ll,std::string type = "DecDeg");

// Arbitrary string to DecDeg string conversion (universal guesser function)
std::string toDecDegString(std::string ll);
	
/**
 * Distance between two lat/lon points in meters
 **/
double DecLatLonDistance(double lat_1,double lon_1,double lat_2,double lon_2);

/**
 * Computes the lat/lon in decimal degrees of a seafloor pixel in an image but assumes:
 *  1) a flat seafloor 2) a vertical view down onto the seafloor 3) the DeepSurveyCamera imaging system (focal length etc.)
 * @param px the pixel x coordinate
 * @param py the pixel y coordinate
 * @param heading the direction of travel of the camera platform (e.g. AUV)
 * @param img_lat the latitude of the image as recorded by the IMU/USBL/LBL/...
 * @param img_lon the longitute of the image as recorded by ...
 * @param altitude the distance from the camera to the seafloor (remember: perpendicular view axis!)
 * @param img_w the image widths in pixels
 * @param img_h the image height in pixels
 * @param imu_offset the distance of the IMU (i.e. the position of the given lat/lon pair) to the camera center
 *                   assumes that IMU and camera lie on the same axis in the diretion of travel and have the same
 *                   altitude above the seafloor
 * @return an ocv::LatLon object with the geo position of the pixel
 */
ocv::LatLon pixelLatitudeLongitude(size_t px,size_t py,float heading,float img_lat,float img_lon,float altitude,size_t img_w,size_t img_h,float imu_offset);

}
