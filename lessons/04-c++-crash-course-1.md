C++ Crash Course 1
==================

After this lesson, you should be able to read and write basic C++
code.  We will be covering the most important features, the ones
you'll be using every day, and not working about esoteric or very
low-level, unsafe features.  We'll be introducing those later, but for
now, we'll be covering the simplest and most important parts.

Focus on the ideas and don't get caught up in details or in syntax.
This lesson corresponds in ideas to the first chapter of _A Tour of
C++_, so you have seen a lot of this material before.  This lesson
presents it in a slightly different way, though, and focuses more on
teaching you good habits than giving you a full picture of the
language.  Even if you have some experience with C or C++, I ask that
you forget much of that and work through this lesson carefully.

I ask that you only use the features we talk about here at first, not
any that you may know previously from C, that you may find online, or
even that you may have learned in _A Tour of C++_.  I've carefully
selected the features here to teach good habits, because they should
be your go-to features.  We'll get to the other features in good time.

Variables and Values
--------------------

Recall from your reading in _Elements_ what Stepanov and McJones term
an _entity_.  We often conceptualize our programs to manipulate
entities, without regard for their representation on the computer.  In
order to tell the computer what to do at all, though, we have to
associate a representation with our entity.  Once we associate a
representation with an entity, we have a _value_.  Put another way, a
value is a representation and an associated meaning.  C++'s most
fundamental piece of data is the value, of which there are an
unbounded number.  As with any language, though, we need a syntax to
write values.

### Literals

A _literal_ is exactly that.  That is, a literal is the textual
representation of a value.  For example, if we want to express the
value of “five” in a C++ program, we write `5`.  If we want to express
the value of “a string of the characters f, i, v, and e”, we write
`"five"s`.  As you can tell, we have already seen some literals
before—we used string literals in the previous lesson.

Values, and thus literals, on their own aren't terribly useful—we want
to store them in objects and manipulate those objects.  To do that, we
need _variables_.

### Variables

A _variable_ creates a place in memory to store values.  The syntax
for declaring a variable is as follows:

```cpp
auto var_name = some_expression;
```

This statement creates a variable with the name `var_name`, and
initializes it to the result of `some_expression`.  Its type, even
though it is not explicitly stated, is whatever type `some_expression`
has.  If `some_expression` is `0`, `var_name` has type `int`.  If it
instead is `"hello"s`, `var_name` will have type `std::string`.

```cpp
auto character_count = 140;               // type is int
auto warning_text = "Don't do that!"s;    // type is std::string
```

Once you have a variable, you can change its value by using the
_assignment operator_, which is a single equals sign.  The value you
are assigning to the variable will need to have the same type as the
variable itself or needs to be _convertible_ to the type of the
variable; otherwise, you'll get an error.

```cpp
// Create variable and initialize it.
auto current_page_number = 51;
// Reassign it to new values.
current_page_number = 62;
current_page_number = 63.3;     // warning from compiler, 63.3 -> 63
current_page_number = "error"s; // error from compiler
```

This may come as a surprise: unlike in other languages, `string`s are
not convertible to other types, and vice versa.  To convert a number
to a string, use the `std::to_string` function template.  To convert a
string to an integer, use the `std::stoi` function (“string-to-int”).
There are a number of reasons for this:

  1. Conversion from a string to an integer or vice versa is not a
     quick operation, so we don't want it to happen automatically
     behind the scenes.  We want to explicitly say when we're doing
     it.

  2. What number base should we use to convert an integer to a string?
     Which number characters should we use (Arabic numerals?  Indian
     numerals?  CJK numerals?)?  Should we output leading zeros or
     not?  What about digit separators?  All of these make sense in
     certain situations, and there's no _natural_ representation; we'd
     be _defining_ a representation.

     This actually turns out to be a problem with interfacing with
     other languages.  ECMAScript, Python, and Perl have all defined
     slightly incompatible formats for integer to string conversion,
     which can cause data loss when interfacing between the languages.
     We may not care in our application, but can we be sure that no
     user of C++ cares?

  3. How do we handle errors?  Should we lose information, or should
     we If the conversion happens without extra
     syntax, it may be easy to overlook places where there is a large
     chance for errors.

Because of these, it doesn't make sense to privilege a certain
conversion mechanism with an implicit syntax.  It turns out these
exact considerations apply when we're designing our own conversion
code as well, which we'll see later.

What if we have an expression of some type `T`, but we really want our
variable to be of type `U`?  As long as we can convert a `T` to a `U`
without losing information, we can _explicitly declare_ the type of
our variable this way:

```cpp
auto var_of_type_U = U{ expr_of_type_T };
```

Let's look closely at this expression.  Nothing magical is going on
here, actually.  We are still following the variable declaration
syntax we learned earlier, but we've wrapped our `expr_of_type_T` in
an expression to create a _temporary_ of type `U`.  For the moment, we
will think of _temporaries_ as nameless objects; next lesson we will
come back to this definition and fine-tune it.  So, we first create a
temporary of type `U` out of the result of evaluating `expr_of_type_T`
and then we initialize our `var_of_type_U` with it.  `auto` deduces
the type of `var_of_type_U` based on the type of the right-hand side
of the equals sign, which is `U`.  The new `var_of_type_U` becomes a
copy of the temporary object.

You may be tempted to use this syntax with an explicit type name for
documentation or to aid the reader of your code.  Certainly writing
code as follows is better for the reader of the code:

```cpp
auto var = int{ some_opaque_function_name() };
```

