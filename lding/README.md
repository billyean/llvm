# lding is a experiment simple compiler for arithmetic

## How to  run

assume your arithmetic expression is a + b + 3.(Two variables a, b)

```
# compile your expression into LLVM IR. 
lding  "with a, b: a * b + 3" | llc -filetype=obj -relocation-model=pic -o=expr.o
# link with predefined rtcalc.c
clang -o expr expr.o rtcalc.c
```
