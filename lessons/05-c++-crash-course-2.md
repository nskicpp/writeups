C++ Crash Course 2
==================

In [the last lesson][lesson-4], we saw how to write programs in a
small subset of C++.  This subset is Turing-Complete, so you can
describe an arbitrary computation with it.  While this is an
interesting observation, you probably found implementing the programs
described in the exercises tedious and repetitive.  Starting with this
lesson, we will be looking at some of the abstractive capabilities of
C++ that allow us to simplify this code and make it cleaner, clearer,
and more expressive.  In tandem with this, we will start to look at
some of the lower level details of the C++ language, enough that we
can roughly understand how the abstractions we make perform and why
these language features are designed the way they are.

[lesson-4]: https://github.com/nskicpp/writeups/blob/master/lessons/04-c%2B%2B-crash-course-1.md

Three Useful Abstractions
-------------------------

Before we start talking about the abstractive capabilities of C++,
we'll briefly touch on three types that you will find very useful.  So
far, we've only seen types that are included in every C++
implementation, either as part of the language itself or included in
the standard library (under the `std` namespace).  You'll remember
that we installed an additional set of libraries in
[Lesson 3][lesson-3], the [Boost libraries][boost-website].  This set
of libraries originally started out as an open source staging ground
for components to be added to the standard library (like
`std::array`).  In this section, we will discuss some libraries that
are being reviewed for standardization, two of which are very likely to
be included in C++17 and another which has a good chance.

[lesson-3]:      https://github.com/nskicpp/writeups/blob/master/lessons/03-dev-environment.md
[boost-website]: http://www.boost.org/

`boost::optional<T>`
--------------------

Let's consider `cin`'s interface.

```cpp
cin >> var;
```

Each `>>` operation has two expected outcomes: one is that can
successfully read an input, and one where there is no more input to
read.  Neither of these is exceptional, so it doesn't make sense to call
either one an error.  `cin` deals with this by mutating `var` on
successful input and not mutating it on unsuccessful input; we then
check for success by converting it to a `bool` (as in an `if`
statement).  The two step process is somewhat strange.  Really what we
want is an operation that returns “value-or-nothing”.

Conceptually, this type should be able to store any value of some
arbitrary type `T`, and a special extra value that means “none”.  You
may be used to a type like this in functional-influenced languages—it's
very similar to Haskell's `Maybe a` type constructor, OCaml's `'a
option`, or C#'s `T?`.  Luckily, Boost provides a template like this for
C++: `boost::optional<T>`.

To keep this section brief, we will illustrate `optional` with an
example of its use:

```cpp
#include <boost/optional.hpp>

int main() {
  using namespace boost;

  auto o1 = optional<int>{};     // empty optional
  auto o2 = optional<int>{ 5 };  // optional which contains int 5
  auto o3 = make_optional(5);    // same as, preferred to above

  auto i1 = *o1;                 // Error!
  auto i2 = *o2;                 // int 5
  auto i3 = *o3;                 // int 5

  auto i4 = o3.value();          // int 5

  auto i5 = o1.value_or(0);      // int 0
  auto i6 = o3.value_or(0);      // int 5

  if (o1) { }  // Doesn't execute
  if (o3) { }  // Executes

  // You can compare optional<T> for equality and inequality with ==
  // and !=.  You can compare optional<T> for ordering with <, <=, >,
  // and >= as long as T can be ordered.
}
```

For more details on `optional<T>`, please consult the library's
[documentation][boost-optional-docs].  It is uses advanced terminology
and features we won't understand for a while, so don't focus on the
syntax or discussion, and just on the interface it provides.

Back to `cin`.  We could imagine, if it were designed with `optional` in
mind, that the interface would return an empty `optional` at the end of
the input stream.

One final note: GCC's standard library provides a version of
`optional<T>` that is proposed for inclusion in C++17.  Because it's not
yet in the standard, it's called `std::experimental::optional<T>`, and
you can find it in the `<experimental/optional>` header.  Its interface
is slightly different from `boost::optional<T>`.  In this course, we'll
prefer the Boost version, because it works across all major compilers,
but we'll eventually look at the `std::experimental::` version.

[boost-optional-docs]: http://www.boost.org/doc/libs/1_59_0/libs/optional/doc/html/index.html

`boost::variant<T1, T2, …>`
===========================

`optional<T>` is the C++ analogue to the OCaml `'a optional`,
Haskell `Maybe a`, and C# `T?` types.  The former two are true
Algebraic Data Types: you have a set of type constructors that make a
type, and only one can be engaged at a time.  We can build a
generalization of this in C++ as well: in Boost, this is called
`variant`.