While this sentiment is definitely a good one, this syntax is not the
correct tool for doing that.  We do gain some readability, certainly,
but we lose flexibility in our code.  When we're reading code, we
usually don't care about the exact _type_ of a variable (“Is this an
`int`?”), but rather the _interface_ with which we use the type (“Can
I add two of them together with `+`?”).  This idea should sound
familiar: we care about a _concept_ that the type satisfies.  For
example, we care not whether the type is `int`, but whether it's
`Integral`.  We'll find that C++ has many built-in `Integral` types,
and that explicitly converting them all to an `int` is _not_ what we
want.

Even if we check the return type of the function and make sure we use
that type exactly, this still isn't what we want.  What if the return
type is changed after the code is written?  We might get lucky and get
a compile error, and then we can change the type to what the new
function return type is.  More likely, though, is that we get unlucky,
and the new return type can be explicitly converted to what we
wrote—we now have an _extremely subtle_ bug, brought on because we're
misunderstanding what the syntax really means.  Furthermore, we're
misleading our readers into thinking we actually meant the explicit
conversion to the type we wrote.

Finally, in certain contexts where we are doing complex computation on
types, there is no way for us to know what the return type is going to
be.  All we may know is that it is `Integral`.  We can't express that,
though, by using the explicit type syntax.  The solution of writing
the type, then, is not universal.

We don't want a partial solution ridden with pitfalls, and especially
not one that doesn't say what we're really trying to express.
Unfortunately, C++ currently doesn't have a way to express what we
want to express in the syntax (although it will soon, and we will be
playing around with it), so for now follow this best practice: use
`auto var = expr;` whenever possible, and `auto var = type{ expr }` to
explicitly say “`expr` has a different type, but I really, really need
to convert that to `type`.”

As I said before, this syntax works if the compiler can prove we
aren't going to lose any information in the conversion.  As a last
resort, we can explicitly tell the compiler to perform the potentially
unsafe operation with a `static_cast`.  A _cast_ is an explicit
declaration that we're subverting the C++ type system.  It's for when
we know better than the type system for some reason.  As you can
imagine, this is not incredibly common, and so the syntax for it is
verbose and ugly—a big __look here!__ sign in the code.  Let's look at
an example of it:

```cpp
auto i = 0;
i = static_cast<int>(5.6);          // fractional part is discarded, i is 5
// cf. i = 5.0; or i = int{ 5.0 };
```

When we convert a `double` to an `int`, we necessarily lose the
fractional part.  If the compiler can prove that this won't lose any
information (because the fractional part is zero), there is no
problem.  But if the compiler cannot prove this, or it can prove that
it will lose information, we have to use a `static_cast`.

We can combine some of these features together:

```cpp
auto i = static_cast<int>(3.14);
i = int{ 5.0 }; // construct new int from double, assign that to i
```

In summary,

  - Create and initialize a variable using the syntax:
    ```cpp
    auto var_name = expr;
    ```

  - Assign a new value to a variable using the syntax:
    ```cpp
    var_name = new_expr;
    ```

  - Construct a temporary of type `T` using this syntax:
    ```cpp
    T{ arguments… }
    ```

  - When it's safe to convert from an expression's type to type `T`,
    do so using this syntax:
    ```cpp
    T{ expr }             // constructs new temporary of type T from expr
    ```

  - When we have to subvert the type system, because we know something
    the compiler doesn't (like that losing information is okay in a
    specific case), cast an expression's type to `T` using this
    syntax:
    ```cpp
    static_cast<T>(expr);
    ```

### `const`

The `const` keyword creates a compiler-enforced promise that you will
not write to a variable, only read from it.  This means you won't be
able perform any operations that modify it.

```cpp
const auto i = 0;
i = 1;                  // write to i: compiler error!
auto j = i;             // read from i: no problem
```

Immutability allows us to reason more easily about our software, but
using it as much as possible is unidiomatic.  We'll later see it
inhibits a special property of some types that gives us correctness,
readability, and substantial performance benefits.  When our method
bodies are small and interactions are localized, mutability can allow
us to express our intent more clearly.

### Useful types

Here are some useful types and their literal syntax you'll want to
know about.  There are others, but these should be your defaults
unless if you really, really know you need others.

  - `int`: Stores positive and negative integers.  `int` literals are
    written as a zero or a string of digits and single quotes not
    starting with a nonzero digit.

    ```cpp
    auto x = 0;
    auto y = 13;
    auto z = 10'000'000; // easier to read than 10000000
    ```
  - `double`: Stores floating-point numbers.  `double` literals are
    written like `int` literals, but have a period.

    ```cpp
    auto x = 0.;                 // or 0.0 or .0
    auto y = 1.333;
    auto z = 10'000'000.333'333;
    ```

    We can also write `double` literals in scientific notation, with
    an `e` or `E` in the literal:

    ```cpp
    auto x = 1.0e10;   // 1 × 10¹⁰
    auto y = 1e10;     // Same thing
    auto z = 1.2E-3;   // 1.2 × 10¯³
    ```
  - `bool`: Stores a true value or a false value.  `true` and `false`
    are the `bool` literals.

    ```cpp
    auto x = true;
    auto y = false;
    ```
  - `std::string`: Stores a character string.  `std::string` literals
    are written with an initial `"` and a final `"s`.  (Remember!  You
    need to write `using namespace std::literals;` to use this!)

    Within a string, you can write certain escapes: `\\` for a
    backslash, `\"` for a double quote, `\' for a single quote, `\n`
    for a newline, `\t` for a horizontal tab.

    ```cpp
    #include <string>
    using namespace std::literals;

    auto x = "something"s;
    auto y = "This has a \" and a \\ in it"s;
    auto z = "  initial spaces and\nsome other \tcharacters"s;
    auto w = "We can write \' or ' in a string literal"s;
    ```
  - `char`: Stores a character.  `char` literals are a pair of single
    quotes (`'`) with a single character (or one of the above escape
    sequences) in between.

    ```cpp
    auto x = 'x';
    auto y = '\n';
    auto z = '\'';  // Can't use a normal ' in the literal
    auto w = '"';   // But can use a normal ", as well as \"
    ```

  - `std::complex<double>`: Stores a complex number.
    `std::complex<double>`s have two parts: the real part, which is
    just a `double`, and the imaginary part, whose literal is written
    as a `double` with an `i` suffix.  Just as with `std::string`
    literals, you will need to write `using namespace std::literals;`
    to use this.

    ```cpp
    #include <complex>
    using namespace std::literals;

    auto x = 1. + 0.i;
    auto y = 3.4i;
    auto z = 1. - 3.4i;
    ```

