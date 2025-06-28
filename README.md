# Helium: A Minimal Language

Helium is a simple expression-based, stack-machine-oriented language designed to help understand the core principles of compilation. It supports basic arithmetic operations, variable declarations, and the ability to exit with an expression-based return code.

## Features

- Arithmetic expressions with proper precedence and associativity
- Variable declarations and usage
- `exit()` for terminating the program with an integer value
- Parentheses for grouping expressions
- Generates x86-64 NASM assembly

## Example

```helium
let x = 8;
let y = 3;
let z = x % (y + 1) * 2;
print(x + y - z);
exit(z);
```

## Grammar

```ebnf
program     ::= { statement }
statement   ::= "let" IDENT "=" expression ";" 
             |  "exit" "(" expression ")" ";"
             |  "print" "(" expression ")" ";"
expression  ::= term { ( "+" | "-" ) term }
term        ::= factor { ( "*" | "/" | "%" ) factor }
factor      ::= INT | IDENT | "(" expression ")"
```

## Build Instructions

Build Helium using CMake. Ensure you have CMake installed, then run the following command in the terminal:
```sh
cmake --build/build
```
Compile the source code with:
```sh
./build/helium ./test.he
```
Run the generated assembly code with:
```sh
./out
```
View the exit code with:
```sh
echo $?
```


## Overview
This assumes the use of a `Makefile` that automates the following steps:

1. Tokenize, parse and compile the `.he` source file into `out.asm`
2. Assemble the output to `out.o` using NASM:
   ```bash
   nasm -felf64 out.asm
   ```
3. Link the object file with `ld` to create the final executable:
   ```bash
   ld -o out out.o
   ```
4. Run the resulting binary:
   ```bash
   ./out
   ```

## File Structure

- `main.cpp`: Entry point. Handles file I/O, tokenization, parsing, codegen and compilation.
- `tokenizer.hpp`: Converts input source code into tokens.
- `parser.hpp`: Parses tokens into an abstract syntax tree (AST).
- `arena.hpp`: Simple arena allocator to avoid heap fragmentation during AST construction.
- `generation.hpp`: Code generator that outputs x86-64 assembly from AST.
- `out.asm`: Generated NASM assembly.
- `out`: Final compiled binary.

## Requirements

- C++17 or newer
- NASM (Netwide Assembler)
- GNU `ld` or compatible linker
- A Unix-like system (Linux/macOS/WSL)
