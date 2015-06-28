Setting up a Development Environment
====================================

In this lesson, we will be looking at the compilation model for C++,
introduce various tools we can use to help us write better code, set
up our Linux systems for the rest of the course, and write our first
C++ program.

The C++ Toolchain
-----------------

C++ inherits its compilation mechanism from C, so if you have any
experience with compiling C programs, the process for compiling C++
programs will be familiar.  Although we won't be interacting with this
compilation mechanism directly most of the time, it's useful to know
what's going on under the covers.

C++ programs are composed of a number of _source files_, which usually
have the `.cpp` extension, and _header files_, which usually, but not
always, have the `.h` or `.hpp` extension.  Usually, the header files
contain _declarations_—you can think of these like the interface of
your functions, types, and so forth—whereas source files contain
_definitions_—the actual implementations.  As an example, every source
file that wants to send output to the terminal has to “include” the
header file `<iostream>`, which has the declarations of all the
functions and types you need to send output to the terminal.

When you want to compile a C++ program, first you have to send each
source file into the _preprocessor_ and then to the _compiler_.  The
preprocessor's job is to perform certain text substitutions in the
your source file, like taking header files that the source file
“included”—we'll see how to do this later this lesson—and putting them
directly in the source file (!).  The output of the preprocessor is
fed to the compiler, which converts the source file into a compiled
_object file_.  Finally, all the object files and any libraries you
need are stitched together by the _linker_, which makes the final
executable or library.  Because the output of each of these tools is
fed into the next, we call this the _toolchain_ (or more colloquially
and less precisely, just the _compiler_).

![Source files are fed into the preprocessor, and then the compiler to
produce object files.  Object files are combined by the linker into an
executable or library.](toolchain.png?raw=true)

You may hear me refer to _translation units_.  This is the technical
term for “a single source file and all the header files that it
includes.”  Thus, each translation unit corresponds to a single object
file.  This is sometimes an important thing to think about, but often
you can get by just thinking of source files and header files.

If you don't have any experience with C, the preprocessor will
probably seem strange.  It is.  If you have experience with C, the
preprocessor will probably seem useful.  It is.  I will caution you
not to rely on it, though.  It can be incredibly problematic, as we'll
later see.

### A note for the interested…

If this mechanism seems antiquated compared to other languages you are
familiar with, that's because it is.  At this point, and especially
for compatibility with C, not much can be done to evolutionarily
improve this.  However, there is work in the ISO C++ Committee to
create a more modern compilation model.  This model, based on an
abstraction called _modules_, provides significant benefits over the
traditional header-file include model.  It is likely to be
standardized either in the next version of the Standard (C++17), or is
a separate specification released at roughly the same time.

__Discussion__: What are some of the problems you can think of with
the current model?  If you know C, you may have first-hand experience
with these problems.  How would you fix these?  What models do other
languages use, and how would they work (or not work) for C++.
Consider our two points from last lesson, avoiding a mechanisms that
limits the potential abstractive power of our source code or
introduces run-time costs.

### Compilers

Now that you understand what a toolchain is, we can talk about some
specific toolchains.  In this course, we'll primarily be using two
different Free and Open Source toolchains to compile and test our
software.  These toolchains are both high-quality.

  - __The GNU toolchain (GCC)__: GCC, which stands for GNU Compiler
    Collection, is a very stable toolchain that supports a variety of
    platforms and languages.  It is the standard toolchain on most
    GNU/Linux variants.  You may find it produces faster code.
  - __The LLVM toolchain (Clang)__: Clang/LLVM is a relatively new
    toolchain that was designed to be extremely extensible.  The
    compiler, specifically, is designed as a number of libraries that
    interact.  This allows, for instance, a tool that just needs to
    parse the C++ code and not generate an executable from to just
    link to Clang's parsing library; a tool that injects code into the
    final executable (called _instrumentation_) could be linked into
    the compiler easily.  You may find it compilers code more quickly.

In addition, as the course goes on, we may start targeting a third
toolchain via an online build service.  __The Microsoft toolchain
(MSVC)__ is the standard compiler on Windows systems.  It is not quite
as standards-conformant as the above toolchains, but it is still very
close.

__Discussion__: What is the benefit of testing code on multiple
toolchains?  Is it only necessary when targeting multiple platforms?

