rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard, src/, *.cpp)
OBJS := $(subst src/,build/,$(SOURCES:.cpp=.o))
CFLAGS = -isystem /var/tmp/travis/include -isystem /u/78/moronkai/unix/usr/include -isystem include -Isrc -std=c++11 -Wpedantic -Wall -Wextra -Werror -Ofast
LDFLAGS = -L/var/tmp/travis/lib -L/u/78/moronkai/unix/usr/lib -Lplatform/linux/lib -lGL -lGLEW -lglfw3 -lfreetype-gl -lfreetype -lOpenCL -lboost_system -lm -lstdc++ -lXrandr -lXi -lXcursor -lXinerama
TARGET = raycer

ifeq "$(TRAVIS)" "true"
	CFLAGS += -DRUN_UNIT_TESTS
endif

ifeq "$(findstring clang,$(CXX))" ""
	CFLAGS += -fopenmp -flto
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