Operators
---------

You may have noticed that in the above discussion, we stated that our
numeric types can hold negative values, but we never showed the way to
write them.  In this section, we will briefly look at operators and
how to use them.  This will be a very cursory glance at operators, and
we will get much more detail later.

In C++, there is a closed set of operators, so you aren't allowed to
declare new ones.  The exact meaning of operators is determined by
their arguments' types; we say the operators are _overloaded_ on their
arguments' types.  An overriding principle, however, is one of
_interchangeability_: we want the same operator to mean roughly the
same thing, even when applied to different types.  This principle will
carry over into a lot of other parts of the language.

We will be looking at two types of operators for the moment: _unary
operators_, which take one argument, and _binary operators_, which
take two operators.  Another axis along which we can classify our
operators is _prefix_ (the operator precedes its arguments), _infix_
(the operator comes between two of its arguments), and _postfix_ (the
operator follows its arguments).

A holdover from C is a complex set of _type promotion_ rules that
apply to operators.  When a binary operator is applied to arguments of
two different built-in types, one of the values will change to a
different type.  We will hold off on looking at the exact rules for
this, but it is something to beware of.  With the types we've seen so
far, these mostly don't do anything surprising.  We will talk about
type promotion rules later, because they will be a pitfall when we
talk about more advanced features in C++.

### Arithmetic operators

The following operators apply to numeric values (`int`, `double`,
`complex<double>`).  The operators are applied first according to the
order in the table below, and then within each group, left-to-right.

| Operator   | Effect                                               |
| ---------- | ---------------------------------------------------- |
| Prefix `+` | No effect on a numeric value, useful for clarity     |
| Prefix `-` | Negates a numeric value                              |
| ---------- | ---------------------------------------------------- |
| Infix `*`  | Multiplies two numeric values                        |
| Infix `/`  | Divides two numeric values                           |
| Infix `%`  | Returns the remainder of dividing two numeric values |
| ---------- | ---------------------------------------------------- |
| Infix `+`  | Adds two numeric values                              |
| Infix `-`  | Subtracts two numeric values                         |

As a shorthand for `var = var `op`value;`, you can write `var `op`=
value;` As a shorthand for `var += 1;`, you can write `++var;`, and as
the shorthand for `var -= 1;`, you can write `--var;`.  The following
code illustrates the usage of each of these operators.

```cpp
auto i = 10.;
auto j = 20.;

i = +i;
i = -i;

i = i * j;      // i *= j;
i = i / j;      // i /= j;
i = i % j;      // i %= j;

i = i + j;      // i += j;
i = i - j;      // i -= j;

++i;
--i;

i = i + j * i;
i = (i + j) * i;
```

### String operators

You can also use the infix binary `+` to concatenate two `string`s
together.

```cpp
auto s = "abc"s + "def"s;
```

### Logical operators

The following operators take `bool`ean arguments and return a
`bool`ean value.

| Operator   | Effect                                               |
| ---------- | ---------------------------------------------------- |
| Prefix `!` | Negates a boolean value.                             |
| ---------- | ---------------------------------------------------- |
| Infix `&&` | Boolean AND                                          |
| ---------- | ---------------------------------------------------- |
| Infix `||` | Boolean OR                                           |

The `&&` and `||` operators _short-circuit_.  This means `&&` only
evaluates its right-hand argument if its left-hand argument is
`true`, and `||` only evaluates its right-hand argument if its
left-hand argument is `false`.

### Relational operators

The following operators take any type we've discussed so far but
`complex<double>` and return a `bool`.

| Operator   | Effect                                                   |
| ---------- | -------------------------------------------------------- |
| Infix `<`  | Is the left argument less than the right?                |
| Infix `>`  | Is the left argument greater than the right?             |
| Infix `<=` | Is the left argument less than or equal to the right?    |
| Infix `>=` | Is the left argument greater than or equal to the right? |

__Discussion__: Why do these not make sense with `complex<double>`?

The following operators take two arguments of the same type and return
a `bool`.

| Operator   | Effect                                                   |
| ---------- | -------------------------------------------------------- |
| Infix `==` | Is the left argument equal to the right?                 |
| Infix `!=` | Is the left argument not equal to the right?             |

Compound Data Types
-------------------

### Arrays

An _array_ is a compound data type that contains a fixed number of
elements of a single type, where each element is adjacent in memory.
In C++, an array of `N` adjacent `T`s is written with as
`std::array<T,N>`.

```cpp
#include <array> // Include header to use arrays
auto x = array<int, 10>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
auto y = array<double, 3>{ 0., 0., 0. };
auto z = array<array<int, 2>, 2>{ { 0, 1 }, { 2, 3 } };
```

Because the number is part of the type, we can't resize this array
dynamically.  However, we can still get the size (which will never
vary) using the `size` member function.  This is often a better
approach than hard-coding the size.

