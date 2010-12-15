TOPDIR = ../
CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/libs/osc/ -I$(TOPDIR)/libs/simplegl/ -I$(TOPDIR)/libs/simplecv/ -I$(TOPDIR) -I$(TOPDIR)/libs/math/ -I$(TOPDIR)/libs/math/tnt/ -I$(TOPDIR)/libs/math/jama/
LDFLAGS += -ltools -loscpack -lsimplecv -lsimplegl -L$(TOPDIR)/libs/tools/ -L$(TOPDIR)/libs/osc/ -L$(TOPDIR)/libs/simplecv/ -L$(TOPDIR)/libs/simplegl/

APPS=calibd calibtool distort
INCS=*.h
OBJS=Calibration.o

include $(TOPDIR)/scripts/rules.mk

Calibration.old.o: Calibration.old.cc
	g++ $(CFLAGS) $< -o $@ $(shell pkg-config --cflags opencv)

test: test.o Calibration.o
	g++ $(LDFLAGS) $+ -o $@

test.old: test.o Calibration.old.o
	g++ $(LDFLAGS) $+ -o $@ $(shell pkg-config --libs opencv)

