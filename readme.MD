I start to play with LLVM a little.

Here is the experiment that I've done for LLVM

1. test folder contains all the code to support IR generation with LLVM.

2. optimization folder contains the code for generated IR.

The way to run a IR file is:
lli [IR file name]

The way to optimize a file is:
opt -S [option] [IR file name] -o [optimized IR file]