```cpp
// We'll see why this cast is necessary later.
auto size = static_cast<int>(x.size()); // size is now 10
```

To get an object out of an array at a specific index, you can use the
following:

```cpp
auto first_element  = x[0];    // precondition that the index is in the array
auto second_element = x.at(1); // performs bounds checks (slower, safer)
auto n              = 3;
auto nth_element    = x[n];    // this is a run-time operation
```

If the index is known at compile time (that is, it's not in a
variable), we can use the `get<>` function template to access an
element.  This performs bounds checking _at compile time_, so you are
guaranteed that all array accesses using this are valid.

```cpp
auto third_element = get<2>(x);
```

### Tuples

A _tuple_ is a compound data type that contains a fixed number of
homogeneous types.  In C++, a tuple of `T1`, `T2`, …, `Tn` is written
as `std::tuple<T1, T2, …, Tn>`.

```cpp
#include <tuple> // Include header to use tuples
auto x = tuple<int, string, double>{ 1, "hello"s, 2.71828 };
auto y = tuple<int, int>{ 5, 10 };
auto z = tuple<string, array<int, 3>>{ "A string"s, { 1, 2, 3 } };
```

Tuples also don't have a literal syntax, but we can use the
`std::make_tuple` function template to construct them and deduce the
types in the tuple.  We strongly prefer this over explicitly writing
the type above (and hope the proposed `std::make_array` gets in the
standard in C++17 as well—we will write our own in a few lessons
anyhow).

```cpp
auto tup = make_tuple(1, "hello"s, 2.71828);

return make_tuple(5.2, true); // returns tuple<double, bool>
// same as:
return tuple<double, bool>{ 5.2, true };
```

It turns out that tuples are very useful in C++, because they allow us
to easily (and efficiently) return multiple values from a function.
To unpack a tuple, you can use the `tie` function template.

```cpp
// We haven't seen functions yet, but here's an example of one:
auto my_function() { return make_tuple(1, "abc"s); }
// We'll talk about them in detail in the next lesson.

// Later on
auto i = 0;
auto s = ""s;

tie(i, s) = my_function();
```

If we want to get a value out of a tuple, we quickly run into a
problem: because C++ is statically-typed, it needs to know the type of
an expression at compile time.  If we used something like `[]` or the
`at()` function, though, this can't be done.  What would the compiler
know about the type in the following program?

```cpp
#include <iostream>
#include <tuple>

int main() {
  using namespace std;

  auto i = 0;
  cin >> i;

  auto tup = make_tuple(1, 1., true);
  /* ??? */ member = tup.at(i);
}
```

With an array, since all the values have the same type, we don't need
to worry about this.  Tuples don't have this guarantee, though.  To
avoid a situation like this, we need to know the index at compile
time.  Again, we use the `get` function template for this, and we get
the same compile time bounds checking.

```cpp
auto val = get<3>(make_tuple(1, 1., true)); // val must be a bool!
```

In fact, if we know the type of the value we want, and the tuple only
contains one element of that type, we can use that as our template
parameter to `get`:

```cpp
auto val = get<bool>(make_tuple(1, 1., true)); // gets the 3rd element!
```

### Dynamic Arrays (Vectors)

A _vector_ is an array that can grow dynamically as elements are added
to it.  Like arrays, the elements are guaranteed to be stored
adjacently in memory.  A vector of `T` is written as `std::vector<T>`.

```cpp
#include <vector> // Include header to use vectors
auto x = vector<int>{ 1, 2, 3, 4 };
auto y = vector<string>{ "abc"s, "def"s };
auto z = vector<double>{};
```

There are five important operations to mutate a vector: `push_back`,
which adds an element to the end of the vector; `front` and `back`,
which return the elements at the beginning and end of the vector,
respectively, `pop_back`, which removes the element from the end of
the vector; `insert`, which inserts into any position in the array,
and `erase`, which erases an element.  `push_back` and `pop_back` are
guaranteed 𝒪(1) (amortized), while `insert` and `erase` are 𝒪(𝑛)
(because they have to move all elements after them).

```cpp
auto x = vector<int>{ 1, 2, 3, 4 };  // [ 1, 2, 3, 4 ]
x.push_back(5);                      // [ 1, 2, 3, 4, 5 ]
auto back = x.back();
x.pop_back();                        // [ 1, 2, 3, 4 ]
/* This below is a little weird, but we'll see why it makes sense and
   why it's more flexible than simple indexes when we talk about the
   design of the standard library in a later lesson.  For now, accept
   the weird syntax.  Sorry :(
*/
x.insert(begin(x) + 1, 0)            // [ 1, 0, 2, 3, 4 ]
x.insert(begin(x), { -1, -2, -3 })   // [ -1, -2, -3, 1, 0, 2, 3, 4 ]
x.erase(begin(x) + 3);               // [ -1, -2, -3, 0, 2, 3, 4 ]
x.erase(begin(x) + 2, begin(x) + 4); // [ -1, -2, 2, 3, 4 ]
```

As with an array, we can ask its size or request an element from it:

```cpp
auto length         = static_cast<int>(x.size());
auto second_element = x[1];     // no bounds checking
auto third_element  = x.at(2);  // bounds checking
```

Since a vector is runtime sized, we _can't_ ask for an element with
compile time bounds checking using `get`, unlike an array or tuple.

`vector` has a very large interface that we will tackle later, but
this should be enough for now.  You will have noticed, however, that
the interfaces of these types are very similar.  The standard library
is designed with this sort of _substitutability_ in mind.  Arrays and
dynamic arrays have similar interfaces, because they are similar in
idea.  Arrays can be thought of as tuples where all the elements are
the same type, so tuples and arrays also have similar interfaces.
This is our first look at a very powerful and fundamental idea that
will lead us to something called _Generic Programming_.

