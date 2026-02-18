# Mini-KVS (Key-Value Store)

**Mini-KVS is a lightweight, persistent Key-Value Store written entirely in C.**

## Educational Project
🎓 This project was developed as a hands-on exercise to master low-level C concepts. It focuses on manual memory management, pointer arithmetic, generic data structures, binary file I/O, and modular compilation. It is not intended for production use but rather as a demonstration of "under the hood" system programming.

## 🚀 Features
- In-Memory Storage: Uses a Hash Map with chaining to handle collisions.

- Persistence: Saves and loads the state of the database from a binary file (.db) to ensure data survives after the program exits.

- Custom Error Stack: Implements a robust, stack-based error handling system to trace issues back to their origin function.

- REPL Interface: A command-line interface to interact with the database in real-time.

- Modular Architecture: Clean separation of concerns (Input, Error, Hashing, Persistence).

## 📏 Conventions
To ensure code robustness and maintainability, the following rules must be followed:

### Memory Management

- Every structure's string attributes must be malloc in the allocation function and not be the pointer provided to the function.
- If a structure have an attribute that need its own allocation or a linkage then it must have its own allocation and free function.

## 🛠 Architecture
### Data Structure

The core of the project is a Hash Map. To handle collisions (when two keys produce the same hash), it uses a doubly linked list (chaining).

- Entry: Represents a node containing the table name, key ID, value, and pointers to the next/previous nodes.

- HashMap: Contains the array of pointers to Entries.

- CachedHashMap: A wrapper structure designed to handle LRU (Least Recently Used) caching strategies.

### Persistence Layer

Data is serialized into a custom binary format to optimize storage size and read speed.

- Header: The file starts with metadata integers: Entry Size and Data Size (Capacity).

- Body: (In progress) Sequential storage of serialized entries.

## 📦 Installation & Usage
### Prerequisites

- GCC Compiler

- Make

### Compilation

The project uses a Makefile for automated compilation.

```Bash
# Compile the project
make

# Clean up build files
make clean
```

### Running the program

```Bash
./mini-kvs
```

### CLI Commands

Once inside the REPL (Read-Eval-Print Loop):

- Help: Type help to see available commands.

- Add Value: (Syntax in progress) add TABLE:KEY_INT:VALUE.

## 📂 Project Structure

- `main.c:` Entry point and main loop.

- `hashage.c/h:` Hash Map logic and data structures.

- `persistence.c/h:` Handles writing/reading the binary .db files.

- `error.c/h:` Custom error propagation system.

- `input.c/h:` Safe user input handling.

- `Makefile:` Build configuration.

## 🧠 Concepts Learned
Building this project helped explore:
- Serialization: Converting complex C structures into binary streams for file storage.

- Double Pointers: Using Error** to propagate error states up the stack.

- Memory Management: Heavy use of malloc, free, and tracking memory leaks.

- Header Files: Managing dependencies and preventing circular inclusions with Include Guards.

Author: Pollamothe