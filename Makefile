H_FILES = src/cryptogram.h

CPP_FILES = src/cryptogram.cpp \
			src/cryptoslam.cpp

O_FILES = $(addprefix build/,$(notdir $(CPP_FILES:.cpp=.o)))

CFLAGS = -g -Wall
LIB_FLAGS = -g -lcurses -lstdc++

all: cryptoslam

cryptoslam: build/cryptoslam

build/cryptoslam: $(O_FILES)
	mkdir -p build
	gcc -o build/cryptoslam $(LIB_FLAGS) $(O_FILES)

clean:
	rm -rf build

build/%.o: src/%.cpp
	mkdir -p build
	gcc $(CFLAGS) -c $< -o $@


