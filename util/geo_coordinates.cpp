#include "oceancv/util/geo_coordinates.h"

namespace ocv {
	
	DMS::DMS() : _c("0"),_d(0),_m(0),_s(0) {}
	DMS::DMS(std::string c,double d,double m,double s) : _c(c), _d(d), _m(m), _s(s) {}
	
	double DMS::d() const {
		return _d;
	}
	void DMS::d(double d) {
		_d = d;
	}
		
	double DMS::m() const {
		return _m;
	}
	void DMS::m(double m) {
		_m = m;
	}
		
	double DMS::s() const {
		return _s;
	}
	void DMS::s(double s) {
		_s = s;
	}
		
	std::string DMS::c() const {
		return _c;
	}
	void DMS::c(std::string c) {
		_c = c;
	}
		
	double DMS::operator[](size_t idx) {
		switch(idx) {
			case 0:
				if(_c == "-" || _c == "S" || _c == "s" || _c == "W" || _c == "w")
					return -1;
				else
					return 1;
			break;
			case 1:
				return _d;
			break;
			case 2:
				return _m;
			break;
			case 3:
				return _s;
			break;
			default:
				return -2;
			break;
		}
	}
	
	double DMS::numericDecimalDegrees() const {
		std::cout << _d << " " << _m << " " << _s << std::endl;
		double dec = 1.0*_d+(_m*60.0+_s)/3600.0;
		if(_c == "-" || _c == "S" || _c == "s" || _c == "W" || _c == "w")
			return -dec;
		else
			return dec;
	}
	
	
	LatLon::LatLon(double lat, double lon) : _lat(lat), _lon(lon) {}
		
	double LatLon::lat() const {
		return _lat;
	}
	void LatLon::lat(double lat) {
		_lat = lat;
	}
	double LatLon::lon() const {
		return _lon;
	}
	void LatLon::lon(double lon) {
		_lon = lon;
	}
	
	double LatLon::distance(double lat,double lon) const {
		return ocv::DecLatLonDistance(_lat,_lon,lat,lon);
	}
	
	
	
	
// Conversions between array data representations
	 
	ocv::DMS DMS2DecDeg(std::string c,double d,double m,double s) {
		return ocv::DMS(c,1.0*d+(m*60+s)/3600);
	}
	
	ocv::DMS DMS2DecMin(std::string c,double d,double m,double s) {
		return ocv::DMS(c,d,1.0*(m*60+s)/60);
	}
	
	ocv::DMS DecDeg2DMS(std::string c,double dec) {
	
		double d = floor(dec);
		double min = 60.0*(dec-d);
		double m = floor(min);
		double s = 60.0*(min-m);
		
		return ocv::DMS(c,d,m,s);
		
	}
	
	ocv::DMS DecDeg2DecMin(std::string c,double dec) {
		double d = floor(dec);
		double min = 60.0*(dec-d);
		return ocv::DMS(c,d,min);
	}
	
	ocv::DMS DecMin2DMS(std::string c,double deg,double min) {
		double m = floor(min);
		double s = 60.0*(min-m);
		return ocv::DMS(c,deg,m,s);
	}
	
	ocv::DMS DecMin2DecDeg(std::string c,double deg,double min) {
		double m = floor(min);
		double s = 60.0*(min-m);
		return ocv::DMS(c,0.0+deg+(m*60+s)/3600);
	}
	

// Conversions between array representations and string representations

	std::string DMS2DMSString(std::string c,double d,double m,double s) {
		std::stringstream ss;
		ss << c << int(d) << "°" << int(m) << "'" << std::setprecision(15) << s << "''";
		return ss.str();
	}
	
	std::string DecDeg2DecDegString(std::string c,double d) {
		std::stringstream ss;
		ss << c << std::setprecision(15) << d << "°";
		return ss.str();
	}
	
	std::string DecMin2DecMinString(std::string c,double d,double m) {
		std::stringstream ss;
		ss << c << int(d) << "°" << std::setprecision(15) << m << "'";
		return ss.str();
	}
	
	std::vector<size_t> separatorSearch(std::string str) {
		return {ocv::inArray(std::vector<std::string>({"+","-","S","N","E","W"}),str.substr(0,1)),str.find_first_of("°"),str.find_first_of("'"),str.find_first_of("''")};
	}
	
	ocv::DMS DMSString2DMS(std::string dms) {
		std::vector<std::size_t> s = ocv::separatorSearch(dms);
		return DMS(dms.substr(0,s[0]),atof(dms.substr(s[0],s[1]-s[0]).c_str()),atof(dms.substr(s[1]+2,s[2]-s[1]-2).c_str()),atof(dms.substr(s[2]+1,s[3]-s[2]-1).c_str()));
	}
		
	ocv::DMS DecDegString2DecDeg(std::string dec_deg) {
		std::vector<std::size_t> s = ocv::separatorSearch(dec_deg);
		return DMS(dec_deg.substr(0,s[0]),atof(dec_deg.substr(s[0],s[1]-s[0]).c_str()));
	}
	
	ocv::DMS DecMinString2DecMin(std::string dec_min) {
		std::vector<std::size_t> s = ocv::separatorSearch(dec_min);
		return DMS(dec_min.substr(0,s[0]),atof(dec_min.substr(s[0],s[1]-s[0]).c_str()),atof(dec_min.substr(s[1]+2,s[2]-s[1]-2).c_str()));
	}


// Arbitrary string to string representation

