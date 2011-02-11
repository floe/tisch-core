#ifndef _CONFIGURATOR_H_
#define _CONFIGURATOR_H_

#include <GLUTWindow.h>
#include <typeinfo>

#include <iostream>
#include <sstream>

#include "Filter.h"

using namespace std;

class Configurator {
public:
	Configurator(GLUTWindow* targetWindow, Filter* currentFilter);
	virtual ~Configurator();
	virtual void update(Filter* currentFilter);
	virtual void showInfo();
	virtual void increaseValue();
	virtual void decreaseValue();

protected:
	GLUTWindow* win;
	Filter* filter;
	ThreshFilter *myThreshFilter;

};

#endif // _CONFIGURATOR_H_
