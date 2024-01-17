#include "include/lango.h"


// clang++ -o lango `llvm-config --cxxflags --ldflags --system-libs --libs core` -I./include lango-llvm.cpp
int main() {
	Lango lango;
	lango.exec("my-prog.ll");
	return 0;
}