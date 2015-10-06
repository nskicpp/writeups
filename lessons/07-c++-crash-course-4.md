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
  if (y < x)   return y;
  else         return x;
}
```

Would this function still work if we replace `int` with `double` or
`std::string`?  We can see that the exact representation is not
important to this function; we care about properties of the
`operator<` we use.  We are forced to tightly couple our function to
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
  if (b < a)    return b;
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

### Template Specialization

The version for `min` is pretty generic and looks pretty efficient.
For the majority of cases, this will work very well, because of its
genericity.  If you were to write the same function by hand for each
type `T`, it would likely look very, very similar.  This might be
surprising: we have no extra overhead over hand-writing all the
versions we need ourselves.

In most cases, that is.  Imagine a type for which it is easy to check
for equality, but hard to check for ordering.  It may be that for this
type—let's call it `mytype`—a more efficient, hand-coded
implementation may look like this:

```cpp
auto min_mytype(mytype a, mytype b) {
  if (a != b && b < a)     return b;    // short-circuits before <
  else                     return a;
}
```

By short-circuiting, we can make sure that we avoid the costly
operation in the case that `a` and `b` are equal, a check which has
have neglible cost.  Because we know certain specialized properties of
our type `mytype`, we can make a specialized version of our algorithm
which is more efficient in certain cases.  Ideally, we would want our
templated version to be as fast, but there is no way we could do that
generically, because we know extra, specialized information about the
type that makes this true.

A _template specialization_ solves that problem.  We can maintain our
generic version of `min<T>`, allowing us to still work with any type,
but we can apply optimizations as `min<my_type>`.  To the user, there
is no difference in the template usage: they just call `min(my_type{},
my_type{})`, and it works, just as before.  Instead of stamping out a
generic version of `min`, though, the compiler uses our optimized
version.  The syntax for doing so is as follows:

```cpp
// Normal, generic version (as above)
template <typename T>
auto min(T a, T b) {
  if (b < a)    return b;
  else          return a;
}

// …

// Later on, elsewhere, the specialization
template <>
auto min<my_type>(my_type a, my_type b) {
  if (a != b && b < a)     return b;  // short-circuits before <
  else                     return a;
}
```

The two parts of any specialization are the empty template parameters
(`template <>`) and the specialization type after the name of the
function, type, or variable (`min<mytype>`).  For a template that
takes multiple parameters, you just list them within the
specialization list after the function, type, or variable name.

We now have a way to conditionally select a more optimal, hand-written
implementation when we have extra information.  The user doesn't need
to know the difference; it's completely transparent to them.  Their
generic code can use our `min` function and our optimization on our
type, and they don't even need to know that our type exists.  We will
come back to this idea in a bit, as it allows us to write software
that is optimally efficient for all possible types that satisfy our
required properties.

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
this concept `LessThanComparable`:

```cpp
template <typename T>
concept bool LessThanComparable =
  requires (T a, T b) {              // for all a and b of type T,
    { a < b } -> bool;               // expr "a < b" must be of type bool
  };
```

This code expresses what we just described in prose.  Now, let's
constrain our `min` function template with `LessThanComparable`:

```cpp
template <typename T>
  requires (LessThanComparable<T>) // can be arbitrarily complex expression
auto min(T a, T b) {
  if (b < a)    return b;
  else          return a;
}

