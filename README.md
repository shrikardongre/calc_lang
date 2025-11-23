A toy language to improve my compiler understanding.

Grammer : 
```
calc : ("with" ident ("," ident)* ":")? expr ;
expr : term (( "+" | "-" ) term)* ;
term : factor (( "*" | "/") factor)* ;
factor : ident | number | "(" expr ")" ;
ident : ([a-zAZ])+ ;
number : ([0-9])+ ;
```
```
./src/calc "with a: a*3" > out1.ll
cat out1.ll

; ModuleID = 'calc.expr'
source_filename = "calc.expr"

@a.str = private constant [2 x i8] c"a\00"

define i32 @main(i32 %0, ptr %1) {
entry:
  %2 = call i32 @calc_read(ptr @a.str)
  %3 = mul nsw i32 %2, 3
  call void @calc_write(i32 %3)
  ret i32 0
}

declare i32 @calc_read(ptr)

declare void @calc_write(i32)

```
Linking it with the runtime library gives : 

```
clang out1.ll ../rtcalc.c -o expr1_exec
warning: overriding the module target triple with x86_64-unknown-linux-gnu [-Woverride-module]
1 warning generated.

./expr1_exec 
Enter a value for a: 2
The result is: 6
```
  
