CXX = g++ # C compiler
CXXFLAGS = -fPIC -Wall -Wextra -O2 -g -m32 ${INCLUDE_PATH} # CXX flags
LDFLAGS = -shared -m32 ${LIB_PATH} ${LIBS} # linking flags
RM = rm -f  # rm command
TARGET_LIB = libDhsTranslator.so # target lib
LIB_PATH = -L${DHS_CLIENT}/lib -L${AXIS2C_HOME}/lib/
LIBS = -ldhs -lgen -lrt -lpthread -lboost_system-mt
INCLUDE_PATH = -I${DHS_CLIENT}/include/  -I${AXIS2C_HOME}/include/axis2-1.6.0/

GCCMAYORVERSION := $(shell gcc -dumpversion | cut -f1 -d.)

ifeq "$(GCCMAYORVERSION)" "4"
CXXFLAGS += --no-strict-aliasing
endif
 
#SRCS = %.cpp # source files
SRCS = DhsAdapterSim.cpp DhsService.cpp service.cpp DhsAdapter.cpp DhsUtil.cpp MessageCache.cpp
OBJS = $(SRCS:.cpp=.o)
 
.PHONY: all
all: ${TARGET_LIB}
 
$(TARGET_LIB): $(OBJS)
	$(CXX) ${LDFLAGS} -o $@ $^
 
$(SRCS:.cpp=.d):%.d:%.cpp
	$(CXX) $(CXXFLAGS) -MM $< >$@
 
include $(SRCS:.cpp=.d)
 
.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.cpp=.d)
 