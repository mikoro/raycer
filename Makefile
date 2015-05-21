rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard, src/, *.cpp)
OBJS := $(subst src/,build/,$(SOURCES:.cpp=.o))
CFLAGS = -isystem include -Isrc -std=c++11 -Wpedantic -Wall -Wextra -Werror -Ofast -fopenmp
LDFLAGS = -Lplatform/linux/lib -lstdc++ -ldl -lm -lpthread -lglfw3 -lGL -lOpenCL -lfreetype-gl -lfreetype -lboost_system -lXrandr -lXi -lXcursor -lXinerama
TARGET = raycer

# travis-ci environment
ifeq "$(TRAVIS)" "true"
	CFLAGS += -isystem /var/tmp/travis/include -DRUN_UNIT_TESTS
	LDFLAGS += -L/var/tmp/travis/lib
endif

# gcc
ifeq "$(findstring clang,$(CXX))" ""
	CFLAGS += -flto
endif

# clang
ifneq "$(findstring clang,$(CXX))" ""
	CFLAGS += -D__extern_always_inline=inline
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
