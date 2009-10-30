TOPDIR = ../../

LIBS = libsimplegl.$(LSF)
APPS = gldemo # cvdemo
OBJS = tinyxmlerror.o tinyxmlparser.o tinyxml.o X3DTools.o X3DRender.o #GLUTWindow.o GLUTWrapper.o
CFLAGS += -I$(TOPDIR)/libs/simplecv -I$(TOPDIR)/libs/tools
LDFLAGS += -lsimplecv -ltools -L$(TOPDIR)/libs/simplecv -L$(TOPDIR)/libs/tools

include $(TOPDIR)/scripts/rules.mk
