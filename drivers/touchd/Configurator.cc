#include "Configurator.h"

Configurator::Configurator(GLUTWindow* targetWindow, Filter* currentFilter) {
	win = targetWindow;
	filter = currentFilter;

	update(currentFilter);

}

Configurator::~Configurator() {
}

void Configurator::update(Filter* currentFilter) {
	filter = currentFilter;

	// retrieve filter name
	std::string name = typeid(*filter).name();
	const char* myfiltername = name.c_str();
	// skip string length
	while (myfiltername && (*myfiltername >= '0') && (*myfiltername <= '9'))
		myfiltername++;

	if (strcmp(myfiltername, "ThreshFilter") == 0) {
		myThreshFilter = (ThreshFilter*) filter;
		numberOfOptions = 2;
	} else {
		myThreshFilter = 0;
	}

	// when switching the filter set first option active
	activateFirstOption();
}

void Configurator::showInfo() {
	glColor4f(0.0, 1.0, 0.0, 1.0);

	win->print(std::string("Configuration"), 10, 40);

	// dirty
	if (myThreshFilter != 0) {

		std::ostringstream result;
		result.str("");

		result << "TH Min: ";
		result << myThreshFilter->threshold_min;
		if (toggle == 0) {
			glColor4f(0.0, 0.0, 1.0, 1.0); // blue
		} else {
			glColor4f(0.0, 1.0, 0.0, 1.0); // green
		}
		win->print(result.str(), 10, 60);

		// reset result
		result.str("");
		result << "TH Max: ";
		result << myThreshFilter->threshold_max;

		if (toggle == 0) {
			glColor4f(0.0, 1.0, 0.0, 1.0); // green
		} else {
			glColor4f(0.0, 0.0, 1.0, 1.0); // blue
		}
		win->print(result.str(), 10, 80);
	}

	win->swap();
}

// value manipulation
void Configurator::toggleOption() {

	toggle = (toggle + 1) % numberOfOptions;

	if (myThreshFilter != 0) {
		// switch possible options for Threshold Filter
		switch (toggle) {
		case 0:
			p_currentOption = &(myThreshFilter->threshold_min);
			break;
		case 1:
			p_currentOption = &(myThreshFilter->threshold_max);
			break;
		}
	}

}

void Configurator::activateFirstOption() {
	toggle = 0;

	if (myThreshFilter != 0) {
		p_currentOption = &(myThreshFilter->threshold_min);
	}
}

void Configurator::increaseValue() {

	if (*p_currentOption < 255)
		(*p_currentOption)++;

}

void Configurator::decreaseValue() {
	if (*p_currentOption > 0)
		(*p_currentOption)--;

}

