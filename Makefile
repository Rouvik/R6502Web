.PHONY: all mkdirs clean

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

$(BUILD)/%.o: $(SRC)/%.cpp $(INCLUDE)/%.hpp
	$(CXX) -c -o $@ $< $(FLAGS)

mkdirs:
	mkdir -p $(BUILD)

clean:
	rm -r $(BUILD)