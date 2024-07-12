## How to run

### Compile module file

```shell
ldu --filetype=obj module2/gcd.mod

# print llvm IR
ldu --filetype=asm --emit-llvm -o - module2/gcd.mod
```
