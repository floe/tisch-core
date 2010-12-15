TOPDIR = ../../

LIBS = libsimplegl.$(LSF)
APPS = gldemo osmdemo
INCS = *.h
OBJS = tinyxmlerror.o tinyxmlparser.o tinyxml.o X3DTools.o X3DRender.o OSMRender.o GLUTWindow.o

CFLAGS += -I$(TOPDIR)/libs/simplecv -I$(TOPDIR)/libs/tools
LDFLAGS += -lsimplecv -ltools -loscpack -L$(TOPDIR)/libs/simplecv -L$(TOPDIR)/libs/tools -L$(TOPDIR)/libs/osc

ifneq ($(OS),iPhone)
  OBJS += GLUTWrapper.o
else
  OBJS += GLUTWrapper.oo
endif

include $(TOPDIR)/scripts/rules.mk

