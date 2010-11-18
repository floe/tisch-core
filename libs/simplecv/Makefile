TOPDIR = ../../
CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/calibd/

LIBS = libsimplecv.$(LSF)
INCS = *.h
OBJS = IntensityImage.o RGBImage.o YUV420Image.o YUV420SPImage.o YUYVImage.o Line.o Circle.o ShortImage.o ColorLUT.o

ifdef USE_BIGTOUCH
  APPS += ledtouch/bttest ledtouch/BigTouchCalibration 
  OBJS += ledtouch/BigTouch.o ledtouch/FlatSensorImageSource.o ledtouch/cusbdev.o ledtouch/ItNorm.o 
  CFLAGS += -I. -Iledtouch/ -O2 $(shell pkg-config --cflags opencv)
  LDFLAGS += -lusb -lpthread $(shell pkg-config --libs opencv)
endif

ifneq (,$(findstring dc1394,$(LDFLAGS)))
  OBJS += DCImageSource.o
endif

ifneq (,$(findstring freenect,$(LDFLAGS)))
  SLFLAGS += -lusb-1.0
  OBJS += KinectImageSource.o
endif

ifneq ($(OS),iPhone)
  OBJS += mmx.o PicoPNG.o
else
  OBJS += PicoPNG.oo
endif

ifeq ($(OS),Linux)
  OBJS += V4LImageSource.o 
endif

include $(TOPDIR)/scripts/rules.mk

cvdemo.prof: cvdemo.o $(OBJS)
	g++ $(CFLAGS) $^ -o $@ ../simplecv/*.o ../tools/*.o