// or, for simple expressions:
template <LessThanComparable T>
auto min(T a, T b) {
  if (b < a)    return b;
  else          return a;
}
```

Now, we are expressing the requirements in code, and the compiler can
perform checking at the call-site to make sure the concepts are
satisfied.

__Discussion__: We have seen a system for defining constraints, but
not axioms.  Why are axioms necessary?  Design a type `T` which
satisfies the constraints of `LessThanComparable`, and for which
`min<T>` will compile, will not give the desired results.  What are
the axioms on the concept `LessThanComparable`?

### Weak Coupling

The combination of templates and concepts provides us with a powerful
basis for designing type-safe and efficient software.  From other
languages, though, you may be aware that there are other techniques
for doing this.  We can use Java-style interfaces or mixins, which act
as concepts do to give us safety, for example; several (even
non-Object Oriented) languages provide abstractions that are very
similar, each with slightly different benefits.

It might be tempting to think of concepts as a sort of OO interface,
but this analogy misses one of the most important aspects of concepts
and templates: concepts have very loose coupling to the types that
implement them.  This observation may seem unimportant, but it is
going to lead us to a paradigm that will shape the way we right C++
code for the rest of this course.

Think about our `min` function again.  How would we implement this in
Java, say?  We could write our method body very similarly, maybe
calling a `lessThan` method on our objects.  For this to work, though,
we need to take in a parameter type that supports a `lessThan` method.
We are forced to make an interface or base class, maybe called
`LessThanComparable`, which provides this `lessThan` method.  Now, we
just change our argument type to only accept arguments that can
inherit from or implement `LessThanComparable`.  So far, this looks a
lot like our concepts example.

The difference, though, comes in what we can accept as input to our
`min` method.  In Java, our method only works if the implementer of
the class implements our `LessThanComparable` interface.  Put another
way, the implementer of a type has to know about `LessThanComparable`
for that type to work with our code.  A user who does not have control
over the type cannot use `min` with it.  While the user can get around
this (the user can make their own class that wraps that type and that
implements `LessThanComparable`), doing so requires a lot of extra
code, and it is not the path of least resistance—just write your own
min code inline in the method body.  We are faced with the choice
between rewriting the routine (defeating its point) or writing a lot
of scaffolding to make it work.  Even if the library implementer made
their own method `lessThan`, but didn't implement
`LessThanComparable`, we would be unable to use it easily.

This problem stems from the fact that there is a coupling between the
type and its interface: the type implementer has to explicitly state
that they are fulfilling the contract.  With concepts, as we have
seen, this is not the case: types inherently fulfill concepts, and
this is checked by the compiler.  Suddenly, any type that implements
`operator<` can use our `min` function—and `operator<` is a very
commonly implemented routine.  Our function is widely general now.
Certainly we will still need scaffolding if they implement it some
other way, giving it a name like `lessThan`.  Then, though, we just
need to make an `operator<` implementation for the type (which we
don't need to be the implementer to do) that implements it in terms of
`lessThan`.  Now suddenly, our type works for anything that provides
`lessThan` or `operator<`.  Notice how we haven't modified either our
`min` function or our type's implementation in any way.  This allows
our `min` function to work with types we've never heard about and
which have never heard about us, with minimal effort on the user part.
Soon, we will see an example of this that allows us to define
optimally efficient algorithms on any sort of data stucture, even ones
that haven't been invented yet, without reimplementing the algorithm
on each data structure specifically.

### Concepts Outside of Templates

As we have seen, concepts are predicates on the interface of a type,
instead of a representation.  We motivated templates primarily with
the desire to program to an interface and not to a representation,
while not encapsulating that representation from us.  Concepts have
allowed us to specify precisely what interface we are looking for in
code.  This has several benefits, allowing us to express to the reader
our intent and allowing the compiler to check that the user follows
that intent.  It turns out, though, that there are other places where
we don't care so much about the representation, and care more about
the interface.

We've seen type deduction in C++ in two places: in templates, allowing
the template argument types to be deduced, and with `auto` in variable
declarations.  It turns out that concepts can be used to constrain
both types of type deduction.  Recall our syntax for defining
variables:

```cpp
auto var_name = expr;
```

The type of the variable `var_name` is deduced from the type of
`expr`.  In your experience with C, you may be familiar with the
syntax below, which I have encouraged you not to use:

```cpp
type_name var_name = expr;
```

This inherited syntax has one important benefit over the `auto`
syntax: a reader of the code knows exactly what operations they can
perform with `var_name`; with `auto`, that is much, much less easy to
see.  There are several problems with this inherited syntax, though,
the most important of which is what happens when `type_name` and the
type of `expr` don't match.  C is perfectly happy to allow this, and
while C++ is stricter, there are certain conversions that this syntax
allows, because of our backwards compatibility with C.  For example,

```cpp
int zero = 0.5;  // zero == 0 !
```

The warning settings we use in this course will alert us of this, so
it's not that bad, right?  Especially since we can see the bug, and
hopefully catch it easily, right?

It turns out it isn't as easy as it may seem.  Consider the following
code:

```cpp
int x = get_coordinate();
```

Does this suffer from the problem above?  We need to know the type of
`get_coordinate` to know.  This is an annoyance, but we can still
figure this out.  What, then, if the return type changes?  Suddenly,
this code is at best non efficient (if it is safe to build an `int`
from the new return value, because the new return value can hold all
values of `int`) and at worst not correct (if the new return type has
a greater range than `int`).  Maybe the compiler will warn us, but
then we need to go through all our code that uses `get_coordinate`
just to change the type.

We're running up against a mismatch in our expectations and the real
meaning of the syntax: we want to use the `int` to express in code
what the type we are expecting is, but the inherited syntax really
means "This variable _must_ be an `int`."  In other words, the syntax
coerces the type to an `int`, and doesn't just label that it is an
`int`.  This is at the root of our problems above.  If we use the
inherited syntax, this isn't very clear, though.

As such, in this course, we follow the prevailing
[Almost Always Auto style][aaa-style].  This style is not universally
accepted, and we will later look at some of the arguments against
it—we need a stronger basis to be able to evaluate the claims, though.
We will after a while relax this constraint, but only in very few
places to draw a very important distinction that wouldn't be possible
otherwise.  This is mentioned briefly in Sutter article, but he leaves
the problem as simply syntactic, rather semantic, as we will see it
really is.  When we really do want to coerce the type, we use the
following syntax:

```cpp
// When it's safe (not-lossy) to make expr into type,
auto var_name = type{ expr };
// Or when the compiler can't determine that it's safe, but we have
// extra information that allows us to know.  Put a comment and say
// why we know that!
auto var_name = static_cast<type>(expr);
```

This makes it clear and explicit that something special is going on.

How do concepts enter into this, though?  Let's make a second attempt
at solving this problem, one that doesn't involve precisely specifying
the type.  It turns out, we usually don't care about the actual
representation of the type.  We care more about the operations we can
perform on it.  If we expand the code we wrote above,

```cpp
auto x = get_coordinate();
return (x + 1) * 5;            // shift and then scale
```

we can see that this code depends on the type `T` that `auto` gets
deduced to having an `operator*` and an `operator+`.  We seem to have
a concept—`T` has to be `Numeric`—that we care about.  Whether or not
`T` is an `int` or a `double` (or a more exotic type, like a
`big_int`) doesn't really matter here.  We want to annotate our
intent, which is that we expect the deduced type to model the concept
`Numeric`.

```cpp
Numeric x = get_coordinate();
return (x + 1) * 5;
```

Now, we've satisfied both desires: we've expressed useful information
about the variable and its usage, and we've also avoided the
maintainability problem of overly strictly specifying the type (as
well as the other problems with the old C syntax).  Furthermore, we
now have the additional benefit of breakages really meaning something:
if `get_coordinate`'s type changes in a way that violates the concept
`Numeric`, we know that means we've made an assumption in our code
that is not true any longer.  We _have_ to change our code to fix
this.  This leads us to our course coding style:

    Declare variables with their salient concepts when possible.
    Otherwise, or if there are no salient concepts (if, for example,
    we are simply propagating a value), use `auto`.  If you care about
    the exact representation, use the temporary construction syntax
    `type { … }` or a cast `static_cast<type>(…)` to make this easily
    noticeable.

Always beware of settling for a solution that doesn't really solve the
problem—try to think about the real fundamental problem you're facing,
which may not be immediately apparent.  As we saw, we were able to use
this to minimize the drawbacks of both our initial solutions by
considering what our real goals were.  When it is not possible within
a language to solve the fundamental problem, you may have to settle
for a solution: in that case, be conscious of this and be flexible and
willing to switch to a better solution if you find one.

__Discussion__: What are the drawbacks of this approach?  There are
the syntactic drawbacks presented in Sutter's article, but try to
consider a more abstract view.  When does the above reasoning fail?
Could we fix this by modifying the language while still keeping the
benefits that we get from this style and concepts more generally?

[aaa-style]: http://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/

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

### Generic Programming Via `sort()`

As an example of this, let's consider a function `sort` that sorts a
container type `C`, shown below.  Take a moment to convince yourself
that this code is correct.

```cpp
template <typename C>
void sort(C& container) {
  // Selection sort algorithm

  // Loop invariant: ∀𝒾 < i, container[𝒾] is in its final, sorted position.
  for (auto i = 0; i < size(container); ++i) {
    // Find index of maximum element in range [i,size(container))
    // Loop invariant: ∀𝒾 s.t. i < 𝒾 < j, container[max_index] >= container[𝒾].
    auto max_index = i;
    for (auto j = i + 1; j < size(container); ++j) {
      if (container[max_index] < container[j])
        max_index = j;
    }
    swap(container[i], container[max_index]);
  }
}
```

Because this code is a function template, we know `C` must model some
concept for the generated functions to be functionally correct.  Let's
call this concept `Sortable`.  What are the constraints and axioms of
this concept?  To perform this sort of analysis, we methodically walk
through the function template and look at all uses of the type `C`.

  1. `size(container)`: We need a function called `size` defined that
     can be called as `size(C{})`, the return value of which needs to
     be `Integral`.  We expect that accessing any element from index 0
     up to (though excluding) index `size(container)` be well-defined
     behavior, and that there be no indexes outside this range for
     which access is well-defined (that is, if we sort the entire
     range [0,`size(container)`), we will sort the entire container).
     
  2. `container[max_index]`: We need a function called `operator[]`
     defined that can be called as `container[int{}]`, the return
     value of which we expect to be some type `T`.  We expect that
     there be a well-defined mapping between an index 𝒾 and a `T` 𝓉;
     accessing `container[𝒾]` should always give the same 𝓉.

  3. `container[max_index] < container[j]`: We need a predicate called
     `operator<` defined that can be called as `T{} < T{}`.  We expect
     that this predicate form a strict weak ordering over all values
     of type `T` that appear in the container.

  4. `swap(container[i], container[max_index])`: We need a function
     called `swap` defined that can be called as `swap(𝓉₁, 𝓉₂)`, where
     `𝓉₁` and `𝓉₂` are of type `T`.  We expect that, if 𝓉₁ and 𝓉₂
     represent the original two values passed into `swap` and 𝓉₁′ and
     𝓉₂′ represent the new values after the call to `swap`, 𝓉₁′ and 𝓉₂
     should be the same value, and 𝓉₂′ and 𝓉₁ should be the same
     value.

We can encode the constraints into a C++ concept:

```cpp
template <typename C>
concept bool Sortable =
  requires (C c, Integral i, Integral j) {
    size(c)            -> Integral;     // 1
    c[i];                               // 2
    c[i] < c[j]        -> bool;         // 3
    swap(c[i], c[j]);                   // 4
  };
