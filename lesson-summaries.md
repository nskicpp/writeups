Niedzielski C++ Course Lesson Summaries
=======================================
Instructor:  Patrick M. Niedzielski <patrick@pniedzielski.net>

Unit 1 -- Introduction
----------------------

  - __1. Introduction__: Goals for the course and logistics

  - __2. What is C++?__: A high-level view of the language, its
    design, and the designs it pushes you towards.  C++ gives you two
    tools: the ability to write code that is efficient and performs
    well, while at the same time building abstractions so you don't
    always have to think about that.  The four primary tools we have
    to build abstractions in C++ are _functions_, _types_,
    _templates_, and _concepts_.

  - __3. Setting up a Development Environment__: Looks at the C++
    compiler toolchain, as well as other useful tooling for
    development.  Then, gives two Hello, World! programs, shows how to
    compile them, and discusses their differences.

  - __4. C++ Crash Course 1__: Shows the basics of C++ code, without
    any abstraction.  The features presented here are all you need to
    write any program, but doing so is difficult.  Covers language
    control flow features, operators, compound data types, variables,
    literals, and namespaces.

  - __5. C++ Crash Course 2__: Introduces free functions in C++ and
    three abstractions from Boost that students may find useful.

  - __6. C++ Crash Course 3__: Shows how to build type abstractions
    using `struct`s, type aliases, member functions, and access
    control.  Finally, briefly introduces references, with a note on
    efficiency.

  - __7. C++ Crash Course 4__: Motivates and explains function and
    type templates, with type and non-type template parameters.  Looks
    at specialization of templates based on types, and how this is
    similar to pattern matching.  Then, looks at concepts and how
    C++17 partially expresses them in the language.  This leads us
    into the ideas of generic programming and will shape the way we
    design software for the rest of this course.  Finally, hints at
    how templates enable type-level metaprogramming.

  - __8. Stack Memory, Heap Memory, Lifetimes, and Moving__: Explains
    the difference between the stack and the heap, shows how to
    allocate memory on the heap, covers lifetimes and then RAII, and
    introduces C++ move semantics.  We will introduce C++ lambda
    expressions.

Unit 2 -- Building Abstractions
-------------------------------

  - __9. Correct Software__: We will look at ways to reason about our
    software's correctness, as well as ways to show it is correct.  We
    will look at runtime testing, static analysis, and dynamic
    analysis.  We will look at how to design software so it is easy to
    understand (and thus easy to show correct), and how to design
    software so it is easy to write tests for.  We will briefly
    mention TDD and various test programs.  Finally, we will look at
    the benefits that each way bring, both from a correctness
    standpoint and a maintainability standpoint.

  - __8. The Standard Template Library__: With basic knowledge of
    generic programming from the last unit, we look at a real library
    that is designed on this principle.  We focus on sequence
    containers and the standard algorithms, and use them to introduce
    the concepts of `Iterator`s.  Then, we will write our own
    STL-compatible `conservative_vector` data structure and reason
    about its performance and efficiency.

  - __9. Coupling and Cohesion__: We used these two ideas without
    knowing it in the last lesson to give us the iterator abstraction
    that glues together containers and algorithms.  We will start to
    look at how we can use the principle of minimizing coupling and
    maximizing cohesion to design better software.

  - __10. Value Types, Equality, and Identity__

  - __11. Documentation__

  - __12. Encapsulation, Invariants, Classes, and Open Abstractions__

  - __13. Designing Interfaces__

  - __14. Designing Systems__

  - __15. Case study: `iostreams`__

  - __16. Customization Dimensions__

  - __17. Object-Oriented Programming__

  - __18. Code Formatting__

  - __19. Functional Programming__

  - __20. Algebraic Structures__

  - __21. Concepts__: We will compile the latest version of GCC which
    has language support for expressing constraints.  We will look at
    the previous version of concepts, _Concepts C++0x_, which didn't
    get voted in.  We will also compare concepts with Haskell
    typeclasses and Java interfaces and look at their similarities and
    differences.  Finally, we will look at Andrei Alexandrescu's
    recent talk on concepts, and look at how the C++ language
    addresses this, and how it may shape our designs.

  - __22. Design Patterns__

  - __23. Type Erasure__
