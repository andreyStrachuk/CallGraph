CXX_COMPILER = clang++
CXXPASSFLAGS = -Xclang -load -Xclang ../build/callgraph/libCallGraphPass.so -c

CXXFLAGS = -c

LIBSTRINGPARSER = ../lib/libstringparser.a
SRC_OBJ = List.o main.o getInfo.o $(LIBSTRINGPARSER)

ELF_NAME = test
LINK = linking

all: $(SRC_OBJ) $(LINK)

main.o: main.cpp
	$(CXX_COMPILER) $(CXXPASSFLAGS) main.cpp

List.o: List.cpp
	$(CXX_COMPILER) $(CXXPASSFLAGS) List.cpp

getInfo.o : ../lib/getInfo.cpp
	$(CXX_COMPILER) $(CXXFLAGS) ../lib/getInfo.cpp

$(LINK):
	$(CXX_COMPILER) $(SRC_OBJ) -o $(ELF_NAME)

clean:
	rm *.o
