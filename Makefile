CXX = g++
CXXFLAGS = -std=c++17 -I./src -I./test
SRC = $(filter-out src/app/main.cpp, $(wildcard src/app/*.cpp src/file/*.cpp src/tx/*.cpp src/tx/LogRecord/*.cpp src/logging/*.cpp src/buffer/*.cpp src/record/*.cpp src/scan/*.cpp src/meta/*.cpp src/indexing/*.cpp src/plan/*.cpp src/parse/*.cpp src/interface/*.cpp src/materialize/*.cpp))
TESTS = $(wildcard test/*.cpp)
OUT = build/test_runner
CLI_OUT = build/sampledb

all: $(OUT)

$(OUT): $(SRC) $(TESTS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) $(TESTS) -o $(OUT)

cli: $(CLI_OUT)

$(CLI_OUT): $(SRC) src/app/main.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) src/app/main.cpp -o $(CLI_OUT)

run: $(OUT)
	./$(OUT)

clean:
	rm -rf build *Test
