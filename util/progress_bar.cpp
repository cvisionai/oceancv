#include "oceancv/util/progress_bar.h"

namespace ocv {

progress_bar::progress_bar(const std::vector<std::string>& elements, size_t width) : _elements(elements), _width(width) {
	_current = 0;
	_message = "";
	_msg_length = 0;
	_have_printed = false;
}

void progress_bar::advance(size_t idx) {

	// Move back to beginning of the line
	if(_have_printed) {
		for(size_t i = 0; i < 5 + _width + _elements[_current].length() + _msg_length; i++)
			std::cout << "\r";
	}

	_have_printed = true;
	_current = idx;

	size_t frac_done = _width * _current / (_elements.size()-1);

	std::string percentage_done = " " + std::to_string(100*_current / (_elements.size()-1)) + "%";
	_message += percentage_done;

	// Replot
	std::cout << "[";
	for(size_t i = 0; i < frac_done; i++)
		std::cout << "-";
	for(size_t i = frac_done; i < _width; i++)
		std::cout << " ";
	std::cout << "] " << _elements[_current] << " | " << _message << std::flush;

	_msg_length = _message.length();
	_message = "";

	if(_current == _elements.size() - 1)
		std::cout << std::endl;

}

void progress_bar::message(std::string msg) {
	_message = msg;
	advance(_current);
}

}