A `boost::variant<T1, T2, …>` is an object that holds a `T1`, a `T2`,
and so forth, but only one of them can be engaged at a time.  To
construct a `variant` over `int` and `char`, for instance, we write
the following:

```cpp
auto int_or_char = boost::variant<int, char>{};
```

`int_or_char` is now an object that can store either an `int` or a
`char`.  When we construct it that way, it stores the first template
argument, an `int`, and it constructs it as `int{}`, which is `0`.  If
we instead wanted it to store a `char`, we could construct it with an
argument:

```cpp
auto int_or_char = boost::variant<int, char>{ 'a' };
```

Or, we could assign to it:

```cpp
int_or_char = 'a';
```

To get the value out of the `variant`, we need to be able to say what
the type that is stored in it is.  We do this with the `get<T>`
function template.

```cpp
auto c = get<char>(int_or_char);
```

If the currently held value is not a `char`, we will get a runtime
exception, so there is no chance for corrupted memory or incorrect
values.  This does, though, result in a runtime check that often can't
be optimized out.  (There is another, more powerful way of using
`variant` that is reminiscent of Haskell pattern matching, but we will
discuss it later, because the technique is much more generally
applicable, but also more complex.)

To use `variant`, include the `<boost/variant.hpp>` header.  You can
read [the documentation for `boost::variant`][boost-variant-docs],
though it focuses a lot on things you won't need to worry about.

__Discussion__: How could you use `variant` to build an abstraction
similar to `optional`?

[boost-variant-docs]: http://www.boost.org/doc/libs/1_59_0/doc/html/variant.html

`boost::any`
============

`boost::variant<T1, T2, …>` is a type that can store a pre-specified
set of other types.  This is often useful, because it enables the
compiler to typecheck expressions with `variant`s to make sure we're
not passing in a type we didn't expect.  Sometimes, though, we want to
accept absolutely any type, but still do so in a safe way that doesn't
subvert the type system.  In essence, we want the runtime flexibility
of a dynamically-typed language like Python as well as the safety of a
statically typed language like the C++ we've seen so far.  We can
accomplish this with `boost::any`.

A `boost::any` is essentially a `boost::variant<Ts>`, where `Ts` is
the set of every possible type, along with a special _empty_ state.
You can construct it with any type, but you are only allowed to get a
value out of it if you can say the type's name.

To construct an empty `boost::any`, we write:

```cpp
auto empty_any = boost::any{};
```

Or, to construct a `boost::any` that holds an object of some type `T`,
we write:

```cpp
auto any_with_tea = boost::any{ T{} };
```

You can check whether an `any` is empty using the `empty()` member
function:

```cpp
empty_any.empty()          // ← true
any_with_tea.empty()       // ← false
```

Finally, if you can say the name of the type stored within the `any`,
you can retrieve the value, using the `any_cast<T>` template function:

```cpp
auto t = any_cast<T>(any_with_tea);
```

If the object stored within the `boost::any` is not of type `T`, this
throws an exception.  This means it is not possible to subvert the
type system: either you get back the type you wanted, or you get back
nothing at all.  This, however, has to happen at runtime, which makes
it slower than directly storing the `T`.

The way `boost::any` works is with a technique called _type erasure_;
you may have heard this term before, but this technique is likely not
what non-C++ programmers have meant in saying that.  We will be
looking at type erasure in the next unit.

You can look through the documentation [here][boost-any-docs].

__Discussion__: What are some places you might want a type like
`boost::any`?

[boost-any-docs]: http://www.boost.org/doc/libs/1_59_0/doc/html/boost/any.html

Functions
---------

Our primary means of organizing code in C++ is the _function_.  We have
talked about functions before, but to recap, a function is a piece of
code with a single, well-defined point of entry and return to the same
place they were called from.  Functions can take an arbitrary number of
arguments and can return zero or one values.

### Our first function

Let's consider a motivating example.  You are writing a calculator that
accepts values in imperial units, but does all calculations internally
in SI units.  Knowing you're likely to mix up the conversion, you decide
to separate the code that converts from feet to meters into a separate
function that you can call from anywhere in the codebase.  This
function—we'll call it `feet_to_meters`—should take a `double`
representing a length in `feet` and return a `double` representing an
equal length in meters.

For reference, here is our original code:

