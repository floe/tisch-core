TOPDIR = ../
CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR) -I. -Ifeatures
LDFLAGS += -ltools -L$(TOPDIR)/libs/tools/

APPS = gestured
LIBS = libgestures.$(LSF)
OBJS = Region.o Gesture.o Factory.o InputState.o $(patsubst %.cc,%.o,$(wildcard features/*.cc))

include $(TOPDIR)/scripts/rules.mk

