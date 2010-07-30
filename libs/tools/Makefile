TOPDIR = ../../

LIBS = libtools.$(LSF)
INCS = *.h $(TOPDIR)/tisch.h
OBJS = Socket.o BasicBlob.o Thread.o
DOCS = $(TOPDIR)/README $(TOPDIR)/tisch.sh

LDFLAGS += -lpthread

ifeq ($(OS),Linux)

  LDFLAGS += $(shell pkg-config --libs   bluez dbus-1 2> /dev/null)
  CFLAGS  += $(shell pkg-config --cflags bluez dbus-1 2> /dev/null) -I.

  ifeq (bluetooth,$(findstring bluetooth,$(LDFLAGS)))
    ifeq (dbus,$(findstring dbus,$(LDFLAGS)))
      OBJS += Scanner.o 
    endif
  endif

endif

include $(TOPDIR)/scripts/rules.mk