Control Flow
------------

Because C++ is an imperative language based on C (an imperative,
procedural language), our programs algorithms tend to be structured
around explicit control flow structures.  Several of these are
important enough that they are built into the language, but it is
idiomatic C++ to define functions that also act as control flow
structures; once we talk about the syntax for defining functions and
look at the standard library is the coming several lessons, we will
begin to use these.  For now, let's just look at the structures that
are built into the language.

Blocks
------

_Blocks_ are the simplest sort of control flow structure, to the point
where most people don't consider them control flow.  A block is a
series of statements that are executed in order—just like normal code.
The unique property about blocks is that they count as a single
statement themselves.  Thus, you might also hear them called _compound
statements_.

The syntax for blocks is as follows:

```cpp
{
  statement1;
  statement2;
  /* … */
  statementN;
}
```

You can nest blocks, as well:

```cpp
{
  auto str = "abc"s;
  {
    cout << str
         << ' ';
    str = "def"s;
  }
  cout << str << '\n';
}
```

There are not many uses for blocks on their own (although we will see
a very good one next lesson), but they can combine with the other
control structures very nicely.

Conditionals
------------

C++'s primary conditional structure is the `if` statement.  As in
other languages, `if` statements evaluate their conditional expression
(which must be convertible to a `bool`), and then, if it is `true`,
evaluate their first _branch_ (a statement).  If it is `false`, they
evaluate their `else` branch, if such a branch exists.  Then,
execution continues on.

This is how you use an `if` statement without an `else`:

```cpp
if (cond_expr)
  true_branch_statement;
```

With an `else`, this becomes:

```cpp
if (cond_expr)
  true_branch_statement;
else
  false_branch_statement;
```

If you want to execute more than a single statement in each branch,
you can use a block, which as you'll remember, count as a single
statement:

```cpp
if (cond_expr) {
  true_branch_statement1;
  true_branch_statement2;
  /* … */
  true_branch_statementN;
} else {
  false_branch_statement1;
  false_branch_statement2;
  /* … */
  false_branch_statementM;
}
```

Something you may want to know is that `else`s bind to the closest
`if` statement.  Consider the following code:

```cpp
if (famous)
  if (successful)
    happy = true;
else
  happy = false;
```

By the way this code is indented, you may think that you're not happy
unless you're famous.  However, the indentation does not match the
semantics of this code.  It would be better indented as:

```cpp
if (famous)
  if (successful)
    happy = true;
  else
    happy = false;
```

As you can see, this code says nothing about whether you are not happy
if you are not famous.  If you want the former behavior, you need to
use a block explicitly:

```cpp
if (famous) {
  if (successful)
    happy = true;
} else
  happy = false;
```

For this reason, many suggest always using blocks for your branches.
This can sometimes help readability and sometimes hurt it.  In this
course, we will take the approach “apply to taste”.  If you prefer
adding blocks, do so.  If you prefer not adding blocks, don't.  As
with many things, both arguments are equally good, and it comes down
to personal preference.  Consistency is key, and there are much better
things to worry about.  Either way, you will be dealing with code that
does both, so you will need to understand this property of `else`.

__Discussion__: As a always, a nuanced approach is often better.
Dogmas often are the result of false binaries.  Discuss this in the
context of block styles, and describe the failings of both extremes.

Loops
-----

A loop executes the same block of code many times.  Because the code
doesn't change, loops need some changing state to exit or, for the
most part, to do something useful.

### `while` Loops

Our simplest looping primitive in C++ is the `while` loop, which
executes a statement as long as a condition expression evaluates to
`true`.  The syntax for a `while` loop is this:

```cpp
while (condition)
  statement;
```

Again, we can use blocks to execute multiple commands:

```cpp
while (condition) {
  statement1;
  statement2;
  /* … */
  statementN;
}
```

With most `while` loops, they are only useful if they terminate, which
can only happen if `condition` evaluates to `false` because of some
state change.

From a `while` loop, we can build up any sort of other loop.  We can
make a repeat statement that repeats its body `n` times:

```cpp
auto i = 0;
while (i != n) {
  /* body */
  ++i;
}
```

It is common to use the following type of loop to express this, but we
will soon see a better and clearer way to abstract this.

### `for` Loops

When we are iterating over a container (an `array`, for instance, or a
`vector`), we can write code similar to what we did above.  We have an
_initializer_ statement that creates an _iterator_ variable (`auto i =
0;`), a _condition_ expression that checks whether the iterator is at
the end of container (`i != n`), and an _update_ statement that
changes state, updating the iterator.  It makes sense to abstract
this, since iterating over a container is so common; on the other
hand, we still want to be general enough to support arbitrary
initializers, conditions, and updates.

We have identified the three important parts of a `for` loop, and
pointed out their main use.  Here is the syntax for a `for` loop:

```cpp
for (initializer; condition; update)
  statement;
```

If any of these are not needed, we can simply leave them out.
Remember to keep the `;`, though.

Let's iterate over an `array` and print its contents:

```cpp
#include <iostream>
#include <array>

int main() {
  using namespace std;

  auto arr = array<int, 5>{ 0, 2, 3, 5, 6 };

  for (auto i = 0; i != 5; ++i)
    cout << arr[i] << ' ';
  cout << '\n';
}
```

This program will print `0 2 3 5 6` on a line.  If we want to print
them in reverse order, we don't have to touch our loop body at all; we
just change our `for` loop:

```cpp
for (auto i = 4; i != -1; --i)
  cout << arr[i] << ' ';
```

We are still dealing with a low-level of abstraction here, and this
code does not yet say “iterate over a container in reverse”, even
though that is its effect.  We will soon discuss ways of expressing
this in our code.

