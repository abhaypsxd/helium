# Helium

**Helium** is a small, expression-based programming language built from scratch as a learning project. It supports arithmetic expressions, variables, operator precedence, and basic `exit()` statements.

This project is not intended for general use. It's a personal educational playground to learn the internals of compilers, parsers, and interpreters.

---

## ✨ Features

- Integer literals and identifiers
- `let` bindings (variable declaration)
- Arithmetic expressions with:
  - Addition (`+`)
  - Subtraction (`-`)
  - Multiplication (`*`)
  - Division (`/`)
  - Modulus (`%`)
- Parentheses for grouping
- `exit(expr);` to terminate and evaluate an expression
- Operator precedence and associativity handling

---

## 🧾 Example Code

```c
let x = 8;
let y = 7;
let z = x % y + 2 * (x - 3);
exit(z);
```

---

## 📜 Grammar (EBNF Style)

```ebnf
program     ::= { statement }

statement   ::= "let" IDENT "=" expression ";" 
             | "exit" "(" expression ")" ";"

expression  ::= term { binary_op term }

term        ::= INT_LIT 
             | IDENT 
             | "(" expression ")"

binary_op   ::= "+" | "-" | "*" | "/" | "%"

IDENT       ::= [a-zA-Z_][a-zA-Z0-9_]*
INT_LIT     ::= [0-9]+
```

---

## 🔧 Architecture

- **Lexer**: Converts source code into a list of tokens (not shown here)
- **Parser**: Builds an Abstract Syntax Tree (AST) using:
  - Operator precedence climbing
  - Arena allocation for fast memory management
- **AST Nodes**:
  - `Expr`, `Term`, `BinExpr*`, `StmtLet`, `StmtExit`, etc.
- **Interpreter**: Coming soon — will evaluate the AST and return values.

---

## 📂 File Structure

```
├── tokenization.hpp    # Token types and lexer definitions
├── arena.hpp           # Simple arena allocator
├── parser.hpp          # Parser logic and AST construction
├── main.cpp            # (Optional) Entry point for parsing & running code
```

---

## 🔮 Next Steps

- [x] Add parentheses support in expressions
- [ ] Add evaluation logic for expressions and statements
- [ ] Implement variable environment
- [ ] Support comparisons (`<`, `>`, `==`, etc.)
- [ ] Add `if` / `else` control flow
- [ ] Experiment with functions and scopes

---

## 🧑‍💻 Author

This project is developed and maintained as a hobby learning exercise.

If you're reading this and want to build a language too — go for it. It’s fun, and you’ll learn a lot!