```cpp
#include <iostream>

int main() {
  using namespace std;

  auto input1 = 0.;
  cin >> input1;
  input1 *= 0.3048;

  auto input2 = 0.;
  cin >> input2;
  input2 *= 0.3048;

  cout << input1 * input2 << '\n';
}
```

We can split the function out like this:

```cpp
#include <iostream>

auto feet_to_meters(double length_in_feet) {
  return length_in_feet      // ft
    * 12. / 1.               // -> in
    * .254                   // -> cm
    * 1. / 100.;             // -> m
}

int main() {
  using namespace std;

  auto input1_ft = 0.;
  auto input2_ft = 0.;
  cin >> input1_ft >> input2_ft;
  
  auto input1_m = feet_to_meters(input1_ft);
  auto input2_m = feet_to_meters(input2_ft);

  cout << input1_m * input_m << '\n';
}
```

This has cleared up the code (and also made the conversion
definitionally correct, barring floating point errors).

You'll notice that the syntax for defining functions is similar to the
syntax for declaring variables.  We start with `auto` and then give the
function a name.  After that, though, we give the parameter list in
parentheses and the _function body_ afterwards, enclosed in braces.  The
syntax for calling a function matches this, too, with the argument
expressions to the function call enclosed in parentheses.

Inside our function body, we have a single statement, although we are
allowed as many as we want.  This statement, called a `return`
statement, evaluates the expression that appears after it (in this case,
the long arithmetic expression) and immediately returns it to the
caller.  For any function that returns a value, you need a `return`
statement for every branch the function can take.  The compiler deduces
the return type of the function from the type of the expression in the
`return` statements.

We've seen these syntaxes before: the function call syntax is exactly
the same as when we used `get<N>` for `tuple`s and `array`s, and the
function definition syntax is similar to `main`'s syntax (`main` is a
special function that you are not allowed to call and which doesn't need
a `return` statement, so it will look a little bit different).

### Evaluating a Function

Let's take a more complex example to see the exact steps the compiler
takes when evaluating a function call:

```cpp
auto f(int x, char c) {
  return make_tuple(x + 1, c);
}

// …

auto result = f(10 - 2, 'a');
```

As this code is executing, it comes to the line with the function call.
First, it evaluates the argument expressions, though in no defined
order: `'a'` is a literal, which evaluates to itself, and `10 - 2` ends
up evaluating to `8`.  Then, it jumps to `f`, and assigns `8` to `x` and
`'a'` to `c`.  Finally, it evaluates `f`'s function body, starting at
the beginning of the block and executing statements in order.  The first
(and only) statement is a `return` statement, so the compiler evaluates
the expression to the right of the `return` keyword, creating a
`tuple<int, char>` that holds `9`and `'a'`.  It then immediately stops
executing any remaining code in the body, and returns our tuple back to
the caller, which names it `result`.

__Discussion__: Can you think of any benefits for leaving the
evaluation order of function arguments undefined?

### Declarations

Let's throw a little bit more complexity into this discussion.  We've
seen that the compiler deduces the return type based on the type of
the `return` statement.  Thus, to know the type of the function, the
compiler needs to see the body of the function.  This is pretty
obvious.

What happens, then, when we introduce a second file into the program?
If you remember our discussion of the C++ toolchain, each file is
compiled separately, without knowing about any others, and only at
link time are they all brought together.  While this allows much
faster recompiles than other methods, it does present some interesting
design problems: the compiler needs to know the type information of a
function to type check it, but it doesn't get to see that function
until long after it's performed type checking.

To solve this problem, C++ has the concept of _declarations_.  A
declaration simply introduces a name into a scope and provides the
necessary information for the compiler to be able to use it.  This
information has to match the definition; otherwise, the linker will
give an error.

We can declare a function `f` that returns type `T` and takes three
arguments, an `int` and then two `char`s, using the following
_function declaration_ statement:

```cpp
T f(int, char, char);
```

Note that we haven't told the compiler what happens when `f` is
called.  We've only asserted that there's a function `f` in the
current namespace, it takes in an `int` and two `char`s, and it gives
back a `T`.  This is all the compiler needs to be able to call the
function from a certain scope—going back to our description of the
function abstraction above, we've fulfilled all three parts of the
contract between a function caller and function body.

Function declarations are often placed in a header file to be
`#include`d by other translation units.  You may also hear a function
declaration referred to as a _function forward declaration_ or,
especially by recovering C programmers, a _function prototype_.

For additional documentation, we can also give some or all of the
parameters in the declaration a name:

```cpp
T f(int max_length, char splitter, char terminator);
```

