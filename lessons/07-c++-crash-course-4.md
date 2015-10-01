C++ Crash Course 4
==================

If you recall from our discussion of types in the previous lesson,
types are a mapping from a representation in the computer to an entity
that representation represents.  Because entities have operations you
can perform on them, we can state this definition another way: a type
is a representation and set of operations you can perform on that
representation.  No matter how we phrase this, though, every type
necessarily has a representation, and there is no way we can get
around this or ignore this.

As such, every function that we write is dependent on the
representation of the types it accepts and returns.  The functions
represent operations, but are specifically tuned to a specific
representation.  This does just allow both us and the compiler to
generate very efficient code, but it also allows us to reason about
the performance of our functions.  But what about operations that
don't really depend on the operation—operations which we can describe
in the abstract, regardless of the exact representation?  These
operations may depend on properties of the type, properties of other
operations, or properties of the representation, but the exact
representation isn't entrely relevant.  Consider the following
function:

```cpp
auto min(int x, int y) {
  if (y > x)   return y;
  else         return x;
}
```

Would this function still work if we replace `int` with `double` or
`std::string`?  We can see that the exact representation is not
important to this function; we care about properties of the
`operator>` we use.  We are forced to tightly couple our function to
the representation of our type.  We could easily implement `min_int`,
`min_double`, or `min_string` functions that have the same function
bodies, but there are certainly an unbounded number of other types `T`
that you don't know about for which this implementation would work
for.  We are artificially limiting the use of our code.

Like `min` above, a lot of functions really care only about the
operations you can perform on types and properties of those
operations, not the representation.  We need an abstraction that takes
types with sets of operations and gives us back some other
"function"-like thing; using this abstraction, we would be able to
write code that doesn't depend on the representations of the types it
uses.  We're left with a contradiction: representation is fundamental,
so we need this "function"-like thing to be a real function that uses
real types with real, concrete representations.  Essentially, our
abstraction has to be a mapping that takes any type that has certain
operations and stamps out a real function using whatever type we give
it.

Now, consider types we've seen so far like `boost::optional<int>` and
`std::array<bool, 5>`.  These are types, and types have a
representation.  But what is the representation of `array<T, 5>`?  It
depends on the representation of `T`.  While we could write a
different `array` for each type we want to store in an array—as,
perhaps `array_int`, `array_double`, or `array_string`—this would be a
lot of duplication, and we run into the same problem as above: what if
someone wants to use our code with their type `T` that we didn't know
about?  Again, we're tightly coupling an abstraction to the
representation of some type; here though, that abstraction we're
tightly coupling is also a type.  It seems that we want an abstraction
that takes a types and gives us back a "type"-like thing.  By the same
reasoning we used above, this abstraction needs to be a mapping that
types any type that has certain properties and stamps out at a real
type using whatever type we give it.  In `array`'s case, it looks like
we also want to be able to parameterize our abstraction by a
compile-time value, here representing the number of elements the
`array` can hold.

__Discussion__: How would you implement `min` or `array` in other
languages?  What language features do they use to accomplish the same
goal?  Think about the tradeoffs in these designs: do they work at
compile-time or at run-time?  Do they couple the input types to the
`min` or `array`?  Would a user of `min` or `array` necessarily have
to modify their type to make it work with your code?

Templates
---------

What we really want is a way to stamp out a new type or function (or,
in fact, variable) based on a list of type or compile-time value
parameters.  In C++, this is accomplished at compile-time using an
abstraction called a _template_.  Let's first take a look at an
example of a template to get a feel for the syntax.  The following
function template allows us to stamp out `min` functions at
compile-time:

```cpp
template <typename T>     // we take in a type T, and map it to the following
auto min(T a, T b) {
  if (b > a)    return b;
  else          return a;
}
```

Now, we can call `min(4, 5)`, `min("hello"s, "world"s)`, or `min(0.2,
1.4)`—the compiler will deduce the type `T` to use from the types of
the function arguments, and it will then stamp out a new function that
takes in the specific arguments that it will then call.  Notice how
this has no runtime overhead.  It's as if we had written each function
ourselves.  If we had a reason to manually specify what type `T` we
want, or if the compiler could not deduce `T` based on the calling
expression, we can override the compiler's deduction and explicitly
call `min<int>(a, b)`.

