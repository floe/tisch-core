TOPDIR = ../../
CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/calibd/ -I$(TOPDIR)/libs/osc/
LDFLAGS += -ltools -L$(TOPDIR)/libs/tools/

#APPS = test
#CFLAGS += -I$(TOPDIR)/libs/simplegl/
#LDFLAGS += -lsimplecv -lglut -lsimplegl -L. -L$(TOPDIR)/libs/simplegl/

LIBS = libsimplecv.$(LSF)
INCS = *.h
OBJS = IntensityImage.o RGBImage.o YUV420Image.o YUV420SPImage.o YUYVImage.o Line.o Circle.o ShortImage.o ColorLUT.o Blob.o

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

ifeq ($(OS)-$(ABI),linux-gnu)
  OBJS += V4LImageSource.o 
endif

include $(TOPDIR)/scripts/rules.mk
