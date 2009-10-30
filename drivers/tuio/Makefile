TOPDIR = ../../

CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR) -Iosc/ -DOSC_HOST_LITTLE_ENDIAN
CPPFLAGS = $(CFLAGS)
LDFLAGS += -ltools -L$(TOPDIR)/libs/tools/ 

APPS=TUIOtoTISCH TISCHtoTUIO
OBJS=osc/ip/posix/NetworkingUtils.o osc/ip/posix/UdpSocket.o osc/ip/IpEndpointName.o osc/osc/OscReceivedElements.o osc/osc/OscPrintReceivedElements.o osc/osc/OscTypes.o osc/osc/OscOutboundPacketStream.o

include $(TOPDIR)/scripts/rules.mk

