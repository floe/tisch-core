TOPDIR = ../../

LIBS = libtools.$(LSF)
INCS = *.h $(TOPDIR)/tisch.h
OBJS = Socket.o BasicBlob.o Thread.o WebSocket.o TUIOOutStream.o TUIOInStream.o Log.o
DOCS = $(TOPDIR)/README $(TOPDIR)/scripts/tisch.sh $(TOPDIR)/scripts/calib.sh
#APPS = wstest

CFLAGS  += -I$(TOPDIR) -I$(TOPDIR)/libs/osc
LDFLAGS += -lpthread -loscpack -L$(TOPDIR)/libs/osc

ifeq ($(OS),linux)

  LDFLAGS += $(shell pkg-config --libs   bluez dbus-1 2> /dev/null)
  CFLAGS  += $(shell pkg-config --cflags bluez dbus-1 2> /dev/null) -I.

  ifeq (bluetooth,$(findstring bluetooth,$(LDFLAGS)))
    ifeq (dbus,$(findstring dbus,$(LDFLAGS)))
      OBJS += Scanner.o 
    endif
  endif

endif

include $(TOPDIR)/scripts/rules.mk
