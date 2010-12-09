TOPDIR = ../../

CFLAGS += -I$(TOPDIR) -Iosc/ -DOSC_HOST_LITTLE_ENDIAN
CPPFLAGS = $(CFLAGS)

LIBS=libosc.$(LSF)
OBJS=ip/posix/NetworkingUtils.o ip/posix/UdpSocket.o ip/IpEndpointName.o osc/OscReceivedElements.o osc/OscPrintReceivedElements.o osc/OscTypes.o osc/OscOutboundPacketStream.o

include $(TOPDIR)/scripts/rules.mk

