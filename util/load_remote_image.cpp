#include "oceancv/util/load_remote_image.h"

namespace ocv {

// Private writer function, used in loadFromUrl (below)
size_t _curl_write_data(char *ptr, size_t size, size_t memb, void *data) {
	std::vector<uchar> *stream = (std::vector<uchar>*)data;
	size_t count = size * memb;
	stream->insert(stream->end(),ptr,ptr + count);
	return count;
}

bool loadFromUrl(std::string url,cv::Mat& img) {
	
	// The return value
	bool from_net = true;

	if(url.find("\"") == 0) {
		url = url.substr(1,url.length()-2);
	}

	// Initialize image data cache
	std::vector<uchar> stream;

	// Initialize the curl request
	CURL *curl = curl_easy_init();
	CURLcode res;

	if(curl) {
		
		// Set curl parameters
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
		
		// Perform curl request
		res = curl_easy_perform(curl);

		// Validate success
		if(res != CURLE_OK) {
			std::cout << "Curl request failed: " << curl_easy_strerror(res) << url << std::endl;
			from_net = false;
		}

		// Clean up
		curl_easy_cleanup(curl);
		
		// Store image in given cv::Mat
		img = cv::imdecode(stream,-1);
		
	}
	
	return from_net;

}

}
