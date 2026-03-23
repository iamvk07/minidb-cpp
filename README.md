# MiniDB — Lightweight In-Memory Database Engine (C++)

A lightweight in-memory database engine built in C++ supporting SQL-like operations: INSERT, SELECT, UPDATE, DELETE, COUNT, ORDER BY, LIMIT, and CSV export.

## Features

- **INSERT** rows with field-value pairs
- **SELECT** with WHERE predicates, ORDER BY, and LIMIT
- **UPDATE** matching rows with new values
- **DELETE** rows matching a condition
- **COUNT** rows with optional filter
- **CSV Export** — dump any table to a CSV file
- **Pretty printing** — auto-sized column tables in terminal
- **Multiple tables** per database instance
- **Numeric & string sorting** — auto-detects column type for ORDER BY

## Build & Run

```bash
# Compile
g++ -std=c++17 -O2 main.cpp -o minidb

# Run demo
./minidb
```

**Requires:** C++17 or later (`g++`, `clang++`, or MSVC)

## Sample Output

```
  ╔══════════════════════════════════╗
  ║   MiniDB — In-Memory Database    ║
  ║   by Vedant Kadam                ║
  ╚══════════════════════════════════╝

  Table 'students' created.
  INSERT 6 students...

  SELECT * FROM students:
  +----+----------------+------------------+-----+------+
  | id | name           | major            | gpa | year |
  +----+----------------+------------------+-----+------+
  | 1  | Alice Chen     | Computer Science | 3.9 | 3    |
  | 2  | Bob Smith      | Mathematics      | 3.4 | 2    |
  ...

  SELECT * FROM students ORDER BY gpa DESC LIMIT 3:
  +----+------------+------------------+-----+------+
  | id | name       | major            | gpa | year |
  | 1  | Alice Chen | Computer Science | 3.9 | 3    |
  | 5  | Eve Johnson| Computer Science | 3.8 | 2    |
  | 3  | Carol Davis| Computer Science | 3.7 | 4    |
```

## Architecture

```
Database
└── Table (name, columns[])
    └── Row (id, fields{key→value})
```

Key C++ concepts demonstrated:
- Templates (`Stack<T>`, `Queue<T>`)
- `std::function` for predicate callbacks
- `std::unordered_map` for O(1) field lookup
- Move semantics and RAII
- Lambda expressions
- File I/O with `std::ofstream`
- String parsing and type coercion

## Author

**Vedant Kadam** · UNB Computer Science
[github.com/iamvk07](https://github.com/iamvk07)
