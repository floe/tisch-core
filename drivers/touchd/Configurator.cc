#include "Configurator.h"

Configurator::Configurator(GLUTWindow* targetWindow, Filter* currentFilter) {
	win = targetWindow;
	filter = currentFilter;

}

Configurator::~Configurator() {
}

void Configurator::update(Filter* currentFilter) {
	filter = currentFilter;
}

void Configurator::showInfo() {
	glColor4f(0.0, 1.0, 0.0, 1.0);

	// retrieve filter name
	std::string name = typeid(*filter).name();
	const char* myfiltername = name.c_str();
	// skip string length
	while (myfiltername && (*myfiltername >= '0') && (*myfiltername <= '9'))
		myfiltername++;

	win->print(std::string("Configuration"), 10, 40);

	// dirty
	if (strcmp(myfiltername, "ThreshFilter") == 0) {
		myThreshFilter = (ThreshFilter*) filter;

		std::ostringstream result;
		result << "TH Min: ";
		result << myThreshFilter->threshold_min;

		win->print(result.str(), 10, 60);

		// reset result
		result.str("");
		result << "TH Max: ";
		result << myThreshFilter->threshold_max;

		win->print(result.str(), 10, 80);

	}

	win->swap();
}

// value manipulation


void Configurator::increaseValue() {
	if (myThreshFilter->threshold_min < 255)
		myThreshFilter->threshold_min++;
}

void Configurator::decreaseValue() {
	if (myThreshFilter->threshold_min > 0)
		myThreshFilter->threshold_min--;
}