We can also rewrite our `repeat` loop above using a `for` loop.  This
historically is the conventional way to write it, but we will prefer
building an abstraction called `repeat` when we have the tools to do
so.

```cpp
for (auto i = 0; i != n; ++i)
  statement;
```

### Range-based `for` loops

It turns out that iterating over an entire container in order is very
common, so C++ has a special type of `for` loop specifically designed
for this, called a _range-based `for` loop_.  Here, we discard our
iterator variable and just pull out the object from the container each
time.

```cpp
for (auto x : xs)
  statement;
```

This code iterates through the container `xs` in order, makes a copy
of each element and calls it `x`, and then executes its body.  Note
that this means you can't modify the elements inside `xs`.  Mutating
the elements in the container is a very, very compelling use case, and
we will use it to motive a fundamental type of variable in the
language in the next lesson.  For now, I ask that you use a normal
`for` loop if you need to do that.

I haven't yet defined what a `container` is, only given the examples
`vector` and `array`.  For the moment, think of a container as a type
that stores many objects of the same type and which provides some
order we can iterate over them.  We can write our own containers, and
they will work with a range-based `for`.  We will see how in a few
lessons.

### `break` and `continue`

If you want to exit out of a loop body, you can use the `break;`
statement.  If you want to immediately go to the next iteration of the
loop (checking the conditional expression and, in a `for` loop,
executing the update step), you can use the `continue;` statement.

### Loop invariants

Loops are among the hardest things in software to reason about.  They
are a fertile source of bugs—much more fertile than the silly `else`
error we saw when discussing conditional statements.  It is hard to
glance at a loop and know it is correct, because they have low levels
of abstraction.  We will eventually all but abandon loops and replace
them with better abstractions, but for now, we still want to be able
to reason about our code.

As we have said, each loop depends crucially on a change in state.  We
are doing the same thing several times so we can bring about similar
changes in state.  We want to be able to reason about those changes in
state, and to make sure that the loop terminates, but the syntax for a
loop (especially that of `while` loops) does not lend itself to that.

This leads us to the idea of a _loop invariant_.  A loop invariant
describes what we know to be true at each iteration of the loop.
Consider the following loop:

```cpp
for (auto i = 0; i != n; ++i) {
  arr[i] = 0;
```

What do we know to be true about this loop?  Well, we know that, at
any given iteration, for all 𝑖 such that 0 ≤ 𝑖 < `i`, `arr[`𝑖`]` is
zero.  By induction, we can show that after the loop, the entire
container (assuming it is of size `n`) has been zeroed out.  Here, we
have identified our loop invariant, which is a simple one.  Let's put
it in a comment:

```cpp
// Loop invariant: ∀𝑖.(0 ≤ 𝑖 < i) ⇒ arr[𝑖] == 0
for (auto i = 0; i != n; ++i) {
  arr[i] = 0;
```

Here, I have chosen to write it in a formal notation, but you may
prefer to write it as follows:

```cpp
// Loop invariant: All elements before the element at index i are
// zero.
for (auto i = 0; i != n; ++i) {
  arr[i] = 0;
```

Loop invariants can have the side effect of also specifying the intent
of your loop.  While in this example it may be overkill, I encourage
you to specify your loop invariants in comments for all but the most
trivial loops.

__Discussion__: Loops (_iteration_) and recursion are isomorphic: that
is to say, any iterative code can be rewritten as recursive code, and
vice versa.  Explain loop invariants through the lens of
recursion—what is the analog of them in recursive code?  How does
viewing them this way help us write better, clearer code?

Our eventual goal is to almost entirely stop using loops and migrate
to different abstractions.  This will avoid most of the complexities
of loops and make the code easy to understand.

Namespaces
----------

In C++, a _namespace_ is simply a collection of named things.  We
introduced namespaces last lesson, but it is worth coming back to them
in a little more detail, and seeing how we can make our own.  First,
though, let's motivate the idea of namespaces and look at
alternatives.

### Why namespaces?

Imagine that you are writing a program that simulates the forces on a
suspension bridge.  You find this nice library that deals with tensile
forces—let's call it `libforce`, and even provides several nice
presets for different materials.  You start writing your application,
using `libforce`'s `steel_cable` class in your suspension simulation.
You soon discover, though, that the compiler isn't happy with your
user interface code: you wanted to use the standard library's `string`
class to store labels, but `libforce` also defines a `string` class as
a preset material.  The compiler doesn't know which one you meant, so
it gives you an error.  To make it worse, you used the standard
library `vector` class template, but `libforce` defines an internal
`vector` class for dealing with force vectors—one that you aren't even
meant to know about!  It's not simple to fix this, because you can't
modify the standard library or `libforce`.

