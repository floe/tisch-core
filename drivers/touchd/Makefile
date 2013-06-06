TOPDIR = ../../

CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/libs/osc/ -I$(TOPDIR)/libs/simplecv/ -I$(TOPDIR)/libs/simplegl/ -I$(TOPDIR)
LDFLAGS += -ltools -loscpack -lsimplecv -lsimplegl -L$(TOPDIR)/libs/tools/ -L$(TOPDIR)/libs/osc/ -L$(TOPDIR)/libs/simplecv/ -L$(TOPDIR)/libs/simplegl/

APPS=touchd
MORE=*.xml
OBJS=AreaFilter.o BandpassFilter.o BGSubFilter.o FlipFilter.o LowpassFilter.o SpeckleFilter.o SplitFilter.o ThreshFilter.o Pipeline.o FlashControl.o BlobList.o Camera.o Configurator.o Option.o

include $(TOPDIR)/scripts/rules.mk

