EXENAME := xbsak_gem

CXX_EXT := cpp
CL_EXT := cl

AR := ar
CXX := xcpp

CXXFLAGS := -Wall -Werror -std=c++11 -pthread

GEM_PATH= ../../software/userspace/sdaccel/driver/xclgemhal/driver/xclng/user_gem

HAL_LIB_PATH = ../../software/userspace/sdaccel/lib/runtime/platforms/hal/driver

HAL_INC := -I${GEM_PATH}/../../../ -I${GEM_PATH}/../../include

CXXFLAGS += $(HAL_INC) -I${GEM_PATH}

HAL_LIBNAME := ${HAL_LIB_PATH}/libxclgemdrv.a

ifeq ($(debug),1)
	CXXFLAGS += -g -D_DEBUG
else
	CXXFLAGS += -O2 -DNDEBUG
endif

SRCS := $(wildcard *.$(CXX_EXT))
OBJS := $(patsubst %.$(CXX_EXT), %.o, $(SRCS))

-include $(OBJS:.o=.d)

LDFLAGS := -static-libstdc++ -pthread -ldl

#$(error $(ODIR))
#$(error $(DIR))
#$(error $(ROOT))

all : $(EXENAME)

%.o: %.$(CXX_EXT)
	$(CXX) $(CXXFLAGS) $(MYCFLAGS) $(MYCXXFLAGS) -c $< -o $@
	$(CXX) $(CXXFLAGS) $(MYCFLAGS) $(MYCXXFLAGS) -c -MM $< -o $(patsubst %.o, %.d, $@)

$(EXENAME): $(OBJS) $(HAL_LIBNAME)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(HAL_LIBNAME) -lrt

clean:
	rm -rf *.o *.d $(EXENAME)

.PHONY: all

.DEFAULT_GOAL := all
