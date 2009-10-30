CC=g++
CFLAGS=-Wall -O3 -g `pkg-config --cflags libglade-2.0` -I../../tools/ -I../../../simplecv/
CPPFLAGS=$(CFLAGS)

TARGET=dt_test

all: $(TARGET)

dt_test.o: dt_test.c callbacks.h input.h
callbacks.o: callbacks.c callbacks.h input.h
input.o: input.c input.h callbacks.h
dt_eeparse.o: dt_eeparse.c dt_eeparse.h
#UDPSender.o: UDPSender.cc UDPSender.h
#BasicBlob.o: BasicBlob.cc BasicBlob.h

dt_test: dt_test.o callbacks.o input.o dt_eeparse.o ../../tools/BasicBlob.o ../../tools/UDPSender.o
	$(CC) $(CFLAGS) -o $@ $^ -Wl,--export-dynamic `pkg-config --libs libglade-2.0`

clean:
	rm -f *.o *~ $(TARGET)
