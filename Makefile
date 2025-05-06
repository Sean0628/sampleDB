CXX = g++
CXXFLAGS = -std=c++17 -I./src -I./test
SRC = $(wildcard src/app/*.cpp src/file/*.cpp)
TESTS = $(wildcard test/*.cpp)
OUT = build/test_runner

all: $(OUT)

$(OUT): $(SRC) $(TESTS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) $(TESTS) -o $(OUT)

run: $(OUT)
	./$(OUT)

clean:
	rm -rf build
