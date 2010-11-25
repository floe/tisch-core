TOPDIR = ../../

CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/libs/osc/ -I$(TOPDIR)/libs/simplecv/ -I$(TOPDIR)/libs/simplegl/ -I$(TOPDIR)
LDFLAGS += -ltools -losc -lsimplecv -lsimplegl -L$(TOPDIR)/libs/tools/ -L$(TOPDIR)/libs/osc/ -L$(TOPDIR)/libs/simplecv/ -L$(TOPDIR)/libs/simplegl/

ifdef USE_BIGTOUCH
  CFLAGS += -DUSE_BIGTOUCH -O2 -I$(TOPDIR)/libs/simplecv/ledtouch $(shell pkg-config --cflags opencv)
  LDFLAGS += -lusb -lpthread $(shell pkg-config --libs opencv)
endif

APPS=touchd pipetest #touchd.prof
OBJS=Pipeline2.o FlashControl.o ImageSet.o Pipeline.o Blob.o BlobList.o Camera.o Settings.o Filter.o

include $(TOPDIR)/scripts/rules.mk

touchd.prof: $(OBJS)
	g++ $(LDFLAGS) $(TOPDIR)/simplegl/*.o $(TOPDIR)/simplecv/*.o $(TOPDIR)/tools/*.o $+ -o $@

