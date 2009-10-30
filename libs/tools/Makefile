TOPDIR = ../../

LIBS = libtools.$(LSF)
OBJS = UDPSocket.o BasicBlob.o Thread.o
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