### CMake

Dealing directly with the toolchains works effectively can work fine
for small programs, but it quickly becomes hard when we have larger
projects with multiple source files.  We don't want to invoke the
compiler directly with many source files.  If we modify only one
source file, we don't want to recompile all other source files.  On
the other hand, if we modify a header file that is included by many
other source files, we need to recompile those source files.  This can
quickly become difficult to manage.

Various schemes for dealing with this have been devised.  In other
languages, you may have used an IDE like Eclipse to do this—IDEs for
C++ exist and are widely used.  Of course, for IDEs to do this
properly, everyone on the team has to be using the IDE, and the IDE
has to support all target platforms.  In the UNIX world, `Makefile`s
were developed.  These files are fed into a tool called `make` which
uses the information to intelligently compile only the right source
files.  It turns out that maintaining `Makefile`s at scale is also
difficult, but they have another problem: they are not dynamic.  There
is no way to automatically detect something about the system (like the
path to a library or header file) and change the compiler invocation
to reflect that.

In this course, we will be using a build system called
[CMake](http://www.cmake.org) to get around this problem.  CMake is a
tool that generates `Makefile`s (and IDE project files, too!) from a
file called `CMakeLists.txt`.  Unlike a `make`, though, CMake's input
language is a full scripting language, which allows the build system
to be dynamic.  This means we can easily add test programs,
dependencies, and different build settings in an automated fashion.
Furthermore, CMake is cross-platform, making it a good choice for
projects that need to be compiled on multiple toolchains and multiple
platforms.  We will see how to install it and then write a simple
`CMakeLists.txt` file.

Tooling
-------

Before we move on to setting up our systems to compile C++, we should
reflect on something interesting we've just seen: even in just the
last section, we've seen a variety of different tools that help us
write better software.  We've briefly touched upon the difference
between different toolchains and why one might prefer one over
another.  We've discussed multiple solutions to simplifying and
automating building our programs.  These may seem obvious, because
we've seen the problems and have been using similar tools since we've
started programming.  Not all problems are as obvious, though.
Besides our toolchain and build system, let's discuss a few tools that
are incredibly useful in software development, but which you may not
have considered before:

  - __Version Control__: Some good examples of this tool are
    [git][git], which we use in this course, [Subversion][svn], and
    [Perforce][perforce].

    We use version control for two reasons:

      1. We want to be able to store and access previous versions of
         our source code.  If we have that safety net, we can modify
         and break our source code, knowing that we can go back to a
         state we know worked at any point.  On the other hand, if we
         find that a bug was introduced at some point, we can compare
         our previous versions and see exactly where and when the bug
         was introduced, and thus probably how to fix it.
      2. We want to be able to easily work in a team.  If two team
         members are working on the same file, we don't want the one
         who checks in the file later to clobber the changes from the
         one who checks in earlier.  All but the very earliest version
         control tools make it easy to deal with this situation.

    These two goals are sometimes in conflict, and we will talk about
    some best practices and some recommendations for getting the most
    out of our version control.  We'll also see some interesting tools
    that can be built on top of `git`, specifically, and we'll talk
    about why those tools can be built and what they give us.

  - __Documentation Generators__: If you've used Java or Perl before,
    you know how useful these can be if used correctly.  These sort of
    tools are incredibly ingrained in the culture of those two
    communities.  In Java, a tool called `javadoc` is used to generate
    HTML files from comments placed before each method or class
    header.  Perl has a similar tool that takes specially formatted
    POD blocks and generates documentation for a man-page-like viewer.
    A commonly used tool for this in C++ is called [doxygen](doxygen),
    and it is very similar to, but more powerful and extensible than,
    Javadoc.

    We shouldn't limit ourselves to just generating documentation from
    comments.  There are number of interesting documentation forms we
    can generate from source code.  One could easily imagine a tool
    that generates a UML diagram from source code, or a dependency
    graph between modules.  These two views on the software could help
    people understand the entire system better than API documentation
    does.
    
  - __Static Analysis__: When the compiler type-checks a program, it
    is performing a sort of static analysis.  A static analysis tool
    simply checks your program for certain errors without running it.
    These checks might be correct, finding a bug you didn't realize
    you had, or might be a _false positive_, telling you there is a
    bug where there is not.

    Static analysis tools are very useful, but until recently there
    have not been many high-quality freely available tools in almost
    any language.  Later on, we will be using the [CppCheck][cppcheck]
    tool, which will catch most errors from unsafe C++ code.  It is
    not as high-quality as some enterprise tools, but it still works
    very nicely.  Static analysis tools, of course, cannot catch every
    bug, but they are a very important and underused tool for making
    sure our software works, and works well.
    
  - __Dynamic Analysis__: Dynamic analysis tools are complementary to
    static analysis tools, in that they work by running the software
    and analyzing what it's doing as it runs.  A tool you may have
    used before is [valgrind][valgrind], which detects memory leaks
    and invalid accesses at run-time.  Although we will be seeing
    effective ways of preventing these in the first place, in C, where
    you can't use these methods, valgrind is an _extremely_ useful
    tool.  I recommend that you run your tests through valgrind if you
    can.  It does affect the speed of execution by several hundred
    percent, though, so keep that in mind.  Another sort of dynamic
    analysis tool is a _profiler_, like [`gprof`][gprof], which gives
    us information not about the correctness of our software, but
    rather its performance.

    A slightly modified approach to dynamic analysis is to modify the
    code that is built so it is more suited to dynamic analysis, or so
    it does the analysis on itself.  This is actually the method
    `gprof` uses.  In modern terminology, this sort of tool is called
    a __sanitizer__, and it combines normal dynamic analysis with
    _instrumenting_ the program with bits of code to help give the
    information we're interested in.  We will be using sanitizers
    later in the course.

  - __Linter__: When we talk about high-quality code, we don't just
    mean code that is free of bugs and has high performance.  We want
    code that is idiomatic, clear, and easy-to-read.  While in many
    cases this is a judgment call, there are certain rules that we can
    codify—naming conventions and such.  Once these rules have been
    codified, we can write tools to make sure the source code we write
    is following them.  These tools are called _linters_.  Like static
    analysis tools, they look at your source code to find problems,
    but unlike static analysis tools, they look at your source code
    to find _style_ problems.  [clang-tidy][clang-tidy] for C++ and
    [PerlCritic][perlcritic] for Perl are good examples of this.

  - __Source Formatter__: Once we've detected the problems with a
    linter, the next step is fixing them.  For a large class of
    problems, this can be automated as well.  The most common tool to
    do this is a _formatter_.  A formatter, like
    [clang-format][clang-format] for C++ and [Perltidy][perltidy] for
    Perl, will automatically format your code for you in a consistent
    way.  Formatters are also widely used in the Python and Go
    communities.
    
  - __Tests__: Almost all the tools I've talked about so far are tools
    that others have written for you.  They may be difficult to apply
    to your own project, or they may not completely suit your
    project.  Often we make due with this, because we lack either the
    expertise or time to write the tools ourselves.

    There is, however, a large number of tools that we do write
    ourselves and which are customized to our project: automated
    tests.  Unit test programs do a form of dynamic analysis: they run
    our software in a specific way and verify that the output is as we
    expect.  Other sorts of test programs may function as linters,
    making sure that our APIs are used correctly throughout the entire
    code base.

    Once we have a mentality of tooling, it becomes easier to write
    our own; these tools will help us write better software.  I
    encourage you to think about the tools that you use and be on the
    lookout for tools that have large benefits and open up new ways of
    developing.

This isn't a complete taxonomy, but I hope it helps you understand
some of the problems that we may not realize we face, and how tooling
can help.  These tools also interact: we can, for example, set up a
static analysis tool or source formatter to run whenever we commit, so
we can guarantee that our code is free from a certain class of errors
or from incorrect formatting.

__Extra__: A tool I have not used, but which I've been intending to
for a while is called [CLING][cling].  It is a _REPL_ for C++ based on
Clang.  A REPL (“Read-Eval-Print Loop”) is a tool that allows you to
directly type lines of source and execute them immediately.  Scripting
languages commonly use tools like these, and a good example is Ruby's
`irb` or OCaml's `utop`.  Such tools are hard to implement well for
C++ for a number of reasons, from its strong emphasis on static
semantics and reliance on seeing the entire source file for optimizing
away abstractions, to its ability to access any portion of memory from
anywhere in the code.  You may find it interesting to play around with
this tool, especially if you are familiar with a REPL from other
languages.  If you do, please tell us how you find it!

[git]:          https://www.git-scm.com/
[svn]:          https://subversion.apache.org/
[perforce]:     http://www.perforce.com/
[doxygen]:      http://doxygen.org/
[cppcheck]:     http://cppcheck.sourceforge.net/
[valgrind]:     http://valgrind.org/
[gprof]:        https://en.wikipedia.org/wiki/Gprof
[clang-tidy]:   http://clang.llvm.org/extra/clang-tidy.html
[perlcritic]:   http://www.perlcritic.org/
[clang-format]: http://clang.llvm.org/docs/ClangFormat.html
[perltidy]:     http://perltidy.sourceforge.net/
[cling]:        https://root.cern.ch/drupal/content/cling

Setting up
----------

Please make sure you have the following tools installed.

  - CMake 3.1 or later
  - GCC 5 or later
  - Clang 3.6 or later
  - CppCheck
  - Doxygen
  - Boost (useful libraries we'll be using often)

If you need help with this step, please contact me.

Hello World
-----------

At this point, we are equipped to start writing actual C++ code.  We
are going to be writing the traditional Hello World program, but we
are not just going to write the C++ code and see how it works.  We are
going to set up a `CMakeLists.txt` for this program and see how to
build and test our Hello World program.

### Version 1

Here is a simple Hello World program in C++.  You can find it under
the `examples` directory in the file `hello1.cpp`.

```cpp
/* hello1.cpp

   Prints "Hello, World!" to the screen and exits
*/

#include <iostream> // For std::cout
#include <string>   // For std::string
#include <cstdlib>  // For EXIT_SUCCESS

int main() {
  using namespace std::literals; // So we can use ""s syntax
  
  std::string message{"Hello, World!"s};
  std::cout << message << "\n"s;

  return EXIT_SUCCESS;
}
```

Before we try to build and execute this program, let's make sure we
understand what exactly what it is doing.  At this point, we won't
worry how the parts of the standard library are implemented; that
would be too much, too soon.  Instead, try to focus on what they do,
instead.

The lines that begin with `#include` are preprocessor directives to
include a header file.  Here, we're including three standard header
files: `<iostream>`, `<string>`, and `<cstdlib>` so we can use the
functionality they give.  Remember, the preprocessor takes these
header files and puts their contents directly in your source file.

The rest of the program, from `int main() {` to `}`, is the _function
definition_ of the function `main`.  Every C++ program has exactly one
function called `main`, which is called when program execution begins.
This function _returns_ an integer (type `int`) and takes no
_arguments_ (the empty `()`).  Its _body_ is all the code between the
`{` and `}`.

The important parts of the `main` function are the following three
lines:

  - `std::string message{"Hello, World!"s};`: Here, we declare a
    variable called `message` which has type `std::string` and
    initialize it to the text inside the braces.  The syntax in C++
    for a `std::string` literal is `"stringhere"s`.  You'll need that
    `using namespace std::literals;` line to do this.  We'll talk
    about that line in more detail in a moment.

  - `std::cout << message << "\n"s;`: `std::cout` is a predeclared
    variable that represents the system console output.  To output
    something to the console, you write `std::cout`, the `<<`
    operator, and then the thing you want to write, which in our case
    is the value stored in `message`.  As you can see, we can chain
    these together, adding an extra `<< "\n"s` to make sure we print a
    trailing newline.

  - `return EXIT_SUCCESS;`: As we said before, `main` returns a
    integer value.  This value is returned to the system and tells it
    whether the program ran successfully or had an error.  Here, we're
    saying we ran successfully.

    You may also see this written as `return 0;`, which has the same
    meaning.  Using `EXIT_SUCCESS` is clearer, though.

So, the program starts at the beginning of `main`, declares and
initializes a variable, prints it out, and exits successfully.

Those are the important parts of the program.  You'll also notice that
the program has _comments_.  C++ comments come in two varieties.
_Single-line comments_ start with `//` and continue on until the end
of the line.  _Multi-line comments_ start with `/*` and continue until
an `*/` is encountered.

### Building and Testing

Now that we understand what our code is supposed to do, we need to
build it and make sure it does what we expect it to.  To build the
program, we're going to make a `CMakeLists.txt` file in the same
directory as our `hello1.cpp`:

```cmake
project(nskicpp-examples)
cmake_minimum_required(VERSION 3.1)

# GCC and Clang both default to using an old C++ standard, so we need
# to explicitly tell them to use the version we're using, C++14.  This
# will make all targets use C++14, which is what we want here.
set(CMAKE_CXX_STANDARD          14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hello1 hello1.cpp)

enable_testing()
add_test(NAME test_hello1 COMMAND hello1)
set_tests_properties(test_hello1
  PROPERTIES PASS_REGULAR_EXPRESSION "Hello, World!")
```

We start each `CMakeLists.txt` with a `project()` command, which has a
name that CMake uses internally for your project.  We also want to
make sure we're using a version of CMake that supports the features
we're using—there were radical changes to CMake with version 3.0, and
3.1 added support for C++14.  Then, we require C++14 to be used, and
create an executable from our source file.  Finally, we create a test
case that runs our executable and checks its output.

Later, we'll be seeing some more complex features of CMake, but for
now, this is all we need.

At this point, we run the following commands to build the software
with GCC:

```
$ cd directory/with/cmakelist/
$ mkdir build/ # Let's keep the generated files separate from source files
$ cd build/
$ CC=gcc-5.1 CXX=g++-5.1 cmake .. # Replace with your version of GCC
$ make
$ ./hello1
$ make test
```

Or, to build with Clang, change `g++-5.1` and `gcc-5.1` to your Clang
executable.  Try building it with both.

You'll notice something we did here that may be confusing.  We made a
directory called `build/` and generated all our files here.  This
keeps our source tree clean, and means that whenever we want to clean
the generated files, we just have to delete the files in this
directory.  You'll want to clean the directory when you change
compilers or change your `CMakeLists.txt`:

```
$ pwd # Make sure you're inside build/ !
$ rm -rf *
```

### Another version

Let's look at another version that does the same thing.

```cpp
/* hello2.cpp

   Prints "Hello, World!" to the screen and exits
*/

#include <iostream>
#include <string>
using namespace std;
using namespace std::literals;

int main() { cout << "Hello, World!\n"s; }
```

We can add this to our `CMakeLists.txt` file (see the `CMakeLists.txt`
file in the examples to see how).

Here, you can see we've simplified the program somewhat.  The variable
declaration wasn't needed; we could just use the string literal
inline.  It also turns outs that for `main`, as a special exception,
we don't need a `return` statement, because it has a default return
value of `EXIT_SUCCESS`.

Finally, let's pause for a minute to talk about the `using namespace`
lines.  In C++, a namespace is just a grouping mechanism for anything
that's named—variables, types, functions, even other namespaces.
Nearly everything in the standard library is in the `std` namespace.
In the previous version, we typed that `std::` prefix before
everything from the standard library.  Sometimes, that is clearer.
Here, we take a different approach: we add a `using namespace std;`
statement, which lets us drop that prefix anywhere in the same _scope_
(roughly, within the same pair of braces, or in this case, within the
same file).  You can also see why we added the `using namespace
std::literals;` in the first example: that bit of `""s` syntax is
declared in the `std::literals` namespace.

__Discussion__: What are the benefits of each version?  Which do you
think is clearer?  What techniques were used to make that one clearer?
Would these techniques help in a larger program, or would they make a
larger program less clear?

__Exercise__: Write a program that prompts the user for their name,
stores what they enter in a `string`, and then prints out a message
that has the name in it.  The syntax for input is the inverse of
`std::cout`: to input, you write `std::cin >> input_variable;`.

Further Reading
---------------

For next time, please read [_A Tour of C++_][tour], parts 1 and 2.
Focus on part 1, but don't worry; we will be going over the materials
in it in depth next lesson.  Try to get the big ideas and get a feel
for the syntax.  It may be helpful to play around with some of the
examples.  Also, know that this guide is a guide to C++11.  Some of
the things in it we have a better replacement for in C++14 (like the
`""s`, which is often what you want instead of `""`).  If there are
some slight differences, don't be too worried.  Ask what they mean.

[tour]: https://isocpp.org/tour
