

---

# Custom Scripting Language Compiler

## Overview
This project is a simple compiler written in C that processes a custom scripting language. It supports basic constructs such as variable assignments, print statements, and if-else conditionals. The compiler performs lexical analysis, parsing, abstract syntax tree (AST) construction, and code generation to produce an executable C program (`output.c`). The generated code can be compiled and run using GCC.

- **Input**: Source code in the custom scripting language.
- **Output**: A compiled C program (`output.c`) that can be executed.
- **Date**: March 05, 2025.

## Language Syntax

### Variable Assignment
- **Syntax**: `<identifier> = <expression>;`
- **Description**: Assigns the result of an expression to a variable (implicitly integer).
- **Example**:
  ```
  x = 5 + 3;
  ```

### Print Statement
- **Syntax**: `print(<expression>);` or `print();`
- **Description**: Prints an expression's value followed by a newline. An empty `print()` outputs just a newline.
- **Example**:
  ```
  print(42);
  print(x + 2);
  print();
  ```

### If-Else Statement
- **Syntax**:
  ```
  if (<condition>) {
      <statements>
  } else {
      <statements>
  }
  ```
- **Description**: Evaluates a condition and executes the corresponding block. The `else` clause is optional.
- **Example**:
  ```
  if (x == 8) {
      print(1);
  } else {
      print(0);
  }
  ```

### Expressions
- **Syntax**: `<value> <operator> <value>` or `<value>`
- **Description**: Supports arithmetic and basic comparison operations.
- **Operators**:
  - Arithmetic: `+`, `-`, `*`, `/`
  - Comparison: `==`, `<`, `>`
- **Values**: Integers (e.g., `42`) or identifiers (e.g., `x`).
- **Example**:
  ```
  5 + 3
  x * 2
  y == 10
  ```

### Comments
- Not supported in the current version.

## Features
- **Lexical Analysis**: Breaks source code into tokens (e.g., identifiers, numbers, keywords).
- **Parsing**: Builds an AST from tokens, supporting assignments, print statements, and if-else constructs.
- **Code Generation**: Produces a C program (`output.c`) that mirrors the input script's functionality.
- **Execution**: Automatically compiles and runs the generated code using GCC.

## Prerequisites
- **GCC**: Required to compile the compiler and the generated C code.
- **C Environment**: A system with a C compiler (e.g., Linux, macOS, or Windows with MinGW).

## Installation
1. Clone or download this repository:
   ```
   git clone <repository-url>
   cd <repository-directory>
   ```
2. Compile the compiler:
   ```
   gcc compiler.c -o compiler
   ```

## Usage
1. Run the compiler:
   ```
   ./compiler
   ```
2. Enter your source code, then press `Ctrl+D` (Unix) or `Ctrl+Z` (Windows) followed by Enter:
   ```
   x = 5 + 3;
   print(x);
   if (x == 8) {
       print(1);
   } else {
       print(0);
   }
   ```
3. The compiler will:
   - Generate `output.c`.
   - Compile it to an executable (`output`) using GCC.
   - Run the program and display the output.

### Example Output
```
Source Code:
x = 5 + 3;
print(x);
if (x == 8) {
    print(1);
} else {
    print(0);
}

C code generated in output.c
Compiling output.c...
Running the compiled program...
----- Program Output -----
8
1
-------------------------
```

## Project Structure
- **`compiler.c`**: The main source file containing the compiler implementation.
- **`output.c`**: Generated C code (created during execution).
- **`output`**: Compiled executable (created during execution).

## Limitations
- **Variable Types**: Only integers are supported.
- **Error Recovery**: Stops on the first error encountered.
- **Comparisons**: Only `==` is fully implemented; `<` and `>` are parsed but not fully utilized.
- **Scope**: No support for variable scoping beyond the main function.
- **Token Limit**: Maximum of 100 tokens.

## Future Enhancements
- Support additional data types (e.g., floats, strings).
- Implement full comparison operators (`<`, `>`, `!=`, etc.).
- Add loop constructs (e.g., `while`, `for`).
- Improve error recovery for continued parsing.
- Introduce variable scoping and declarations.

## Error Messages
- `"Token limit exceeded"`: Input exceeds 100 tokens.
- `"Unknown character: <char>"`: Unrecognized character in source code.
- `"Expected <token> after <context>"`: Syntax error with missing or unexpected tokens.
- `"Memory allocation failed"`: Out of memory during AST creation.

## Contributing
Feel free to submit issues or pull requests to enhance the compiler. Potential areas for contribution include:
- Adding new language features.
- Improving error handling.
- Optimizing the generated C code.

