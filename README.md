I start to play with LLVM a little.

Here is the experiment that I've done for LLVM

1. test folder contains all the code to support IR generation with LLVM.

2. optimization folder contains the code for generated IR.
The way to run a IR file is:
lli [IR file name]

The way to optimize a file is:
opt -S [option] [IR file name] -o [optimized IR file]
opt -passname [IR file name] -o [optimized IR file]
option can be below:
- adce: Aggressive Dead Code Elimination
- bb-vectorize: Basic-Block Vectorization
- constprop: Simple constant propagation
- dce: Dead Code Elimination
- deadargelim: Dead Argument Elimination
- globaldce: Dead Global Elimination
- globalopt: Global Variable Optimizer
- gvn: Global Value Numbering
- inline: Function Integration/Inlining
- instcombine: Combine redundant instructions
- licm: Loop Invariant Code Motion
- loop: unswitch: Unswitch loops
- loweratomic: Lower atomic intrinsics to non-atomic form
- lowerinvoke: Lower invokes to calls, for unwindless code generators
- lowerswitch: Lower SwitchInsts to branches
- mem2reg: Promote Memory to Register
- memcpyopt: MemCpy Optimization
- simplifycfg: Simplify the CFG
- sink: Code sinking
- tailcallelim: Tail Call Elimination

The way to get IR file from C source file is:
clang -emit-llvm -S [c source file] -o [IR file name]
clang -cc1 -emit-llvm [c source file] -o [IR file name]

The way to convert IR file to bitcode file is:
llvm-as [IR file] –o [bitcode file]

The way convert LLVM bitcode to assembly code is:
llc [bitecode file] -o [assembly code]
clang -S [bitcode file] -o [assembly code] -fomit-frame-pointer

The way convert LLVM bitcode back to LLVM IR is:
llvm-dis [bitcode file] -o [IR file]

The way link LLVM bitcode is:
llvm-link [bitcode file...] -o [bitcode output]

3. Other features
3-1. llgo:

3-2. DragonEgg
