# Ceme

This is an experimental scheme interpreter written in C for the lecture "Design und Implementierung moderner Programmiersprachen" at Media University Stuttgart.

# Features

* Interactive AST interpreter (REPL)
* Closures and lexical scopes via inner defines/lambdas
* Tail-call optimization through Continuation Passing via trampoline function
* Detection if input expression is complete (REPL indicates pending if not)

# Requirements

* C compiler (`clang`/`gcc`). Specify accordingly by setting the CC env variable.
* `make`

# Getting started

1. Install the above requirements
2. Clone this repository
3. Build
  - `make` for the regular build
  - `make debug` for the debug build
4. Use
  - run `./scheme` to start the REPL

# Ingredients

## Basic Data Types

* Numbers (no Floats at the moment)
* Strings
* Symbols
* Booleans
* Lists (cons)
* Functions
* Nil and Void

## Builtin Syntax

### Own functions and variables

__define__

__`(define sym expr)`__

Define a variable:

```
>  (define foo "bar")
>  foo
"bar"
```

Define a function:

```
>  (define (add a b) (+ a b))
>  add
<procedure: add>
>  (add 1 2)
3
```

__lambda__

__`(lambda [(args) arg] expr ...)`__

Bind an anonymous lambda to a variable:

```
>  (define mylambda (lambda (a) a))
>  mylambda
<procedure: anonymous lambda>
>  (mylambda "hello")
"hello"
```

Bind varargs to a symbol:

```
>  ((lambda a a) 1 2 3 4)
(1 2 3 4)
```

### Control statements

__`(if expr expr expr)`__

```
>  (if #t "t" "f")
#t
```

### Environment mutation

__`(set! symbol expr)`__

Set a variable to a new value

```
>  (define foo "foo")
>  foo
"foo"
>  (set! foo "bar")
>  foo
"bar"
```

### Quoting input

__`(quote expr)`__ or shorter: __`'expr`__

Returns the unevaluated expr. Good for quickly creating lists.

```
>  (quote (1 2 3))
(1 2 3)
>  'foo
foo
```

## Builtin Functions

### Arithmetic Functions (only for Numbers)

__`(+ expr ...)`__

```
>  (+ 1 2 3)
6
```

__`(- expr ...)`__

```
>  (- 1 2 3)
-4
```

__`(* expr ...)`__

```
>  (* (+ 1 2 3) (- 1 2 3))
-24
```

__`(/ expr ...)`__

```
>  (/ 8 2)
4
```

### Pair Functions

__Pair creation__

__`(cons expr expr)`__

Create a well-formed list:

```
>  (cons 1 (cons 2 nil))
(1 2)

// or via quote
>  '(1 2)
(1 2)
```

Create a dotted pair:

```
>  (cons 1 2)
(1 . 2)
```

<br>

__Pair retrieval__

__`(car expr)`__

Get the left part of a pair:

```
>  (car '(1337 4711))
1337
```

__`(cdr expr)`__

Get the right part of a pair:

```
>  (cdr '(1337 4711))
(4711)
```

<br>

__Pair mutation__

__`(set-car! expr expr)`__

Mutably set the left part of a pair:

```
>  (define mylist '(123 456))
>  mylist
(123 456)
>  (set-car! mylist 321)
>  mylist
(321 456)
```

__`(set-cdr! expr expr)`__

Mutably set the right part of a pair:

```
>  (define mylist '(123 456))
>  mylist
(123 456)
>  (set-cdr! mylist 789)
> mylist
(123 . 789)
```

### Compare Functions

__Equality__

__`(= expr expr)`__ Numbers only

```
>  (= 123 123)
#t
```

__`(eq? expr expr)`__ Reference only; no deep equals at the moment

```
>  (define foo "foo")
>  (define bar foo)
>  (eq? foo bar)
#t
```
__Less than__

__`(< expr expr)`__ Numbers only

```
>  (< 1 123)
#t
```

### Boolean type check Functions

`(number? expr)`

`(symbol? expr)`

`(string? expr)`

`(cons? expr)`

`(function? expr)`

`(builtin-function? expr)`

`(syntax? expr)`

`(builtin-syntax? expr)`

`(binding? expr)`

# Todos and Problems

* There is no garbage collection at the moment (naiv mallocing everywhere ;))
* A wide range of builtin syntax and functions is still missing
* Still enough edge cases where random errors creep in
* Code style inconsistencies
* Refactoring
* Harness the power of continuations (call/cc etc.)
