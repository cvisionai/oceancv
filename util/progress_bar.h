#pragma once

#include <iostream>
#include <vector>

namespace ocv {

/**
 * Creates a progress bar for the command line interface
 **/
class progress_bar {

public:
	progress_bar(const std::vector<std::string>& elements, size_t width = 50);
	void advance(size_t i);
	void message(std::string msg);

private:
	std::vector<std::string> _elements;
	size_t _width;
	size_t _current;
	std::string _message;
	size_t _msg_length;
	bool _have_printed;

};

}