So far, we have a shorthand, which is nice, but not groundbreaking.

__Discussion__: Write a function template for which the compiler
cannot deduce one of the template parameter types.

__Discussion__: Try to call `min` on two `std::complex<double>`
numbers.  What happens?  When does this happen?

__Reading__: At this point, please reread section 3.4 of
[_A Tour of C++_][tour-3].

[tour-3]: https://isocpp.org/images/uploads/3-Tour-Abstr.pdf

### Template Specialization (Covered in Lesson)

Concepts
--------

As we said before, a template can't just map any type to a type,
function, or variable.  A template can only map types that satisfy
certain properties.  In our above `min` example, we at least need the
type `T` to define an `operator>`.  Such a set of requirements is
called a _concept_.  A concept is a predicate over types that is true
when the type satisfies properties our template requires.  Every
template requires that its template parameters model certain concepts.
Very recently, we have gotten syntax to express these in the language.
Your compiler very likely does not support this yet, and it may change
in the future, but we will give the syntax anyway.

Concepts have two different types of requirements: _constraints_,
which are syntactic requirements on the type, and _axioms_, which are
semantic requirements on the type.  We can directly express
_constraints_ in the language, but we need to be aware of axioms, as
they are equally important—just harder to write and harder to check.

Let's consider again our `min<T>` function template.  Syntactically,
it requires that the expression `b < a` be defined for `a`,`b`∈`T` and
have a type that can be treated as (converted to) `bool`.  Let's call
this concept `GreaterThanComparable`:

```cpp
template <typename T>
concept bool GreaterThanComparable =
  requires (T a, T b) {              // for all a and b of type T,
    { a > b } -> bool;               // expr "a > b" must be of type bool
  };
```

This code expresses what we just described in prose.  Now, let's
constrain our `min` function template with `GreaterThanComparable`:

```cpp
template <typename T>
  requires (GreaterThanComparable<T>) // can be arbitrarily complex expression
auto min(T a, T b) {
  if (b > a)    return b;
  else          return a;
}

// or, for simple expressions:
template <GreaterThanComparable T>
auto min(T a, T b) {
  if (b > a)    return b;
  else          return a;
}
```

Now, we are expressing the requirements in code, and the compiler can
perform checking at the call-site to make sure the concepts are
satisfied.

__Discussion__: We have seen a system for defining constraints, but
not axioms.  Why are axioms necessary?  Design a type `T` which
satisfies the constraints of `GreaterThanComparable`, and for which
`min<T>` will compile, will not give the desired results.  What are
the axioms on the concept `GreaterThanComparable`?

### Concepts Outside of Templates (Discussed in Lesson)

### Weak Coupling (Discussed in Lesson)

Notice how our type `T` does not need to know about
`GreaterThanComparable`, and vice versa.

Generic Programming
-------------------

As we have seen, we have a way to abstract away from the
representation of types (that is, templates) and a way to describe the
interface of types we are interested in (concepts).  When taken
together, templates and concepts allow us to write programs in a style
that few other languages allow.  This paradigm is called _generic
programming_, because it allows us to define types and operations that
work across an unbounded number of input types, generically.

Generic programming allows code:

  - to express our intent more clearly,
  - to be written once and work with code it was not designed to,
    without modification or duplication,
  - to be as efficient as possible, so that writing each operation by
    hand should be no more efficient,
  - and to be safe, so that correctness can be verified at
    compile-time.

__Reading__: For background before continuing on, please read
[Dehnert and Stepanov (2000)][dest98].  This reading is very
important, although it uses older syntax we haven't covered.  Do not
just skim this.

[dest98]: http://www.stepanovpapers.com/DeSt98.pdf

__Reading__: As supplemental reading (after the lesson), please read
[Notes on Programming, Lectures 1 and 2][notes].  If you have time,
you can read further.

[notes]: http://www.stepanovpapers.com/notes.pdf

### Generic Programming Via `sort()` (Discussed in Lesson)

Templates as Type-level Functions
---------------------------------

(Will be added later)
