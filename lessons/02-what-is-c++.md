What is C++?
============

We will start our course not with a discussion of actual C++ code, but
with a rather philosophical look at the language itself.  What is C++,
from a language design point of view?  Where and why should we use it?
What things should be we keeping in mind while we are programming C++?

__These are important and fundamental concepts that you will be
internalizing throughout this course.  I present them early to expose
you to them, but we will be revisiting them often and from many
different angles.  Please read this lesson carefully, maybe several
times.  This is important stuff.__

C++ is a programming language that supports multiple programming
paradigms and aims to provide both _efficient and expressive
abstractive mechanisms_ and _access to the full capabilities of the
underlying system_.  These are the two primary goals of C++, and we
will see that almost every language feature in C++ is designed to
facilitate them.  At the same time, we will see that these two goals
are sometimes in conflict, and there are many design compromises
between them.  We will see this in our discussion of C++ language
features, standard library constructs, and code that we design
ourselves.  Let's go deeper into these two ideas and provide a
framework we can use to understand what we'll see in C++ later.

Building Abstractions
---------------------

Our primary and best means of writing software that is easy to
understand, and thus to reason about, is through building
abstractions.  We will be discussing this in much more detail in our
next unit, but to frame the discussion here, we need to define what we
mean by an abstraction.  Put simply, an abstraction is something in
our software that we can give a name to that allows us to reason about
our software at a higher level.  This definition is intentionally
vague, because abstractions come in many sorts.  We will be later
talking about other desirable properties of abstractions and looking
at specific types of abstractions, but realizing that things as common
as variables or loops are abstractions allows us to discuss the
similarities and differences between them and think critically about
their advantages and disadvantages.  Our goal, of course, is to
maximize the former and minimize the latter.  My hope is that,
throughout this course, you will see that finding the right set of
abstractions for your application, library, or even language is the
best way of creating clean, easy-to-understand, maintainable, and even
efficient code—better than comments, micro-optimizations, or strange
code formatting tricks.

