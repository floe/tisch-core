TOPDIR = ../../
CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/calibd/ -I$(TOPDIR)/libs/osc/

LIBS = libsimplecv.$(LSF)
INCS = *.h
OBJS = IntensityImage.o RGBImage.o YUV420Image.o YUV420SPImage.o YUYVImage.o Line.o Circle.o ShortImage.o ColorLUT.o

ifdef USE_BIGTOUCH
  APPS += ledtouch/bttest ledtouch/BigTouchCalibration 
  OBJS += ledtouch/BigTouch.o ledtouch/FlatSensorImageSource.o ledtouch/cusbdev.o ledtouch/ItNorm.o 
  CFLAGS += -I. -Iledtouch/ -O2 $(shell pkg-config --cflags opencv)
  LDFLAGS += -lusb -lpthread $(shell pkg-config --libs opencv)
endif

ifeq ($(ARCH),x86_64)
  OBJS += mmx.o mmx_threshold.o mmx_despeckle.o mmx_update.o mmx_intensity_c.o mmx_intensity_s.o mmx_subtract_cs.o mmx_subtract_sc.o
  ASFLAGS = -g -Dpax=rax -Dpbx=rbx -Dpcx=rcx -Dpdx=rdx -Dpsi=rsi -Dpdi=rdi
endif

ifeq ($(ARCH),i686)
  OBJS += mmx.o mmx_threshold.o mmx_despeckle.o mmx_update.o mmx_intensity_c.o mmx_intensity_s.o mmx_subtract_cs.o mmx_subtract_sc.o
  ASFLAGS = -g -Dpax=eax -Dpbx=ebx -Dpcx=ecx -Dpdx=edx -Dpsi=esi -Dpdi=edi
endif

ifeq ($(ARCH),i486)
  OBJS += mmx.o mmx_threshold.o mmx_despeckle.o mmx_update.o mmx_intensity_c.o mmx_intensity_s.o mmx_subtract_cs.o mmx_subtract_sc.o
  ASFLAGS = -g -Dpax=eax -Dpbx=ebx -Dpcx=ecx -Dpdx=edx -Dpsi=esi -Dpdi=edi
endif

ifneq (,$(findstring dc1394,$(LDFLAGS)))
  OBJS += DCImageSource.o
endif

ifneq (,$(findstring freenect,$(LDFLAGS)))
  SLFLAGS += -lusb-1.0
  OBJS += KinectImageSource.o
endif

ifneq ($(OS),iPhone)
  OBJS += PicoPNG.o
else
  OBJS += PicoPNG.oo
endif

ifeq ($(OS),linux)
  OBJS += V4LImageSource.o 
endif

include $(TOPDIR)/scripts/rules.mk

cvdemo.prof: cvdemo.o $(OBJS)
	g++ $(CFLAGS) $^ -o $@ ../simplecv/*.o ../tools/*.o

