MKDIR_P = mkdir -p
OUTPUT := bin

all : simple memory

output_dir:
	${MKDIR_P} ${OUTPUT}

simple : output_dir simple.cpp
	clang++ -O3 simple.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o ${OUTPUT}/simple

memory : output_dir memory.cpp
	clang++ -O3 memory.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o ${OUTPUT}/memory

clean :
	$(RM) bin/*
