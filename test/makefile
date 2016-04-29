main : simple memory

simple : simple.cpp
	clang++ -O3 simple.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o simple

memory : memory.cpp
	clang++ -O3 memory.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o memory

clean :
	rm simple memory