	std::string toGeoString(std::string ll,std::string type) {
	
		std::string dec_deg = ocv::toDecDegString(ll);
		
		std::string c;
		double d;
		
		if(ocv::inArray({'+','-','S','N','E','W'},dec_deg[0])) {
			c = dec_deg[0];
			d = atof(dec_deg.substr(1,dec_deg.length()-1).c_str());
		} else {
			c = "";
			d = atof(dec_deg.substr(0,dec_deg.length()-1).c_str());
		}
				
		if(type == "DMS") {
			DMS t = DecDeg2DMS(c,d);
			return DMS2DMSString(t.c(),t.d(),t.m(),t.s());
		} else if(type == "DecMin") {
			DMS t = DecDeg2DecMin(c,d);
			return DecMin2DecMinString(t.c(),t.d(),t.m());
		}
		return dec_deg;
		
	}


// Arbitrary string to DecDeg string conversion (universal guesser function)

	std::string toDecDegString(std::string ll) {
	
		
		std::vector<std::string> parts = {};

		std::string tmp = "";
		size_t part = 0;

		std::string compass = "";
		std::string neg = "";
		double deg = -1;
		double min = -1;
		double sec = -1;

		for(size_t i = 0; i < ll.length(); i++) {

//			if(int(ll[i]) == 194 && i+1 < ll.size() && int(ll[i+1]) == 176) {
//				parts.push_back(tmp);
//				deg = part;
//				part++;
//				tmp = "";
//				i++;
//				continue;
//			}
			
			if(ocv::inArray(std::vector<std::string>({"S","E","W","N"}),ll.substr(i,1))) {
				compass = ll[i];
			} else if(ll[i] == '-' || ll[i] == '+') {
				neg = ll[i];
//			} else if(int(ll[i]) == 248) {
//				parts.push_back(tmp);
//				deg = part;
//				part++;
//				tmp = "";
			} else if(ll[i] == '\'' && (i+2 > ll.size() || ll[i+1] != '\'')) {
				parts.push_back(tmp);
				min = part;
				part++;
				tmp = "";
			} else if(ll[i] == '\'' && i+1 < ll.size() && ll[i+1] == '\'') {
				parts.push_back(tmp);
				sec = part;
				part++;
				tmp = "";
				i++;
			} else if(ocv::inArray({',',' ',':'},ll[i])) {
				if(tmp != "") {
					parts.push_back(tmp);
					part++;
					tmp = "";
				}
			} else {
				tmp += ll[i];
			}

		}
		
		if(tmp != "")
			parts.push_back(tmp);
		
		if(parts.size() > 3) {
			std::cout << "Too many parts." << std::endl;
			return 0;
		}
		
		if(deg < 0) {
			for(size_t i = 0; i < parts.size(); i++) {
				if(min == i || sec == i)
					continue;
				deg = i;
				break;
			}
		}
		if(min < 0) {
			for(size_t i = 0; i < parts.size(); i++) {
				if(deg == i || sec == i)
					continue;
				min = i;
				break;
			}
		}
		if(sec < 0) {
			for(size_t i = 0; i < parts.size(); i++) {
				if(min == i || deg == i)
					continue;
				sec = i;
				break;
			}
		}
		
		if(deg >= 0)
			deg = atof(parts[deg].c_str());
		if(min >= 0)
			min = atof(parts[min].c_str());
		if(sec >= 0)
			sec = atof(parts[sec].c_str());
		
		if(compass == "") {
			compass = neg;
		}

		if(deg < 0) {
			std::cout << "Could not decode, missing degree." << std::endl;
			return 0;
		} else {
			if(min < 0) {
				// Decimal degrees
				return ocv::DecDeg2DecDegString(compass,deg);
			} else {
				if(sec < 0) {
					// Decimal minutes
					DMS t = DecMin2DecDeg(compass,deg,min);
					return DecDeg2DecDegString(t.c(),t.d());
				} else {
					// DMS
					DMS t = DMS2DecDeg(compass,deg,min,sec);
					return DecDeg2DecDegString(t.c(),t.d());
				}
			}
		}
		
	}
	
	double DecLatLonDistance(double lat_1,double lon_1,double lat_2,double lon_2) {
	
		double lat_1_r = ocv::deg2rad(lat_1);
		double lon_1_r = ocv::deg2rad(lon_1);
		double lat_2_r = ocv::deg2rad(lat_2);
		double lon_2_r = ocv::deg2rad(lon_2);

		double lat_offset = lat_1_r - lat_2_r;
		double lon_offset = lon_1_r - lon_2_r;

		double alpha = 2.0 * asin(sqrt(pow(sin(lat_offset / 2), 2) + cos(lat_1_r) * cos(lat_2_r) * pow(sin(lon_offset / 2), 2)));
		return alpha * 6371000; // Earth's radius
		
	}
	
	ocv::LatLon pixelLatitudeLongitude(size_t px,size_t py,float heading,float img_lat,float img_lon,float altitude,size_t img_w,size_t img_h,float imu_offset) {
	
	float sin_a = sin(ocv::deg2rad(heading));
	float cos_a = cos(ocv::deg2rad(heading));
	
	float w_m = 1.0 * (img_w - 2 * px) / img_w * altitude;
	float h_m = 1.0 * (img_h - 2 * py) / img_h * altitude * 0.75;
	
	return ocv::LatLon(img_lat + cos_a * imu_offset - sin_a * w_m - cos_a * h_m,img_lon - sin_a * imu_offset - cos_a * w_m + sin_a * h_m);
	
}
	
}