These parameter names are completely ignored by the compiler, so they
are only for the the benefit of readers of the code (or tooling on the
code).  They can differ from the names of the arguments in the
definition, which has the nice effect of allowing short names in the
implementation and more descriptive names in the declaration.

The function `f` logically has to be globally unique.  It can't have
multiple bodies; otherwise we'd need rules to govern which body is
called when.  As such, although there can be as many declarations of
`f` as you want in a program (and even multiple in the same
translation unit, or same scope even), there must be at most one
definition of `f` in at most one place.  This is called the _one
definition rule_, and it is pervasive within C++, across all sorts of
abstractions.

### Return values and `void`

Our function `f` above returned a type `T`, which we assumed to be
constructible.  So far, this has been true of all types we've seen—you
can create instances of them.  C++ has a special type that you can't
create instances of, because it can't hold any values: `void`.  It is
impossible to create a `void`, so statements like `void{}` don't work.

Why is this useful?  Well, not all functions need to return a value.
Remember, our function abstraction is not the same as a mathematical
function; it's rather a list of instructions with a single point of
entry.

We can say our function `f` does not return a value the following way:

```cpp
void f(int, char, char);
```

Then, we can call it this way:

```cpp
f(5, 'c', '\n');          // ← note the semicolon at the end!
```

This isn't an expression anymore, because evaluating it doesn't give us
a value.  We need that semicolon at the end, and something like the
following is illegal:

```cpp
auto var = f(5, 'c', '\n');
```

We've seen how to return a value from a function using a `return`
statement.  In a `void` function, we can simply write

```cpp
auto f() {
  // …
  return;
  // unreachable code
}
```

or even more succinctly,

```cpp
auto f() {
  // …
}
```

leaving off the `return` statement entirely.

### Overloading

C++ functions allow something called _overloading_—namely, you are
allowed to have multiple functions with the same name, as long as their
argument lists are different.  We can, for instance, declare and define
the following two functions, even though they have the same name:

```cpp
int random_value();
int random_value(int seed);
```

Then, we can call either

```cpp
auto value = random_value();
```

or

```cpp
auto value = random_value(27);
```

Note that you can't overload on the return type only, because then
there would be no way for the compiler to distinguish which one you
meant.  Consider the following code:

```cpp
int random_value();
char random_value();

auto value = random_value();   // What is the type of value?
```

__Discussion__: What are the values of function overloading?  What are
the drawbacks.  Discuss and evaluate techniques to minimize the
drawbacks while maximizing the benefits.  What are some good
guidelines for when to overload a function and when to use two
different names?

### Anonymous Functions

C++ does have anonymous functions, but we are going to hold off on
learning about them for two lessons, because once we have learned one
more fundamental idea and a technique to use anonymous functions more
easily.

Exercises
---------

Before doing these exercises, please go through the __Additional
Reading__ section below, and consider it when implementing these
exercises.

__Exercise__: Refactor your exercises from Lesson 4 using functions.

__Exercise__: Implement a function that sorts a `std::vector<string>`
lexicographically (using `<`), and a program that accepts inputs
line-by-line and prints them out sorted lexicographically.

__Exercise__: Create a program that takes an arbitrarily long string
of only zeros and ones and prints out the string with
[_Hamming codes_][hamming-code] added.  Do this octet by octet, from
left to right.  Write another program that accepts a string of zeros
and ones with Hamming codes in them, and either:

  1. Prints the string out without Hamming codes to standard out if
     the Hamming codes are correct.
  2. Corrects the payload and prints it without Hamming codes to
     standard out, if the Hamming codes indicate bits were changed.

[hamming-code]: http://users.cs.fiu.edu/~downeyt/cop3402/hamming.html

__Exercise__: Implement the [Boyer–Moore exact string matching
algorithm][boyer-moore].  Write two programs that accept a string `T`
that is terminated by a newline and a pattern string `P` that is
terminated by a newline, and return success if `T` exactly matches
pattern `P`.  For one, use your implementation.  For the other, use
[`std::string::find()`][std-string-find].  Profile both programs on
sufficiently long strings using the UNIX `time` command.  Which is
faster?

[std-string-find]: https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string_search_algorithm

Additional Reading
------------------

Please read [Scripts as Modules][modulinos] by brian d foy.  This
article uses Perl 5, so the syntax may be unfamiliar, but try to
understand the concepts presented in the article.

__Discussion__: How do can you apply these ideas in your C++ codebase?
What benefits would they bring?  What ideas do not map over to C++?

[modulinos]: http://www.drdobbs.com/scripts-as-modules/184416165
