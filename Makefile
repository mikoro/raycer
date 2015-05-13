rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard, src/, *.cpp)
OBJS := $(subst src/,build/,$(SOURCES:.cpp=.o))
CFLAGS = -isystem /var/tmp/travis/include -isystem include -Isrc -std=c++11 -Wall -Wextra -O3 -fopenmp
LDFLAGS = -L/var/tmp/travis/lib -Lplatform/linux/lib -lGL -lGLEW -lglfw3 -lfreetype-gl -lfreetype -lOpenCL -lboost_system -lm -lstdc++ -lXrandr -lXi -lXcursor -lXinerama -fopenmp
TARGET = raycer

default: raycer

raycer: $(OBJS)
	@mkdir -p bin
	@echo "Linking $@"
	@$(CXX) $(OBJS) $(LDFLAGS) -o bin/$(TARGET)
	@platform/linux/post-build.sh

build/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo "Compiling $<"
	@$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf bin build
