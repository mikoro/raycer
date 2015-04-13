rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard, src/, *.cpp)
OBJS := $(subst src/,build/,$(SOURCES:.cpp=.o))
CC = g++
CFLAGS = -isystem include -Isrc -std=c++11 -fopenmp -Wall -Wextra -Werror -O3
LDFLAGS = -Lplatform/linux/lib -lGL -lGLEW -lglfw -lfreetype -lfreetype-gl -lOpenCL -lboost_system -fopenmp
TARGET = raycer

default: raycer

raycer: $(OBJS)
	@mkdir -p bin
	@echo "Linking $@"
	@$(CC) $(OBJS) $(LDFLAGS) -o bin/$(TARGET)
	@platform/linux/post-build.sh

build/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf bin build
