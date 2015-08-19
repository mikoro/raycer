rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

UNAME := $(shell uname -s | tr "[:upper:]" "[:lower:]")
SOURCES := $(call rwildcard, src/, *.cpp)
OBJS := $(subst src/,build/,$(SOURCES:.cpp=.o))
CFLAGS = -isystem include -Isrc -std=c++11 -Wpedantic -Wall -Wextra -Werror -Ofast -fopenmp
LDFLAGS = 
TARGET = raycer

# linux
ifneq "$(findstring linux,$(UNAME))" ""
	LDFLAGS += -Lplatform/linux/lib -lstdc++ -ldl -lm -lpthread -lglfw -lGL -lOpenCL -lfreetype-gl -lfreetype -lboost_system -lboost_filesystem -lXrandr -lXi -lXcursor -lXinerama
endif

# mac
ifneq "$(findstring darwin,$(UNAME))" ""
	CFLAGS += -isystem /opt/local/include -Wno-deprecated-declarations
	LDFLAGS += -L/opt/local/lib -Lplatform/mac/lib -framework Cocoa -framework OpenGL -framework OpenCL -framework IOKit -framework CoreVideo -lstdc++ -lglfw -lboost_system-mt -lfreetype -lfreetype-gl
endif

# travis-ci environment
ifeq "$(TRAVIS)" "true"
	CFLAGS += -isystem /var/tmp/travis/include -DRUN_UNIT_TESTS -D__extern_always_inline=inline
	LDFLAGS += -L/var/tmp/travis/lib
endif

default: raycer

raycer: $(OBJS)
	@mkdir -p bin
	@echo "Linking $@"
	@$(CXX) $(OBJS) $(CFLAGS) $(LDFLAGS) -o bin/$(TARGET)
	@platform/linux/post-build.sh

build/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo "Compiling $<"
	@$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf bin build
