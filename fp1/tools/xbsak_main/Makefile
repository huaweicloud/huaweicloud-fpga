EXENAME := xbsak_ng

CXX_EXT := cpp
CXX := xcpp
CL_EXT := cl

AR := ar
CPP := g++

CXXFLAGS := -Wall -Werror -std=c++0x 

ROOT1 = $(dir $(dir $(CURDIR)))
ROOT = $(ROOT1)/..
HAL_PATH = $(CURDIR)/../../software/userspace/sdaccel/driver

HAL_INC := -I$(ROOT) -I./ -I$(HAL_PATH)/xclhal/include/hal

CXXFLAGS += $(HAL_INC)
CXXFLAGS += -L/usr/lib64 -lfpgamgmt
CXXFLAGS += -L/usr/lib64 -lxclngdrv

ifeq ($(debug),1)
	CXXFLAGS += -g -D_DEBUG
else
	CXXFLAGS += -O2 -DNDEBUG
endif

SRCS := $(wildcard *.$(CXX_EXT))
OBJS := $(patsubst %.$(CXX_EXT), %.o, $(SRCS))

-include $(OBJS:.o=.d)

#LDFLAGS := -static-libstdc++  
#LDFLAGS :=  

#$(error $(ODIR))
#$(error $(DIR))
#$(error $(ROOT))

all : $(EXENAME)

%.o: %.$(CXX_EXT)
	ln -fs $(HAL_PATH)/../lib/runtime/platforms/hal/driver/libxclngdrv.so /usr/lib64/libxclngdrv.so
	$(CXX) $(CXXFLAGS) $(MYCFLAGS) $(MYCXXFLAGS) -c $< -o $@
	$(CXX) $(CXXFLAGS) $(MYCFLAGS) $(MYCXXFLAGS) -c -MM $< -o $(patsubst %.o, %.d, $@)

$(EXENAME): $(OBJS)
	$(CXX) -o $@ $(OBJS) -lfpgamgmt -lxclngdrv -lrt

clean:
	rm -rf *.o *.d $(EXENAME)

.PHONY: all

.DEFAULT_GOAL := all
