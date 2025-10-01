.PHONY: all mkdirs clean objBuild compileASM6

CXX := em++

SRC = ./src
BUILD = ./bin
INCLUDE = ./include

ASM6DIR = ./asm6

TARGETS = $(wildcard $(SRC)/*.cpp)
OBJECTS = $(patsubst $(SRC)/%.cpp, $(BUILD)/%.o, $(TARGETS))

CXXFLAGS = -Wall -Wextra -I$(INCLUDE)

EMCXXFLAGS = --bind -s EXPORTED_RUNTIME_METHODS='["HEAPU8"]'

# DEBUG = -g4

all: mkdirs r6502Main.cpp compileASM6 $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(EMCXXFLAGS) $(DEBUG) -o $(BUILD)/r6502.mjs r6502Main.cpp $(OBJECTS)

# Only build the required object files and not the driver executable
objBuild: mkdirs $(OBJECTS)

$(BUILD)/%.o: $(SRC)/%.cpp $(INCLUDE)/%.hpp
	$(CXX) -c $(CXXFLAGS) $(DEBUG) -o $@ $<

compileASM6: $(ASM6DIR)/asm6.c
	cd $(ASM6DIR) && ./compileAsm6.sh

mkdirs:
	mkdir -p $(BUILD)

clean:
	rm -r $(BUILD)