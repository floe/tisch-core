TOPDIR  = ../../../
PACKAGE = libtisch

WRAP = _$(PACKAGE).so $(PACKAGE).py $(PACKAGE).pth
DOCS = example.py

SWIGFLAGS = -c++ -python

include $(TOPDIR)/scripts/rules.mk

PYTHON  = $(lastword $(subst /, ,$(lastword $(sort $(wildcard /usr/include/python?.?/)))))
PYTHON := -I/usr/include/$(PYTHON)/ -l$(PYTHON)

%.py: %_wrap.cxx

%.pth:
	echo $* > $@

_%.so: %_wrap.cxx
	g++ $(CFLAGS) $(SLFLAGS) $(LDFLAGS) -I$(TOPDIR) -I$(TOPDIR)/libs/osc -I$(TOPDIR)/libs/tools -I$(TOPDIR)/gestured -I$(TOPDIR)/widgets -I$(TOPDIR)/libs/simplecv -I$(TOPDIR)/libs/simplegl -I$(TOPDIR)/gestured/features -L$(TOPDIR)/libs/tools -L$(TOPDIR)/gestured -L$(TOPDIR)/widgets -L$(TOPDIR)/libs/simplecv -L$(TOPDIR)/libs/simplegl $< -ltools -loscpack -lgestures -lsimplecv -lsimplegl -lwidgets -lGL -lGLU $(PYTHON) -o $@ 

clean:
	-rm _$(PACKAGE).so $(PACKAGE).pyc $(PACKAGE).pth

distclean: clean
	-rm $(PACKAGE)_wrap.* $(PACKAGE).py

