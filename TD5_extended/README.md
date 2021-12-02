# Low-level language compiler to Turing machines

TD5_extended is a low-level language borrowing from C++ and Rust that can be compiled down to Turing machines, for use with [this simulator](https://github.com/zdimension/Automatic-Machines-Simulator).

- [The language](#the-language)
  - [Data types](#data-types)
  - [Statements](#statements)
  - [Expressions](#expressions)
  - [Pattern matching](#pattern-matching)
  - [Structures and implementations](#structures-and-implementations)
    - [Operator overloading](#operator-overloading)
  - [Functions](#functions)

## The language

### Data types

- `void`: void type, used for return type
- `uN`: unsigned integer on N bits
    - `bool`: alias for `u1`
- `TYPE*`: pointer to `TYPE`
- `TYPE global`: specify that the pointer type `TYPE` points to a global variable
- `TYPE const`: specify that `TYPE` is constant
- `struct { NAME: TYPE; NAME: TYPE; }`: structure type, can be empty
- `typeof(EXPR)`: type of expression
- `scalarof(EXPR)`: generates the scalar type with the specified size (`EXPR` must be a compile-time constant)
- `self`: in a type declaration implementation, refers to the type itself
- `NAME`: other user-defined type

### Statements

- `var DECL, DECL, ...;`, with `DECL` being one of:
    - `NAME: TYPE = EXPR`: you can omit the type specification or the initial value, but not both
    - `NAME[EXPR] = STRING`: if STRING is specified, array will be initialized and EXPR can be omitted (defaults to the length of STRING), otherwise zero-filled.
- `(NAME, NAME, ...) = (EXPR, EXPR, ...);`: tuple assignment; right-hand side is evaluated completely before assignment
  - useful for swapping two variables: `(a, b) = (b, a);`
- `type DECL, DECL, ...;`, with DECL being one of:
  - `NAME = TYPE`
  - `NAME<T1, T2, ...> where(...) = TYPE`: `where` is optional
- `const NAME = EXPR;`: `EXPR` must be a compile-time constant
- `print EXPR;`: pushes a value on the stack
- `if (EXPR) STMT else STMT`: `else` can be omitted
- `for (EXPR or DECL; EXPR; EXPR)`: all three components can be omitted; if the condition is omitted it is equivalent to `true`
- `do STMT while (EXPR)`
- `while (EXPR) STMT`
- `loop STMT;`: `loop` is equivalent to `while (true)`
- `return EXPR;`: `EXPR` only if the surrounding function's return type is not `void`
- `break EXPR;`: `EXPR` only if the surround loop is an expression
- `continue;`
- `assert EXPR;`: checks at compile time that `EXPR` is true
- `impl NAME { FUNC FUNC ... }`: declare methods for the specified type; type must be a struct
- `func NAME<T1, T2, ...>(NAME: TYPE, ...): TYPE where(...) { ... }`: generic types optional; `where` allowed only if function is generic

### Expressions

- precedence 0
  - `NUMBER`: integer literal, if size not specified will choose the smallest fitting
  - `sizeof(TYPE)`: size of type in memory cells (returns `u8`)
  - `bitsof(TYPE)`: size of type in bits (returns `u8`)
  - `new(TYPE)`: dynamically allocates an instance of `TYPE` (returns `TYPE*`)
  - `(EXPR)`: parenthesized expression
  - `{ STMT; STMT; ... EXPR }`: statement expression
  - `if (EXPR) EXPR else EXPRR`: inline if
  - `loop { ... }`: inline loop, must loop infinitely or yield a value using `break EXPR`
  - `match (EXPR) { PATTERN => EXPR, ... }`: [pattern matching](#pattern-matching)
  - `TYPE { NAME: EXPR, ... }`: structure literal (returns `TYPE`); field names can be omitted and deduced automatically
- precedence 1
  - `EXPR++`, `EXPR--`: postfix increment and decrement
  - `EXPR1[EXPR2]`: equivalent to `*(EXPR1 + EXPR2)`
  - `EXPR.NAME`: member access
  - `EXPR(EXPR, ...)`: function call
- precedence 2
  - `-EXPR`: negation
  - `~EXPR`: bitwise complement
  - `*EXPR`: dereference, `EXPR` must be of pointer type
  - `&EXPR`: address of
  - `++EXPR`, `--EXPR`: prefix increment and decrement
- precedence 3:
  - `EXPR * EXPR`: multiplication
  - `EXPR / EXPR`: division
- precedence 4:
  - `EXPR + EXPR`: addition
  - `EXPR - EXPR`: subtraction
- precedence 5:
  - `EXPR << EXPR`: left shift
  - `EXPR >> EXPR`: right shift
- precedence 6:
  - `EXPR < EXPR`: less than
  - `EXPR > EXPR`: greater than
  - `EXPR <= EXPR`: less than or equal to
  - `EXPR >= EXPR`: greater than or equal to
- precedence 7:
  - `EXPR == EXPR`: equality
  - `EXPR != EXPR`: inequality
- precedence 8:
  - `EXPR && EXPR`: logical and
- precedence 9:
  - `EXPR || EXPR`: logical or
- precedence 10:
  - `EXPR1 |> EXPR2`: pipeline; equivalent to `EXPR2(EXPR1)`
- precedence 11:
  - `EXPR is PATTERN`: [pattern matching](#pattern-matching)
- precedence 12:
  - `EXPR = EXPR`: assignment
  - `EXPR += EXPR`: addition assignment
  - `EXPR -= EXPR`: subtraction assignment
  - `EXPR *= EXPR`: multiplication assignment
  - `EXPR /= EXPR`: division assignment

### Pattern matching

| Description      | Syntax                              | Equivalent                                          |
|------------------|-------------------------------------|-----------------------------------------------------|
| fixed expression | `EXPR`                              | `x == EXPR`                                         |
| range, exclusive | `EXPR..EXPR`                        | `x >= EXPR && x < EXPR`                             |
| range, inclusive | `EXPR..=EXPR`                       | `x >= EXPR && x <= EXPR`                            |
| union            | <code>PATTERN &#124; PATTERN</code> | <code>x is PATTERN &#124;&#124; x is PATTERN</code> |
| wildcard         | `_`                                 | `true`                                              |

### Structure types

A structure type can contain methods, which can be called either on an instance or on the type itself.

An instance method must have at least one parameter, the first corresponding to the instance. If the first parameter is a pointer, then the instance will automatically be passed by reference rather than by value.

**Note:** any method can be called on the type or on an instance, there is no technical difference between a "static" method and an "instance" method.

#### Operator overloading

If a function's name is an operator, then using that operator on an instance of the type will call that method.

This allows for constructions such as:
```rust
type point = struct { x: u16; y: u16; };
impl point
{
    func `+`(self: point, other: point): point
    {
        return point { x: self.x + other.x, y: self.y + other.y };
    }
}

var p = point { x: 1, y: 2 };
var q = point { x: 3, y: 4 };
var r = p + q; // r.x == 4, r.y == 6
```

These three lines are equivalent:
```rust
var r = p + q;
var r = p.`+`(q);
var r = point.`+`(p, q);
```

Any operator can be overloaded, including `(`:
```rust
type counter = struct { value: u32; };
impl counter
{
    func `(`(this: counter*): u32
    {
        return ++self.value;
    }
}

var cnt: counter;
print cnt();
```

These three lines are equivalent:
```rust
print cnt();
print cnt.`(`();
print counter.`(`(&cnt);
```

#### Generic types

A structure type can have generic type parameters, which allow for a decrease in code duplication.

```rust
type vec<T> = struct { x: T; y: T; };
impl vec
{
    func normsq(this: self): T
    {
        return this.x * this.x + this.y * this.y;
    }
}

type vec16 = vec<u16>; // declare a type alias

var p = vec<u8> { x: 1, y: 2 }; // struct literal

var q = p.normsq(); // q is u8
```

### Functions

A function takes a fixed number of parameters and can optionally return a value.

Like a type, a function can be generic. This is often useful to simulate C macros in a type-safe manner.

A generic function will be compiled as a generic type containing an overloaded `(`-operator:
```rust
func add<T>(x: T, y: T): T
{
    return x + y;
}
```
is compiled as:
```rust
type add<T> = struct { };
impl add
{
    func `(`(x: T, y: T): T
    {
        return x + y;
    }
}
```