.PHONY: all mkdirs clean objBuild

CXX := g++

SRC = ./src
BUILD = ./bin
INCLUDE = ./include

TARGETS = $(wildcard $(SRC)/*.cpp)
OUTPUTS = $(patsubst $(SRC)/%.cpp, $(BUILD)/%.o, $(TARGETS))

FLAGS = -Wall -Wextra -I$(INCLUDE)

# DEBUG = -ggdb

all: mkdirs main.cpp $(OUTPUTS)
	$(CXX) -o $(BUILD)/main.exe main.cpp $(OUTPUTS) $(FLAGS) $(DEBUG)

# Only build the required object files and not the driver executable
objBuild: mkdirs $(OUTPUTS)

$(BUILD)/%.o: $(SRC)/%.cpp $(INCLUDE)/%.hpp
	$(CXX) -c -o $@ $< $(FLAGS)

mkdirs:
	mkdir -p $(BUILD)

clean:
	rm -r $(BUILD)