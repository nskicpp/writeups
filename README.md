Niedzielski C++ Course
======================
Instructor:  Patrick M. Niedzielski <patrick@pniedzielski.net>

Prerequisites
-------------

This course assumes programming maturity and familiarity with more
than one programming language.  We will be covering gaps in individual
student's knowledge, but understanding of basic computer science and
software engineering concepts.

Students will be required to submit using GitHub, but are free to use
any git hosting they want.  Our development environment will be
GNU/Linux; please contact me if you need help setting up a Linux-based
system.

Assignments
-----------

I will be assigning many different resources for you to look at.  I
expect you to look at these critically, and try to understand the
fundamental ideas in each of them.  These may be talks, articles,
chapters of books (provided to you or freely available online), or
podcasts.  Some of these may be optional, but it is still in your best
interest to look at them if you can—usually they will be optional or
recommended because they aren't freely available or are too long for a
Fair Use copy.

Additionally, there will be some small coding assignments to
demonstrate your understanding of a particular part of the language.
Consider this code “living”—we will very frequently be coming back to
it and looking at it again, so it is your best interest to write them
and document them well.

Finally, we will be working on two projects throughout the course: a
`util` project, which will contain utilities that you will find useful
in any software project, and an assigned/negotiated project.  The
assigned project will be about applying what you have learned about
C++ and software engineering, and will require you to learn about the
domain of the project as well.  These two projects will be ongoing
throughout the course.

Learning
--------

This course is structured around instructor-student interaction: you
will be introduced to C++ and software engineering through the
resources I provide, but this will be solidified by discussion with me
(and to a lesser extent, with your peers).  As such, I want you to be
thinking critically about all the material you are shown in this
class and everything I say, as well.  Much of this is an art, and we
will be looking at pros and cons of every design decision we come
across.

As such, please, please come to me with your questions and ideas.  Do
not design and work on your assignments in a vacuum—this is a great
way to fail.  You are expected to learn, not to figure out on your
own.  When you can't figure it out, please ask at any point.  I will
get back to you as soon as I can.

Participation and Evaluation
----------------------------

In this course, you will not be graded on your work.  Your goal here
is not to get a good grade, but rather to get a good understanding of
the course material.  You are encouraged to collaborate with peers,
and you will be evaluated not on your work, but on your understanding
of your work.

As a result, we need a way both for me and for you to evaluate your
understanding of the material.  This will be done a number of ways,
but the primary one will be by code review.  After submitting your
work, I will be commenting on it and evaluating whether it is
acceptable as a final product.  You are expected to respond to those
comments, either answering the questions in them, explaining your
reasoning for your design over other design, or changing the code to
address the comments.

These comments will be things you should be asking yourself as you are
writing software.  Later in the course, you will be required to review
your peer's submissions as well.  These are the sorts of questions you
should be asking.  This will further demonstrate your understanding of
the material.

In addition, this particular repository will contain lessons and other
documents for the course.  I encourage you to contribute if you have a
comment; please do so by pull-request and not direct commit.

Licensing
---------

Unless otherwise noted, the materials in this repository are licensed
under the Creative Commons Attribution 4.0 International License. To
view a copy of this license, visit
<http://creativecommons.org/licenses/by/4.0/>.

I expect your work to be licensed under a Free Software and Open
Source license.  A commonly-used license in C++ is the
[Boost Software License][boost-license], which is a permissive license
like MIT/X11; this is a good choice.  Another good choice is the
[GNU GPL, version 3.0 or later][gplv3+].

[boost-license]: http://www.boost.org/users/license.html
[gplv3+]:        https://www.gnu.org/licenses/gpl.html

Schedule and Pace
-----------------

I expect that some students will be ahead of others.  This is fine.
It is more important that you understand than you move at the same
pace as others.

In other words, this course will be self-paced.  Be respectful of the
fact that your peers may be ahead or behind you, and be respectful of
my own time.

The following are a rough sequence of topics we will be covering:

  1. __Introduction__: We will be looking at from a very high-level
     perspective, trying to answer the question: What is C++?  We will
     set up a development environment, take a tour of C++, motivate
     some goals in software design and engineering, and look at forms
     of tooling and uses of version control that you you should be
     aware of and using as much as you can.  _Assignments will be
     small pieces of code to get a feel for C++._
  2. __Building Abstractions__: Here, we will be covering the C++ way
     of building abstractions.  We will look at designing interfaces
     that are flexible, easy-to-use, safe, and testable.  We will be
     learning about _open abstractions_, _vocabulary abstractions_,
     the _pyramid_ model of software design, and _customization
     dimensions_.  We will also be taking a look at the C++ standard
     library and the Boost software library.  Throughout all of this,
     we will be introducing features of C++ that allow us to build the
     abstractions.
  3. __Performance__: We will be peeling back some of our layers of
     abstraction and looking at what the compiler is actually doing.
     This will involve much lower-level abstractions that you will not
     have used extensively before.  We will learn about compiler
     optimizations and be looking at assembly code generated by the
     compiler.  We will also learn about performance testing.  By the
     end of this unit, you should be comfortable reasoning about
     performance in a C++ program.
  4. __Error Handling and Signalling__: We will be talking about the
     standard C++ error signaling means, _exceptions_, as well as what
     their strengths are and their weaknesses.  Once we have covered
     the basic use of exceptions, we will look at how to make code
     exception-safe, when to use exceptions, and alternative means of
     error handling and signalling.
  5. __Advanced Tools and Pitfalls__: We will be going back to topics
     that the last segments have introduced and looking at them in
     more detail.  Specifically, we will be covering advanced features
     of the C++ object system, template system, and standard library.
     In addition, we will be filling in more details about we've
     learned that may surprise you; you will usually not run into
     these, but when you do, you will need to know these details,
     unfortunately.  We will also play around with an experimental
     implementation of C++ Concepts Lite, a proposal to add
     constraints checking to the language.  After this unit, you
     should have a relatively complete grasp of the C++ language and
     standard library.
  6. __Metaprogramming__: We will be looking at the three subsystems
     of the library that allow us to do computation and processing at
     compile-time.  You will be looking at idioms that facilitate
     this, and building abstractions above these idioms to make
     metaprogramming substantially easier.
  7. __Concurrency__: We will be working with atomic variables, memory
     models, threads, and higher-level concurrency constructs.  In
     addition, we will be looking at proposals to add _coroutines_,
     _transactional memory_, _parallelism_ support, and additional
     high-level _concurrency_ constructs to the C++ language.
  8. __Future Developments and Other Topics__: We will be taking a
     look at the C++ standardization process and discussing proposals
     to extend the language.  You will be evaluating proposals and
     gaining a sense of where the language is headed.  We may also
     cover miscellaneous topics here which would not fit elsewhere in
     the course.

This schedule is ambitious and requires a level of participation from
you as well.

Feedback
--------

I appreciate feedback about the course and teaching style.  Although
by its nature I will be interacting with you throughout this course,
I still would appreciate direct feedback.

Resources
---------

We have a mailing list at <nskicpp@googlegroups.com>  This is a
_great_ place to ask for help.

This repository will contain a number of good resources as well—check
back frequently (or better yet, watch it to be updated of any
changes).
