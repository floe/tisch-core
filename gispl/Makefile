TOPDIR = ../
CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/libs/osc/ -I$(TOPDIR) -I. -Ifeatures
LDFLAGS += -ltools -loscpack -L$(TOPDIR)/libs/tools/ -L$(TOPDIR)/libs/osc/

APPS = gestured
LIBS = libgispl.$(LSF)
INCS = *.h features/*.h
OBJS = Matcher.o Region.o Gesture.o Factory.o InputState.o $(patsubst %.cc,%.o,$(wildcard features/*.cc))

include $(TOPDIR)/scripts/rules.mk

