TOPDIR = ../../

CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/libs/simplecv/ -I$(TOPDIR)/libs/simplegl/ -I$(TOPDIR)
LDFLAGS += -ltools -lsimplecv -lsimplegl -L$(TOPDIR)/libs/tools/ -L$(TOPDIR)/libs/simplecv/ -L$(TOPDIR)/libs/simplegl/

ifdef USE_BIGTOUCH
  CFLAGS += -DUSE_BIGTOUCH -O2 -I$(TOPDIR)/libs/simplecv/ledtouch $(shell pkg-config --cflags opencv)
  LDFLAGS += -lusb -lpthread $(shell pkg-config --libs opencv)
endif

APPS=touchd #touchd.prof
OBJS=touchd.o FlashControl.o ImageSet.o Pipeline.o Blob.o BlobList.o Camera.o Settings.o

include $(TOPDIR)/scripts/rules.mk

touchd.prof: $(OBJS)
	g++ $(LDFLAGS) $(TOPDIR)/simplegl/*.o $(TOPDIR)/simplecv/*.o $(TOPDIR)/tools/*.o $+ -o $@

