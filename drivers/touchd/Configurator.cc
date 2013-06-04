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
	helpMode = false;
	editMode = false;
	visible =  false;
	updateCurrentFilter(currentFilter);

}

Configurator::~Configurator() {
}

void Configurator::draw() {
	if (!visible) return;
	showInfo();
	if (helpMode) showHelp();
	if (editMode) showEditInfo();
}

/*
 * updating the Configurator when switching the filter
 */
void Configurator::updateCurrentFilter(Filter* currentFilter) {
	filter = currentFilter;
	current = filter->getOptions().begin();
}

/*
 * displaying the settings of the current selected filter
 */
void Configurator::showInfo() {
	int xCoord = 10;
	int yCoord = 40;

	OptionList const& options = filter->getOptions();

	glColor4f(1.0, 1.0, 1.0, 1.0); // white
	win->drawRectangleBackground(xCoord, yCoord, 225, (options.size()+1)*20, 2);

	glColor4f(0.0, 0.0, 0.0, 1.0); // black
	win->print(std::string("Configuration: (h)elp"), xCoord, yCoord);

	for (OptionList::const_iterator opt = options.begin(); opt != options.end(); opt++) {
		yCoord += 20;

		// highlight the current selected option
		if (opt == current) {
			glColor4f(1.0, 0.5, 0.0, 1.0); // orange
		} else {
			glColor4f(0.0, 0.0, 0.0, 1.0); // black
		}

		// display each option of filter in a line
		std::ostringstream OptionValue;
		OptionValue.str("");
		OptionValue << opt->first << ": ";
		OptionValue << opt->second->get();
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
	//win->print(std::string("use (s) to save config of CURRENT filter"), xCoord, yCoord);
	//yCoord += 20;
	win->print(std::string("use (S) to save config of ALL filters"), xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("use (h) to show/hide this help"), xCoord, yCoord);

}

void Configurator::showEditInfo() {
	int xCoord = 100;
	int yCoord = 320;

	glColor4f(1.0, 1.0, 1.0, 1.0); // white
	win->drawRectangleBackground(xCoord, yCoord, 450, 100, 2);

	glColor4f(0.0, 0.0, 0.0, 1.0); // black
	win->print(std::string("Editing value for ")+current->first, xCoord, yCoord);
	yCoord += 20;
	win->print(std::string("New value: ")+userinput, xCoord, yCoord);
	yCoord += 30;
	win->print(std::string("Finish your input with ENTER."), xCoord, yCoord);
	yCoord += 30;
	win->print(std::string("Leave editing mode without changes with ESC."), xCoord, yCoord);
}

int Configurator::handleInput( unsigned char c ) {

	// show/hide configurator
	if (c == 'c') visible = !visible;

	if (!visible) return 0;

	// switching to editing mode
	if (editMode) {

		switch (c) {

			// ESC quits edit mode without applying changes
			case 0x1B:
				editMode = false;
				break;

			// backspace deletes last char
			case 0x08:
				if (userinput.size() > 0)
					userinput.resize(userinput.size()-1);
				break;

			// Enter finishes Input
			case 0x0D: {
				// parse input to double, 0.0 if a double couldn't be read
				double result = atof(userinput.c_str());
				// apply new value
				current->second->set(result);
				std::cout << "input was: " << result << std::endl;
				editMode = false; // close editing mode
				break;
			}

			default: 
				userinput += c;
				break;
		}

		return 1;
	}
	
	// processing keyboard entries as usual

	// show/hide help
	if (c == 'h') helpMode = !helpMode;

	// increase value
	if (c == 'i') current->second->inc();

	// decrease value
	if (c == 'd') current->second->dec();

	// activate editing mode: overwrite non bool variables with user input
	if (c == 'e') {
		if (filter->getOptions().size() > 0) {
			userinput = "";
			editMode = true;
		}
	}

	// toggle Option with Tab
	if (c == 0x09) {
		current++;
		if (current == filter->getOptions().end())
			current = filter->getOptions().begin();
	}

	return 0;
}