When we build abstractions, we are trying to organize similar things
together, and separate different things.  Thus, we have to look for
similarities and differences.  We may not always be conscious of this,
though, so let's try to think about it more deeply.  Let's first look
at algebra, where we'll find many useful abstractions.  Specifically,
let's look at a structure known as an _additive monoid_.  An additive
monoid is a set, a binary associative, commutative operation that
takes two elements of the set and gives a third, and an identity
element for the operation in the set (an element e∈S such that
∀𝑥∈S.op(𝑥,e)=𝑥).  This description is somewhat abstract, but if we
apply some more useful names to what we've defined here, we can see
that we have just defined our intuition as to what addition is.  Let's
conveniently relabel our operation to be + and our identity element to
be 0: now, all we're saying is that 𝑥+0=0+𝑥=𝑥.  We've captured
something generic about addition, and we can now apply this to
different domains.  Two examples of additive monoids are 𝐑≥0 and
matrices with natural number coefficients.  (Now, does the notation
"abc" + "def" for string concatenation make sense?  That is, do
strings with concatenation as a binary operation model additive
monoid?  We will see why this is not just a purely theoretical
concern and turns out to be a problem later.  What we've just seen is
called a _concept_, which we'll discuss below.)

Let's look at an example of abstracting that you may find easier to
understand, but is more subtle.  Consider the following psuedocode:

```cpp
for(i = 0; i < n; ++i)
  sum += A[i];
```

Here, we are accumulating over an array `A`.  Let's consider two
things here: `+` and `i`.  What are the things we are assuming here,
and what nice properties can we get out of this?

Concerning `+`: as we've seen, `+` is an operation on an additive
monoid.  We're probably, then, going to assume it's associative,
commutative, and has an identity element.  But what does that get us?

  - For an associative operation, we get _parallelizability_—the order
    we perform the operations doesn't matter, so we can farm this
    sequential operation out to multiple threads, cores, or nodes.
    “Associative” is a good indicator of “embarrassingly parallel.”

  - For a commutative operation, we get _permutability_—which side of
    the operation each value appears on doesn't matter.  Let's say one
    of the elements of `A` represents +∞.  In this case, the result is
    going to be +∞ (so long as we don't have -∞ or don't care about
    indeterminate results).  So if adding is expensive, it might make
    sense to reorder the addition so that the infinities are at the
    front of the array: then, we check whether the accumulator is ∞,
    and if it is, we don't need to do an expensive addition.  This
    could take an 𝒪(𝓃²) operation down to an 𝒪(𝓃) operation!
  - For an operation with an identity element, we have a good initial
    value.  We expect that `sum` above is initialized to the `0` of
    its type.  Without this, we don't have a meaningful end result.

What about with `i`?

  - In the above code, we expect `i` to have constant time access.  We
    _don't_ want it to go out to a database over a network just to
    read or update it—ever!
    
  - `i` needs to be able to be an index into `A`.  If `A` is a `map`,
    `i` could be some sort of string.  If `A` is an array, `i` needs
    to be a non-negative integer.  If `A` is an STL collection, `i`
    could be an iterator.

It is a useful exercise to look at such trivial parts of your code and
see what you're really assuming, and what it gets you.

### C++ Abstractions

C++ programs have four primary sorts of abstractions.

  - __Functions__: A C++ function is an abstraction over a
    well-defined series of steps of computation that take an input
    state to an output state.  You are familiar with these from other
    languages, and though there are some slight differences we'll see
    in C++ later on, you probably have a very good intuition on them.
    Specifically, a C++ function is designed to take some number of
    argument values, run its computation, and then produce a return
    value.  A function has one single, well-defined entry point
    (namely, the first instruction in the function), but may have
    multiple return points.  In place of what we call a function,
    Stepanov and McJones (2009) use the word _procedure_, which is
    clearer and less ambiguous, but less commonly used and understood
    in this sense.

    We will be looking at functions through two different views
    throughout this course, but before introducing them, I caution
    that using one to the exclusion of the other is going to lead us
    nowhere.  Keep both of these views in mind when analyzing and
    evaluating a function.   First, we can consider a function
    exclusively as a well-defined series of steps of computation.
    Under this view, a function describes the steps the machine will
    follow.  This allows us to reason about what the computer is doing
    and make important guarantees about our functions' behaviors and
    interactions.  Here, we are minimizing the role of the input and
    output states; sometimes these will not be the most important
    part, and sometimes, even when they are important, we will only
    concern ourselves with the actual computation.  Another view we
    can use is to consider a function as a black box that takes us
    from the input state to the output state.  In contrast with the
    other view, we are ignoring what steps it takes to change the
    state.  This view can simplify our thinking, which is very useful,
    but can also lead us to ignore important properties about our
    abstractions, such as performance and resource usage, reentrancy,
    and intermediate state changes.

    To prevent potential confusion, I want to also treat some similar
    concepts to make sure we clearly differentiate them from C++
    functions.  As we will be using the term here, a function is a
    specific sort of abstraction we can build with our language.  Be
    sure you keep the following ideas distinct from this.  (If you
    don't quite understand the following list, don't worry!  We'll be
    coming back to this lesson often, and as we learn more, they will
    start to make more and more sense.)

      1. First, I want to make clear that we are not talking about
         mathematical functions here, nor are we necessarily trying to
         model them.  Mathematical functions are simply a mapping
         between an input and output, and do not specify a way to
         compute the output given the input.  On the other hand, our
         functions, given the same input state (same arguments, same
         memory state, same hardware state), will produce the same
         output state; they can thus be partially modelled by
         mathematical functions.

      2. Considering the entirety of memory state when defining the
         input and output states is extremely difficult, so we want to
         avoid this when we can.  You've probably heard that it's hard
         to maintain code that uses global variables.  The reasons for
         this are two-fold: one, any function can modify the global
         variable; two, more importantly here, it's not explicit in
         the call to a function that needs it, so it's easy to forget
         about the dependency.  To simplify this, we can define a
         special case of a function.  It is easier to reason about a
         function where the input state is defined exclusively by the
         parameters and the output state exclusively by the return
         value.  Stepanov and McJones (2009) introduce the term
         _regular procedure_ for this, but you may have heard of it as
         a _pure function_.

         A pure function may still use mutating operations and change
         intermediate states, but the input and output states will not
         reflect this.  If we only need to view a function as a black
         box, this may be okay, but if we need to consider reentrancy
         or performance, this may not be a strong enough guarantee.

      3. A _method_ is a specific type of function where the exact
         code that is called is determined by the dynamic type of one
         or more arguments.  In most object-oriented languages, this
         means the method call is dispatched based on type of object
         you are calling the method on.  Some languages allow you to
         dispatch based on the type of multiple objects—this is called
         a _multimethod_.

         In Java, all non-static functions are methods, and it is
         conventional to call static functions methods as well.  In
         C++, this is not the case.  Functions are not always tied to
         a specific class or class hierarchy, and even when they are,
         they do behave polymorphically unless you specifically
         declare them to.  Even when we do have the same behavior as a
         Java method, we tend to call them _virtual member
         functions_.

         What does this mean?  Unless you make a member function
         _virtual_ in C++, the function that is called is not based on
         the runtime type of the object, but what type the compiler
         sees.  This means that the function that is called is known
         at compile time.

         This concept can be difficult to grasp, especially without
         understanding more C++ code.  Instead, let's look at the
         following two lines of Java as an example.

         ```java
         Base object = new Derived();
         object.someMethod();
         ```

         In Java, this will try to call `Derived`'s `someMethod` if it
         exists, and otherwise will try to call `Base`'s `someMethod`.
         This is because `object` has the _runtime_ (or _dynamic_)
         type `Derived`.  The corresponding C++ code (which is
         slightly different syntactically) would call `Base`'s
         `someMethod`, unless `someMethod` was made virtual.  This is
         because `object` has the _compile-time_ (or _static_) type
         `Base`.

         In this course, we'll see benefits of this opt-in behavior,
         and look at alternatives for the traditional object-oriented
         polymorphism.

      4. A _coroutine_ is a generalization of our function abstraction
         that is currently being proposed for standardization in C++17
         or C++20.  Coroutines allow functions to have multiple entry
         points.  A caller can call a coroutine, which can run and
         produce a value halfway through and suspend itself, giving
         control back to the caller.  The caller can then continue
         working, and call the coroutine again.  The coroutine will
         start back where it left off, with all the same local state,
         and continue working.  Python _generators_, Go _goroutines_,
         and _threads_ are all types of coroutines with different
         properties.  Later in the course, we will be looking at these
         more, and see why they can provide a better basis for
         building certain types of software.
    
  - __Types__: A C++ type is an abstraction over a piece of data and
    associated operations that we are allowed to perform on this
    data.

    This is an intuitive idea that is surprisingly powerful.  It
    doesn't make sense to take a substring of an integer, or to ask
    for the natural logarithm of a TCP/IP socket.  On the other hand,
    we could certainly add two natural numbers together or concatenate
    two strings.  With each of these, we have a piece of data, and we
    know the things we can do with it.

    A great thing about the C++ type system is that it is a _static_,
    _strong_ type system.  This allows the compiler to check that you
    are using a value of the correct type at compile-time.  You can
    let this cause you pain, or you can use it to your advantage.
    When used correctly, we have a built-in checker to make sure the
    semantics of your code are correct—or at least, as you have
    declared them to be.  Types allow you to simply prevent invalid
    states from ever occurring.  This is something we will be doing
    throughout this course, but something you may not see older C++
    code doing.  Don't let this trick you into believing this is not
    useful, beneficial, or something you can and should do.

    We may also think of a type as a mapping from a sequence of bits
    in memory to the abstract notion of a value it is meant to
    represent.  This view of a type simultaneously forces us to manage
    the _representation_ of an object, allowing us to reason about
    performance characteristics of our code, as well as think of the
    valid _operations_ (and sequences of operations) on the value it
    is supposed to represent.

    Finally, we will see that C++ does everything it can to make
    user-defined types look like built-in types.  Many features we
    will see (operator overloading, objects on the stack, literal
    types and user-defined literals) allow us to write type
    abstractions that don't look substantially different from built-in
    types.  This allows us to write clean code that we can understand
    intuitively.  The syntax of C++ is designed to make user-defined
    types look like built-in types.

    For this to work, we have to design our types correctly. C++
    doesn't enforce that we design our types this way, but it's a very
    good way to design the types.  Described informally, `int` is a
    pretty easy-to-understand type.  It's common, easy-to-use, and we
    know more or less what each operation is going to do.  It would be
    wonderful if we could do the same for our own types.  We will be
    talking about _regular types_ and their more refined friends,
    _value types_.  These are conventions that end up saying, “design
    your types like `int`, because it's easy to understand.”  We
    shortly look at _regular types_, and later look at _value types_,
    looking critically at why they are useful and what benefits both
    we and compilers can get from them.
    
  - __Templates__: A template is a means of generating types,
    functions, and variables at compile-time, before static type
    checks are performed.  Any template may take as its input a type,
    a value (with some restrictions), or another template; its output
    is a type, function, or variable.

    As such, we can also think of templates as a pure function from
    types (and values and templates) to types (and functions and
    variables).  This conception of a template as a mapping from type
    to type will become incredibly powerful later in the course when
    we take a look at C++ template metaprogramming.  By thinking of
    templates like that, and using properties of the template system,
    we can write programs that run at compile time within our type
    system.

    Another paradigm that templates (along with concepts, described
    below) facilitate is _generic programming_.  In a generic
    programming approach, we identify the minimum requirements on a
    type that a concrete implementation of some algorithm or component
    needs in order to work correctly and efficiently, and create an
    abstraction where any type satisfying those requirements can be
    used instead.  Take, for example, a simple searching algorithm: we
    traverse an array of integers from start to finish until we find
    what we are looking for.  We can generalize this: this should work
    for any array whose members are of a type that we can check for
    equality.  It should work for any data structure we can consider
    as a sequence of values (an array, a linked list, a stream of
    characters coming from a file, a dynamically generated sequence,
    and so forth, as long as it's an 𝒪(1) operation to go to the
    next element, and we are able to check the current element as many
    times as we want without it changing).  If we are able to abstract
    the searching algorithm away from these implementation details, we
    can implement the searching algorithm once, and use it on any type
    that satisfies our requirements—even ones we don't know about as
    we are designing the component.
    
  - __Concepts__: A concept is a predicate on a type or sequence of
    types.  That is, it maps an ordered tuple of types to a boolean
    value.  Concepts allow us to give names to a set of requirements
    we expect for a type, without explicitly naming the type.  For
    example, it makes sense to talk about equality between integers.
    It doesn't make as much sense to talk about equality of operating
    system resources (what does it mean for two threads to be “equal
    in value”?  Any definition I can come up with surely seems
    artificial, or a product of the representation of the value).  We
    might imagine a concept `EqualityComparable` which is satisfied
    only by types that provide an equality predicate that is an
    equivalence relation (i.e., it is reflexive, symmetric, and
    transitive).  Thus, integers are `EqualityComparable`, but threads
    are not.

    We can distinguish two sorts of requirements on types: syntactic
    requirements, called _constraints_, and semantic
    (correctness/performance) requirements, called _axioms_.  A
    constraint predicate, for instance, would be “given objects `t`
    and `u` of some type `T`, does the expression `t + u` compile and
    give back another `T`?”  An implementation where `t + u` always
    gives back `t` satisfies this predicate.  An axiom predicate, on
    the other hand, may be “given objects `t` and `u` of some type
    `T`, is it true for all possible `t` and `u` that `t + u` and `u +
    t` represent the same value?”  Note that a constraint is trivial
    for a compiler to solve, as it already has all the type
    information necessary to check whether an expression compiles and
    if so, what type the expression returns.  An axiom may be
    significantly more difficult to check and even to formalize.

    In the current C++ specification (C++14), the language has no
    support for expressing concepts.  There is a Technical
    Specification for adding constraints (_Concepts Lite_) to the
    language, and as of writing, this TS is expected to be published
    soon.  We may see it included in the next revision of the
    standard, likely C++17.  Barring that, it is likely that the major
    compiler vendors will implement the TS by that point.  Later in
    this course we will be working with this concept language support
    using an experimental branch of the GCC compiler.

    Even though the language cannot currently express concepts (or
    implement any static checking that they enable), concepts are
    still in your software, either implicitly or explicitly.  Our goal
    is to make both axioms and constraints explicit through good
    documentation and naming conventions.

All four of these means of building abstractions interact.  Although I
distinguish them here and will continue to throughout this course, it
is not possible to view them exclusively in isolation.  For example, a
C++ function will have a type—it has a representation in memory.
Templates generate types and functions (and variables).  Concepts
allow us to restrict the inputs to templates.

C++ provides us with other means of abstraction, but these four sorts
are the primary sorts of abstractions we use to design our software;
they should be the tools you reach for first.

### A brief digression about idioms

Here we take a brief pause in our discussion of C++ to look at
something more general: the difference between _abstractions_ and
_idioms_.  To do so, let's take a look at how we might take a list in
Java, and create a new list where each element has been increased by
one:

```java
List<Integer> xsPrime = new ArrayList<Integer>();
for (Integer x : xs) {
    xsPrime.add(x + 1);
}
```

For comparison, we take a look at how to write it in Haskell:

```haskell
xs' = do_loop xs
  where do_loop (x:xs) = (x+1) : (do_loop xs)
        do_loop []     = []
```

Whereas in Java, we use a clear `for` loop which very clearly says
“iteration,” in Haskell we have to use recursion and pattern matching,
which makes the code harder to read and understand.  On the other
hand, in Haskell, types are inferred, so we don't have the messy
`List<Integer>`s that get in the way.  It is also interesting to note
that the Haskell code is structured more like our problem statement,
and reads as you would explain the problem to someone: “We take in a
list `xs`, do something to it, and get back `xs'`.  That thing we do
is ….”  The Java code obscures what we want to do with how we go about
doing it.  Both, however, get the job done, and a programmer fluent in
Java or Haskell would be able to understand the solution.

What if, though, we want elsewhere to take a list and create a new
list, where each element is now twice what it was before.  While we
could copy our code and change the `+ 1` to a `* 2`, let's take this
as an opportunity to build an abstraction.  In other words, let's make
our goal an abstraction that takes in both a list and what to do on
each element of the list, and then gives back a new list with the
operation applied to each element.  In Haskell, this is easy (and
built-in, even):

```haskell
fmap f (x:xs) = (f x) : (fmap f xs)
fmap _ []     = []

-- Now we can write
xs' = fmap (\x -> x * 2) xs
-- or
twice = fmap $ (*) 2
xs'' = twice xs
```

We've built an abstraction here that can take in a function and a list
and return a list.  This directly expresses our mission above: take in
an operation (a function), the thing to do it on, and then return the
new thing.  Furthermore, it's shorter and clearer than our original
unabstracted version; now that we have this abstraction, we don't have
a reason to write the `do_loop` version anymore.

But when we try to do this in Java, we run into some difficulties.
Java does have a way to express operations (methods), but these are
not first class: we have no easy way take in a method to another
method.  We have to wrap this in an interface:

```java
public class Mapper {

  public interface MapFunction<InT,OutT> {
    OutT execute(InT);
  }

  public static <InT,OutT> List<OutT> fmap(MapFunction<InT,OutT> f, List<InT> xs) {
    List<OutT> xsPrime = new ArrayList<OutT>();
    for (InT x : xs) {
      xsPrime.add(f.execute(x));
    }
    return xsPrime;
  }

}
```

To use this code, we have to do the following:

```java
List<Integer> xsPrime = Mapper.fmap(new Mapper.MapFunction<Integer,Integer>() {
    public Integer execute(Integer x) {
      return x * 2;
    }
  }, xs);
```

Now, both of these solutions work, and both accomplished our goal of
creating an abstraction, but the ways we do so in both Java and
Haskell are drastically different.  In Haskell, we have a language
feature that allows us to directly pass a function to another
function, which is what we want.  In Java, we don't, but we have the
next best thing: we have an _idiom_ that is commonly accepted in the
Java programming world for doing this.  We make an interface with a
method in it, and then create an object of an anonymous class that
implements this interface in the way we want.  An _idiom_ is simply an
abstraction that a language cannot (or a programmer does not) express.
Idioms are idiosyncratic, but can help us write clean, concise code.
We want to use existing idioms where possible, because experienced
programmers are going to be familiar with them, but we want to avoid
creating new idioms as much as we can, because even if we understand
our idioms, or they make the code clearer once you know the idiom,
they make the code harder to read for people unfamiliar with the
idiom.

Unfortunately, in this case, our Java `fmap` makes the code harder to
understand.  We're fighting against the language here, and it's much
easier and clearer to write the original `for` solution.  I want to
make clear, though, that the failure of the Java abstraction is not
its verbosity (or rather, the success of the Haskell abstraction is
not its annoying terseness), but that the Java abstraction does not
say what it means: it wants a class, and a class isn't an operation.
We have to know how the class is supposed to be structured to make any
sense of the code that we have.  In other words, the code doesn't _say
what it means_.

Be on the lookout for idioms in C++ and in other languages you use.
It's best to be conscious of them, and see if there is a way you can
create a real abstraction over them.

Machine Model
-------------

The above abstractive mechanisms are not found in all languages, but
there are other languages that either have these mechanisms or have
mechanisms that provide the same benefits.  C++, however, is unique in
allowing us to combine these with full access to the underlying
system, allowing us to write software for which it is both easy to
reason about correctness and performance characteristics, such as
memory usage, asymptotic runtime, and wall clock time.  C++ is
designed to give us easy access to all the properties.  In essence,
C++ allows us and the compiler to cut away all the abstractions we've
built and think at a very low-level as to what the machine is actually
doing.

This has been a focus of recent developments in the C++ language.  The
original C++ standard was published in 1998; to put this in
perspective, in 1997, when the standard was being finalized, Intel
released its first processor with the MMX vector extensions.  Now,
SIMD instructions like this are widely used in graphics and scientific
domains, giving significant performance benefits.  At least four
architectures have SIMD (single instruction, multiple data)
instructions.  But there is currently no good way to express this in
C++.  We have to manually write the assembly (bad), use
compiler-specific intristics (bad), rely on the compiler to optimize
with them (bad and probably won't work), or go without them (bad!).
With this specific example, there is work going into allowing us to
express this, but the important part is that this is a concern.  The
inventor of C++, Bjarne Stroustrup, likes to say that there should be
room for no language below C++.  What he's trying to say is that C++
should be able to express all of the vocabulary a machine architecture
gives, from SIMD to hardware transactional memory to GPGPU (general
purpose GPU computation).

In order to understand C++, we need to understand the machine model it
lives on.  Only then can we possibly reason about performance.  This
machine model is simplified from what actual hardware is doing, and we
sometimes will concern ourselves with that relationship as well.
Unlike many other languages, though, C++ directly exposes us to the
machine model.  We _can_ subvert (almost) any abstraction we want.  We
_can_ subvert the type system.  We are able to do all of these things
(though we usually don't want to).  Put another way, all abstractions
in C++ are leaky.  We will see how this allows us not only to write
fast code, or code that uses small amounts of memory, but more
importantly, code for which we can easily reason about these things.

We can't let one of the these two goals override the other.  You will
find that with the proper abstractions, it is possible for our highly
abstracted, easy-to-understand code to be highly performant as well.
Our main friend here is the compiler's optimizer: the compiler, given
sufficient information, can make incredibly fast software from the C++
code we give it.  Optimizers love the abstractions we discussed above.
Templates allow us to execute code at compile time, which might
otherwise be executed at runtime.  Functions and types—especially
built-in functions and types—give the optimizer additional information
which it can use to reason about what you want from your code and not
have to guess.  Concepts, especially with axioms which explicitly
state intention, have the potential to allow the optimizer to apply
the same algebraic operations it applies to integral types to your own
types.  The overriding message for performance at this level is the
same as for writing understandable abstractions: _say what you mean_.
Highly abstract C++ is often faster than C or Fortran code written to
do the same thing, and it is usually easier to understand.

We are straddling two worlds here: one is the world of high-level
language design, where we want to build the best abstractions to let
us manage software complexity, and the other is the world of the
low-level machine architecture, where we want to reason about the
performance characteristics of our software and understand exactly
what is happening.  While an ideal C++ would let us do both of these
perfectly and simultaneously, the current C++ lets us do these at all.
By the end of this course, you will feel comfortable shifting between
these two vastly different mental models, and will see how these two
views complement, conflict, and relate to one another in language
design, library design, and application design.

Further Reading
---------------

It is in your best interest to read the following:

  - isocpp.org FAQ, [_Big Picture Issues_][isocpp-faq]
  - Stepanov, [_Notes on Programming_][stepanov2007], Preface and
    Lecture 1.
  - Stepanov and McJones, _The Elements of Programming_, Chapter 1
  - Stroustrup, _The Design and Evolution of C++_, Chapter 4
  - Stroustrup, [_A History of C++: 1979–1991_][stroustrup1995]

[isocpp-faq]: https://isocpp.org/wiki/faq/big-picture
[stepanov2007]: http://www.stepanovpapers.com/notes.pdf
[stroustrup1995]: http://www.stroustrup.com/hopl2.pdf