```

As long as some type `T` models our concept `Sortable`, our `sort`
function should be correct.  This is what I call a _minimal
implementation concept_.  There is a problem, here, though.  If
someone were to implement `sort` from scratch, would they come up with
the same concept?  They could potentially use other functions, such as
`operator>` or `operator<=`, if they don't want a stable sort.  These
are perfectly acceptable implementations, even though they don't model
our concept `Sortable`.  We've introduced a subtle form of coupling
here: `Sortable` only works with our implementation, not with any
other implementations.  What we really want is a concept that
describes some general behavior, not that limits implementations.  We
would want to expand our concept to capture the idea that we want a
strict, weak ordering: this includes `operator<`, `operator>`,
`operator==`, `operator!=`, `operator<=`, and `operator>=`, with
axioms that relate them correctly.  Our goal in creating concepts is
to capture useful groups of related traits, not to create the minimum
possible syntactic representation of our implementation.  `Sortable`
actually provides a good example of this: it doesn't make sense to
have a type that implements `operator<` but not other operators.  Such
types should not model `Sortable`.

Templates as Type-level Functions
---------------------------------

Our initial description of templates should be evocative of a
mathematical concept you're all familiar with.  We initially described
templates as a mapping from a set of types to either a type, a
function, or a variable.  This idea of a mapping is very similar to a
mathematical function: we're defining a morphism between the types and
other abstractions.  This is, incidentally, the same fundamental idea
that motivates pure functional programming, namely that all
computation can be described using maps from one set to another.

Mathematical functions don't themselves imply computation, though.
For real, Turing-complete computation, we have the following rough
requirements:

  1. We need a memory in which to store the data on which we're
     computing.  Roughly, this may correspond to a variable, a
     binding, or a function parameter.

  2. We need unbounded recursion.  We need to be able to write
     functions which return no value and never terminate.  Imperative
     looping constructions, recursive function calls, and the
     fixed-point combinator are ways we can build this.

  3. We need conditional branching.  We need to be able to perform
     different computations depending on the result of a previous
     computation, as in pattern matching or if statements.

So far, we've seen that templates can be recursive, and that template
specialization allows us to yield different results depending on the
input.  Furthermore, we can create bindings for types using our type
alias syntax `using T = U;`.  If we consider our "values" rather as
types (that is, if we look at our computation as _type-level
computation_), we can see that templates allow us to perform arbitrary
computation on types, at compile-time.  This is an advanced technique
known as _template metaprogramming_.

This may be surprising, so let's look at several simple examples of
this.  Here, the point is not to get lost in the (very verbose and
opaque) syntax, but rather to understand what's going on in the
abstract.  This is just to show you that this technique is possible,
and that it can sometimes be useful.  First, let's start with a simple
identity function, `id x = x`, at the type level.

```cpp
template <typename T>  // This is our argument T
struct id {            // This is the name of our metafunction, id
  using type = T;      // type is the return value of id
};
```

Through all the syntax, we can see what's going on: we define our
output type `type` to be the input type `T`.  When we call this
metafunction, we need to reach into our template and pull out that
type `type`:

```cpp
using result_type = typename id<input_type>::type;
```

The above code instantiates the `id` template with a template
parameter of `input_type` (thus performing the computation), and
reaches inside to get the `type` type alias.

But let's try something more complex:

```cpp
template <typename T>
struct bottom { using type = typename bottom<T>::type; };
```

This metafunction recursively instantiates itself: if we try to call
it, the compiler will eventually give up (in the same way we'd get a
stack overflow in value-level computation).

At this point, we have a memory and a recursive loop, although neither
are terribly useful.  Let's build a metafunction that classifies
whether a type is `void` or not.  Here, we don't want to return a
type, but rather a `bool` that is known at compile time.  We thus need
to do pattern matching at compile time on the type, as below:

```cpp
// Most types are not `void`, so our default is false.
template <typename T>
struct is_void { static constexpr bool value = false; };

// Make a specialization for when `T` really is `void`
template <>
struct is_void<void> { static constexpr bool value = true; };
```

This may seem unnecessary, but we have, in fact, already seen a case
where it can be used.  Recall how `std::tuple<Ts...>` does not have a
well-defined layout according to the standard: it's up to the
implementation to decide how to order the elements of a tuple.  We
discussed how ordering the types in memory in an order that differs
from their order in the template parameter list can give substantial
savings in memory by avoiding the need for additional padding between
members.  At compile-time, we need to sort the type list according to
their size: we need recursion, storage, and conditional branching to
do this.  A good implementation of the standard library may use this
computation internally at compile time to minimize the size of any
possible tuple—it's an implementation detail that you don't have to
worry about as the user of the abstraction.
