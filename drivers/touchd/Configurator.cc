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

	glColor4f(1.0, 1.0, 1.0, 1.0); // white
	win->drawRectangleBackground(xCoord, yCoord, 225, (filter->getOptionCount()+1)*20, 2);

	glColor4f(0.0, 0.0, 0.0, 1.0); // black
	win->print(std::string("Configuration: (h)elp"), xCoord, yCoord);

	for (int i = 0; i < filter->getOptionCount(); i++) {
		yCoord += 20;

		// highlight the current selected option
		if (i == filter->getCurrentOption()) {
			glColor4f(1.0, 0.5, 0.0, 1.0); // orange
		} else {
			glColor4f(0.0, 0.0, 0.0, 1.0); // black
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
	int yCoord = 140;

	glColor4f(1.0, 1.0, 1.0, 1.0); // white
	win->drawRectangleBackground(xCoord, yCoord, 410, 180, 2);

	glColor4f(0.0, 0.0, 0.0, 1.0); // black
	win->print(std::string("use (tab) to toggle selected value"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (e) to switch to editing mode"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (i) to increase selected value"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (d) to decrease selected value"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (r) to reset the current filter"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (space) to reset all filters"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (s) to save config of CURRENT filter"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (S) to save config of ALL filters"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (h) to show/hide this help"), xCoord, yCoord);

}

void Configurator::showEditInfo() {
	int xCoord = 100;
	int yCoord = 140;

	glColor4f(1.0, 1.0, 1.0, 1.0); // white
	win->drawRectangleBackground(xCoord, yCoord, 450, 130, 2);

	glColor4f(0.0, 0.0, 0.0, 1.0); // black
	win->print(std::string("Your are now in editing mode."), xCoord, yCoord);
	yCoord += 30;
	win->print(std::string("Please enter a numerical value"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("to overwrite the selected value."), xCoord, yCoord);
	yCoord += 30;
	win->print(std::string("Finish your input with ENTER."), xCoord, yCoord);
	yCoord += 30;
	win->print(std::string("Leave editing mode without changes with ESC."), xCoord, yCoord);
}

void Configurator::showStoreInfo(int mode) {
	int xCoord = 100;
	int yCoord = 140;

	glColor4f(1.0, 1.0, 1.0, 1.0); // white
	win->drawRectangleBackground(xCoord, yCoord, 450, 100, 2);

	glColor4f(0.0, 0.0, 0.0, 1.0); // black
	win->print(std::string("Saving Mode"), xCoord, yCoord);
	yCoord += 30;
	win->print(std::string("Press ENTER to save current settings"), xCoord, yCoord);
	yCoord += 20;
	if(mode == 0) {
		win->print(std::string("of ALL filters"), xCoord, yCoord);
	}
	else if(mode == 1) {
		win->print(std::string("of CURRENT filter"), xCoord, yCoord);
	}
	yCoord += 30;
	win->print(std::string("Use ESC to leave without saving."), xCoord, yCoord);
}