TOPDIR = ../../

CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/libs/simplecv/ -I$(TOPDIR)
LDFLAGS += -ltools -lsimplecv -L$(TOPDIR)/libs/tools/ -L$(TOPDIR)/libs/simplecv/

APPS=touchd #touchd.prof
OBJS=touchd.o FlashControl.o ImageSet.o Pipeline.o Blob.o BlobList.o Camera.o Settings.o

include $(TOPDIR)/scripts/rules.mk

touchd.prof: $(OBJS)
	g++ $(LDFLAGS) $(TOPDIR)/simplecv/*.o $(TOPDIR)/tools/*.o $+ -o $@

