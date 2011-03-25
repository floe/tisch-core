/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
 *     Copyright (c) 2011 by Norbert Wiedermann, <wiederma@in.tum.de>      *
 *   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
 \*************************************************************************/

#include "Configurator.h"

/*
 * Constructor
 */
Configurator::Configurator(GLUTWindow* targetWindow, Filter* currentFilter) {
	win = targetWindow;

	updateCurrentFilter(currentFilter);

}

Configurator::~Configurator() {
}

/*
 * updating the Configurator when switching the filter
 */
void Configurator::updateCurrentFilter(Filter* currentFilter) {
	filter = currentFilter;
}

/*
 * displaying the settings of the current selected filter
 */
void Configurator::showInfo() {
	int xCoord = 10;
	int yCoord = 40;

	glColor4f(0.0, 1.0, 0.0, 1.0);
	win->print(std::string("Configuration: (h)elp"), xCoord, yCoord);

	for (int i = 0; i < filter->getOptionCount(); i++) {
		yCoord += 20;

		// highlight the current selected option
		if (i == filter->getCurrentOption()) {
			glColor4f(0.0, 0.0, 1.0, 1.0); // blue
		} else {
			glColor4f(0.0, 1.0, 0.0, 1.0); // green
		}
		// display each option of filter in a line
		std::ostringstream OptionValue;
		OptionValue.str("");
		OptionValue << filter->getOptionName(i) << ": ";
		OptionValue << filter->getOptionValue(i);
		win->print(OptionValue.str(), xCoord, yCoord);

	}
}

/*
 * displaying additional help to work with the Configurator
 */
void Configurator::showHelp() {
	int xCoord = 100;
	int yCoord = 150;

	glColor4f(1.0, 0.0, 0.0, 1.0);
	win->print(std::string("use (tab) to toggle selected value"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (i) to increase selected value"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (d) to decrease selected value"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (h) to show/hide this help"), xCoord, yCoord);

}