There are a few ways we can solve this.  In C, the common way to solve
this is to prefix everything with the name of the library.  Thus,
`steel_cable` becomes `force_steel_cable`, `string` becomes
`force_string`, and `vector` becomes `force_vector`.  This
accomplishes our goal of disambiguating for the compiler, but it
doesn't seem very convenient.  Not only do we have to type the library
name all the time (it's less convenient to write), but it also makes
the meaning of the code harder to see (it's less convenient to read).
Compare the following:

```cpp
auto v            = force_vector{ 0, 1, 2 };

auto projx_length = force_dot_product(v, force_x_axis);
auto projx        = force_multiply(v, projx_length);

auto projy_length = force_dot_product(v, force_y_axis);
auto projy        = force_multiply(v, projy_length);

auto projz_length = force_dot_product(v, force_z_axis);
auto projz        = force_multiply(v, projz_length);
```

and

```cpp
auto v            = vector{ 0, 1, 2 };

auto projx_length = dot_product(v, x_axis);
auto projx        = multiply(v, projx_length);

auto projy_length = dot_product(v, y_axis);
auto projy        = multiply(v, projy_length);

auto projz_length = dot_product(v, z_axis);
auto projz        = multiply(v, projz_length);
```

The latter is easier to glance at and see what it's doing, of course,
so we're less likely to introduce errors later in maintaining the
code.  This is because there's less visual noise, and the information
we're presenting is the most salient information.  Unlike a compiler,
we know that we're dealing with the `libforce` library here, so adding
the a `force_` prefix only obscures the useful information.  If we
write a specially optimized `patrick_optimized_dot_product` that uses
the special dot product assembly instruction in SSE4.2, that would be
much easier to notice in the latter code than in the former code,
because there's less unimportant information being presented to us.

We want a solution that gives us both of these benefits.  That is, we
want to solve the original problem, where the compiler doesn't know
which symbol to use, but we also want a solution that lets us preserve
the salient information and not drown out the unexpected information.
The solution in C++ is _namespaces_.  Now, `libforce` can make a
`force` namespace, so we have a `force::vector`, a
`force::dot_product`, and so forth.  We can also make our
`patrick::dot_product`.  Now, we can write the following code:

```cpp
auto v            = force::vector{ 0, 1, 2 };

auto projx_length = force::dot_product(v, x_axis);
auto projx        = force::multiply(v, projx_length);

auto projy_length = force::dot_product(v, y_axis);
auto projy        = force::multiply(v, projy_length);

auto projz_length = force::dot_product(v, z_axis);
auto projz        = force::multiply(v, projz_length);
```

which is no better than our original solution.

The benefit comes when we introduce the `using` _directive_.  We can
rewrite the code with this and get both benefits:

```cpp
using namespace force; // read as "now force is the most salient library"
auto v            = vector{ 0, 1, 2 };

auto projx_length = dot_product(v, x_axis);
auto projx        = multiply(v, projx_length);

auto projy_length = dot_product(v, y_axis);
auto projy        = multiply(v, projy_length);

auto projz_length = dot_product(v, z_axis);
auto projz        = multiply(v, projz_length);
```

Now, if we want to switch to the `patrick::dot_product`, it's very
apparent that something special is going on:

```cpp
using namespace force;
auto v            = vector{ 0, 1, 2 };

auto projx_length = patrick::dot_product(v, x_axis);
auto projx        = multiply(v, projx_length);

auto projy_length = patrick::dot_product(v, y_axis);
auto projy        = multiply(v, projy_length);

auto projz_length = patrick::dot_product(v, z_axis);
auto projz        = multiply(v, projz_length);
```

We've come up with a solution that disambiguates names, hides the
information we usually don't care about, and makes clearer the
information that we care about most often.

It is important to distinguish namespaces from any sort of conception
of a module or a package.  We use namespaces to prevent naming
ambiguities, while modules are something structural and semantic.  A
module might declare names in multiple namespaces, and a single
namespace can span across many modules.

### `using`

As you've already seen, the standard library declares its identifiers
in `namespace std`.  Let's pause for a moment and discuss exactly how
we have been working with the `std` namespace so far before we see how
to make our own namespaces.

By default, as we saw above, if something is in the `std` namespace,
we need to explicitly prefix it with `std::` to use it.

```cpp
#include <iostream>
#include <string>
using namespace std::literals;

// We're explicitly declaring the type here to illustrate namespaces.
// You don't want to do this usually!
auto global_variable = std::string{ "Hello, "s };

int main() {
  auto local_variable = std::string{ "world!"s };
  std::cout << global_variable << local_variable << "\n"s;
}
```

This can be a little bit noisy, so we can use a `using` _directive_ to
allow us to drop the prefix in all code that comes below it.

```cpp
#include <iostream>
#include <string>
using namespace std;                                  // ←
using namespace std::literals;

auto global_variable = string{ "Hello, "s };

int main() {
  auto local_variable = string{ "world!"s };
  cout << global_variable << local_variable << "\n"s;
}
```

A `using` directive scoping rules, too, so we can limit it to a
smaller scope.

```cpp
#include <iostream>
#include <string>
using namespace std::literals;

auto global_variable = std::string{ "Hello, "s };     // ←

int main() {
  using namespace std;                                // ←

  auto local_variable = string{ "world!"s };
  cout << global_variable << local_variable << "\n"s;
}
```

If we need to be more precise, we can use a `using` _declaration_ to
explicitly pick out name inside a namespace.

```cpp
#include <iostream>
#include <string>
using namespace std::literals;

auto global_variable = std::string{ "Hello, "s };

int main() {
  using std::string;                                  // ←

  auto local_variable = string{ "world!"s };
  std::cout << global_variable
            << local_variable
            << "\n"s;
}
```

You'll see the special `using namespace std::literals;` line in each
of those examples.  As we discussed last time, you need this line to
use the `std::string` literal syntax.  This is because that syntax is
not built-in, but is actually declared inside a namespace.  There is
no way to explicitly qualify the literal syntax with
`std::literals::`, so we need to add the `using` directive to use it
at all.  (Later on in this course, we'll see how we can make our own
literals like this.)

### Declaring namespaces

Now we know how to use a namespace, but how to do we declare our own?
If you were to look inside the standard library headers, you'd find a
block like this:

```cpp
namespace std {
// important stuff here
}
```

This code declares everything inside the block to be in the namespace
`std`.  If you remember from last lesson that `#include` simply copies
and pastes the header files into your source file before compilation,
we get something that looks somewhat like this being fed into the
compiler:

```cpp
/* #include <iostream> */
namespace std {
// iostreams stuff
}

/* #include <string> */
namespace std {
// string stuff
namespace literals {
// ""s syntax stuff
}
}

using namespace std::literals;

auto global_variable = std::string{ "Hello, "s };

int main() {
  auto local_variable = std::string{ "world!"s };
  std::cout << global_variable << local_variable << "\n"s;
}
```

Here we see the syntax for declaring namespaces, as well as some of
the important properties of namespaces.

  - The syntax for declaring a namespace is `namespace name { … }`.

  - You can declare a namespace at file scope or within another
    namespace (at _namespace_ scope).  This means namespaces can be
    nested.

  - Namespaces can be _reopened_, and more names can be added to
    them.  You can even add things to other namespaces, although it is
    bad practice to do so unless the library author explicitly allows
    you to.  You are, in general, not allowed to add anything to
    `std`; doing so makes your program ill-formed.

### Naming conflicts

When we use `using` directives, we can start to run into naming
conflicts, where two things things that have been brought in from
different namespaces have the same name, so the compiler can't tell
which one we intended to use.  There are a couple ways to deal with
this:

  1. Add an explicit namespace prefix to disambiguate.  If the thing
     you want to use isn't in a namespace, but is actually in the
     global scope, prefix it with just `::`.

     ```cpp
     namespace foo { struct my_type {}; }
     namespace bar { struct my_type {}; }

     using namespace foo;
     using namespace bar;

     auto variable = foo::my_type{};
     ```

  2. Use a `using` declaration to pick out which one you want to use.
     This overrides any earlier `using` directives or declarations.

     ```cpp
     namespace foo { struct my_type {}; }
     namespace bar { struct my_type {}; }

     using namespace foo;
     using namespace bar;

     using foo::my_type;
     auto variable = my_type{};
     ```

Generally, having many `using` directives or many `using` declarations
is considered bad style.  Limit it to one or two `using` directives,
and only for the most salient namespaces in your code.  Also, it's
often a good idea to limit `using` directives and declarations to a
small scope, like a function scope, unless the namespace truly is
pervasive throughout the file.  `std` is an example that tends to be
pervasive, but it is a judgment call.

__Discussion__: It's a bad idea to have `using` directives in a header
file.  Based on what we know about `using` directives and `#include`,
why is this?  Revisit your discussion last time of the question, “how
would you design a modules system for C++?”  What important property
can we see here that do header files lack?

Exercises
---------

Complete the following exercises using _only_ the features discussed
in this lesson.  Don't make a new `CMakeLists.txt` for each one;
instead, just add the executables and tests to your existing
`CMakeLists.txt`.  Make sure you keep this file readable.

__Exercise__: Write a program that reads from standard in (`cin`) and
prints exactly the same thing to standard out (`cout`).  This is
similar to the UNIX utility `cat --`.

To do this, you will need to know that `cin` is convertible to a
`bool` that represents whether the last input operation was
successful.  In our case, `false` means that the input has ended, and
we should terminate.

__Exercise__: Write a program that reads from standard in and prints
the number of words it reads to standard out.

__Exercise__: Write a program that reads a positive number in trinary
from standard in and prints the number in decimal to standard out.

__Exercise__: Write a program that implements the following
stack-based language:

```
(each token is separated by whitespace)

NUM     ::= (the format understood by std::stoi)
OP      ::= + | - | * | /
PRINT   ::= print
EXPR    ::= NUM | EXPR PRINT | EXPR EXPR OP
PROGRAM ::= EXPR
```

Initially, the stack starts out empty.  Each time an `EXPR` is
encountered, it should be evaluated, and its result should be placed
on the stack.  Evaluating `NUM` should result in the number.
Evaluating `EXPR PRINT` results in the result of evaluating `EXPR`,
but as a side effect should print a line to `cout` that shows the
current contents of the stack.  Evaluating `EXPR EXPR OP` results in
applying the operation to both inputs, popping the inputs off in the
process.

For malformed input, write an error message to `cerr` (just like
`cout`, but corresponds to standard error) and exit with
`EXIT_FAILURE`).

__Exercise__: Write a program that reads a positive number from
standard in and prints out the unique prime factors (not the prime
factorization!) of that number.

Make sure you write test cases for each of these exercises.

Now that we are writing larger programs, we want to turn two things on
in our compiler flags: we want to let the compiler _optimize_ the code
to make it faster and use less memory if it can, and we want the
compiler to give _warnings_ when we write code that _probably_ has an
error, even though it may be valid and we could have meant it.  To do
this, please add the following lines to your `CMakeLists.txt` file
before any `add_executable` commands:

```cmake
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # General warning flags
  add_compile_options(-Werror -Wall -Wextra -pedantic)
  # Turn on optimization
  add_compile_options(-O3 -fvectorize -fsized-deallocation)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # General warning flags
  add_compile_options(-Werror -Wall -Wextra -pedantic -pedantic-errors)
  # Turn on additional warnings
  add_compile_options(-Wcast-align -Wcast-qual -Wconversion -Wformat
                      -Wlogical-op -Wno-deprecated-declarations
                      -Wreturn-local-addr -Wshadow -Wsign-conversion
                      -Wswitch-enum -Wundef -Wuninitialized -Wuseless-cast
                      -Wzero-as-null-pointer-constant)
  # Turn on optimization
  add_compile_options(-O3 -flto -fsized-deallocation)
endif()
```

Additional Reading
------------------

Please read the following article by Herb Sutter and Jim Hyslop:
[_Using Me_][using-me].  Although written in a light-hearted way, pay
attention to what it's getting at.

[using-me]: http://www.drdobbs.com/using-me/184401782
