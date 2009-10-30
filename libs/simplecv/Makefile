TOPDIR = ../../
CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/calibd/

LIBS = libsimplecv.$(LSF)
OBJS = GLUTWindow.o IntensityImage.o RGBImage.o YUV420Image.o YUV420SPImage.o YUYVImage.o Line.o Circle.o ShortImage.o ColorLUT.o

ifneq (,$(findstring dc1394,$(LDFLAGS)))
  OBJS += DCImageSource.o
endif

ifneq ($(OS),iPhone)
  OBJS += mmx.o GLUTWrapper.o PicoPNG.o
  #APPS = cvdemo
else
  OBJS += GLUTWrapper.oo PicoPNG.oo
endif

ifeq ($(OS),Linux)
  OBJS += V4LImageSource.o 
endif

include $(TOPDIR)/scripts/rules.mk

cvdemo.prof: cvdemo.o $(OBJS)
	g++ $(CFLAGS) $^ -o $@ ../simplecv/*.o ../tools/*.o

