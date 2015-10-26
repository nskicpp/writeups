The Standard Template Library
=============================

Last time, you were introduced to the thesis I will be trying to prove
to you for the remainder of this course, namely that our primary way
of writing correct software is not by testing (dynamic analysis) or
compile-time checks (static analysis), but rather by writing software
with a high level of abstraction, such that bugs become self-apparent
to readers of the code.  I will not be able to prove this to you this
lesson, but my hope is we will make some progress in starting to show
this is true.

To do that, we will be looking at the design of the algorithms and
data structures in the C++ Standard Library, known is the _Standard
Template Library_.  This library is an excellent example of the
_generic programming_ paradigm, which we briefly looked at in
[lesson 7][lesson7].

[lesson7]: https://github.com/nskicpp/writeups/blob/master/lessons/07-c%2B%2B-crash-course-4.md

Introducing Iterators
=====================

Consider the following C-like function template that copies the
contiguous memory in the range `[ in, in_end )` to the contiguous
memory in the range `[ out, out_end )`.  For this function to work,
`in` needs to precede `in_end` in what they point to in memory, and
`out` needs to precede `out_end` in the same way.  When the first range
is smaller than the second, the remainder of the second is left
untouched; when the first range is larger than the second, only the
part of the first range that can fit in the second range is copied.

```cpp
template <Regular T>
void copy(const T* in,  const T* in_end, T* out, T* out_end) {
  while (in != in_end && out != out_end) {
    *out = *in;
    ++in;
    ++out;
  }
}
```

This algorithm is simple to understand as code; it's so simple, that we
probably wouldn't even write this `copy` function in the first place,
and we'd likely just write it in the code directly.  We can see,
though, that by breaking this loop out into a separate function with
the name `copy` makes the calling code easier to understand:

```cpp
auto source      = std::array<T, 5>{};
auto destination = std::array<T, 10>{};

/*
  auto in      = &source[0];
  auto in_end  = in + size(source);
  auto out     = &destination[0];
  auto out_end = out + size(source);
  while (in != in_end && out != out_end) {
    *out = *in;
    ++in;
    ++out;
  }
*/

copy(
  &source[0],
  &source[0]      + size(source),
  &destination[0],
  &destination[0] + size(destination)
);
```

It's not perfect; we can do better.  But let's play with this idea for a
little bit and see if we can generalize it.  What if we want to copy
from a linked list to a linked list in the same way?

```cpp
template <Regular T>
void copy(const node<T>* in,
          const node<T>* in_end,
          node<T>*       out,
          node<T>*       out_end) {
  while (in != in_end && out != out_end) {
    out->value = in->value;
    in  = in->next;
    out = out->next;
  }
}
```

This is essentially the same algorithm, but we had to replace
the dereference operation with a `->value` and the increment with
an assignment of `->next`.  Otherwise, though, we essentially have the
same algorithm.  This makes sense, of course, as we have two data
structures that organize data in a sequence; we just need to change
our code to use the new data structure.

What if we want to take a block of memory and print it out into an
output stream like `cout`?  Take an input stream like `cin` and read
in an array?  Take an array and push its elements onto the end of a
`vector`?  The algorithms all look very similar:

```cpp
template <Regular T>
void copy(const T* in, const T* in_end, ostream& out) {
  while (in != in_end && out) {
    out << *in;
    ++in;
  }
}


template <Regular T>
void copy(const T* in, const T* in_end, vector<T>& out) {
  while (in != in_end && out) {
    out.push_back(*in);
    ++in;
  }
}


template <Regular T>
void copy(istream& in, T* out, T* out_end) {
  while (in && out != out_end) {
    in >> *out;
    ++out;
  }
}
```

An observation: we could continue writing these algorithms _ad
nauseam_ for all the combinations we didn't write: what if we want to
copy a linked list to a block of memory?  read a linked list in from a
stream?  copy an input stream to an output stream?  In general, if we
have 𝑚 sorts of ranges, we'll need 𝑚×𝑚 different `copy` functions.
That's not scalable, and it requires that the person writing the
`copy` functions know all 𝑚 sorts of ranges.  It's looking more and
more like having a `copy` function might be a little too much work to
maintain.

Another observation: these algorithms all look very similar to what we
saw before: we have a way to check whether we've reach the end of each
range, we have a way to get the value out of each range or put
something into each range, and we have a way to move to the next
element in the range—though for the latter two, we see them merged
into one operation.  It looks like we may have a concept here,
although we have different syntax for each type of range.  If we can
formalize this concept and unify the syntaxes into one, we may have a
shot at generalizing our `copy` function.

Essentially what we're doing in each of our examples is iterating
through a range of elements of the same type; we can call our concept
`Iterator`.  We can see that `Iterator`s need to be copyable and
comparable for equality, so it looks like `Iterator` is a refinement
of `Regular`.  In addition to regularity, it looks like `Iterator`s
necessarily provide an operation to increment them, to get their
successor.  For pointers, this is spelled `operator++`, for streams,
this is spelled `operator>>` or `operator<<`.

Finally, we'll note that our input range iterators and our output
range iterators have different requirements on them—`cin` is a valid
input range, but not a valid output range; similarly, `cout` is a
valid output range, but not a valid input range.  It looks like we
have necessarily two types of `Iterator`s: ones which you can read
from (we'll call these `InputIterator`s) and ones which you can write
to (we'll call these `OutputIterator`s).  An `InputIterator` has a
function that can give you back what the iterator is pointing to, and
an `OutputIterator` has a function that will let you write to whatever
the iterator is pointing to.

We have the general semantics of iterators down, but how do we spell
these semantics in the source code?  First, we'll notice that our
first example with C-style pointers is very simple to read and
understand.  Pointers separate out the two operations well and give
different syntaxes to them.  Compared to C-style pointers, there are
other extra things going on in each of the other examples.
Furthermore, because C-style pointers are built into the language, we
can't really add new member functions to them.  For these two reasons,
namely that the syntax is terse but expressive and not easily
changeable, we will use C-style pointer syntax for the basis of our
`Iterator` syntax.  This allows us to write the following rough
concepts for `InputIterator` and `OutputIterator`:

```cpp
template <typename I>
concept bool InputIterator =
  Regular<I> &&
  requires(I it) {
    // An InputIterator is Regular, and provides the following two
    // operations in addition:
    ++it;                                 // successor
    { *it }  -> const ValueType<I>&       // dereference
  };

template <typename I, typename T>
concept bool OutputIterator =
  Regular<I> &&
  requires(I it, T val) {
    // An OutputIterator is Regular, and provides the following two
    // operations in addition:
    ++it;                                 // successor
    { *it = val };                        // dereference
  };
```

C-style pointers satisfy these trivially; for other range types, we'll
have to create objects that overload `operator*` and `operator++` to
do what we want.  We'll see soon how the standard library actually
does that.

Now that we've defined our concepts, we can rewrite our `copy`
function template in terms of `InputIterator` and `OutputIterator`:

```cpp
template <InputIterator InIt, OutputIterator OutIt>
void copy(InIt in, InIt in_end, OutIt out, OutIt out_end) {
  while (in != in_end && out != out_end) {
    *out = *in;
    ++in;
    ++out;
  }
}

// Or with the shorthand (exactly the same semantics),
void copy(InputIterator  in,
          InputIterator  in_end,
          OutputIterator out,
          OutputIterator out_end) {
  while (in != in_end && out != out_end) {
    *out = *in;
    ++in;
    ++out;
  }
}
```

We now have only one implementation of `copy`, and when given proper
iterators, it subsumes all the `copy` implementations we saw before,
and more.  Instead of writing 𝑚×𝑚 different implementations, we need
to write 𝑚 iterators and 1 algorithm.

Now, generalizing just for the sake of generalizing is not the best
idea.  Consider the following algorithms, though:

```cpp
// Takes an input range and moves the elements to an output range.
void move(InputIterator  in,
          InputIterator  it_end,
          OutputIterator out,
          OutputIterator out_end) {
  while (in != in_out && out != out_end) {
    *out = std::move(*in);
    ++in;
    ++out;
  }
}


// Takes an input range and calls a function on each element, storing
// the results in an output range.
void transform(InputIterator  in,
               InputIterator  it_end,
               OutputIterator out,
               OutputIterator out_end,
               Function       f) {
  while (in != in_out && out != out_end) {
    *out = f(*in);
    ++in;
    ++out;
  }
}


// Takes an input range and copies at most n elements to an output
// range.
void copy_n(InputIterator  in,
            InputIterator  it_end,
            OutputIterator out,
            OutputIterator out_end,
            int            n) {
  while (n != 0 && in != in_out && out != out_end) {
    *out = *in;
    ++in;
    ++out;
    --n;
  }
}
```

All of these algorithms have the same concept requirements on their
arguments.  This tells us that we probably have useful and general
concepts here.  Moreover, we're starting to see the benefits of our
iterator abstraction: if we had 𝑚 sorts of ranges and 𝑛 different
algorithms, we would have had to write 𝑚×𝑚×𝑛 different function
templates—𝑚×𝑚 because we have two ranges as arguments, and extra
factor of 𝑛 for each algorithm.  This is certainly not maintainable,
and not extensible either, because if someone wants to write a new
sort of range, they'd need to make 𝑚×𝑛 new function implementations.
With our new design, we need to write 𝑚 iterators and 𝑛 algorithms, or
𝑚+𝑛 things total.  If someone makes a new range, they only need to
write an iterator that satisfies `InputIterator` and/or
`Outputiterator`.  If someone makes a new algorithm, they only need to
write one function template, and it will work on any type of
range—even ones they don't know about.

If we look at another algorithm, though, we'll start to see a problem:

```cpp
// Returns an iterator to the maximum element in a range.
InputIterator max_element(InputIterator begin, InputIterator end) {
  InputIterator max_it = begin;

  while (begin != end) {
    if (*begin > *max_it)
      max_it = begin;
  }

  return max_it;
}
```

Are the concept specifications on this algorithm correct?  This will
certainly work for C-style pointers, but will it work for something
like an input stream iterator?  With an input stream iterator (coming
from something like `cin`, maybe), we can iterate to each value only
once; there's no way of saving our state and going back to a previous
input value.  We could, of course, save all input we've seen, but that
would be extra overhead that we don't necessarily want (or need, as we
saw with algorithms like `copy` above).  It seems like stream
iterators are _single-pass_, so it doesn't make sense to save an
iterator to some earlier part of the range.  It seems we need a new
concept to describe the iterators for `max_element`, iterators which
are _multi-pass_.

If we can iterate through a range many times, it follows that we can
iterate through only once, as well.  _Multi-pass_ iterators are
strictly more powerful than _single-pass_ iterators.  As such, this
new multi-pass concept should refine `InputIterator`.  We call such
iterators `ForwardIterator`s, which require that you be able to save
an earlier iterator and iterate over the range again.  With
`max_element` above, we need the `max_it` iterator we save to stay
valid even as we iterate through the range looking for an iterator
that points to a greater value.  The corrected `max_element` is listed
below:

```cpp
// Returns an iterator to the maximum element in a range.
ForwardIterator max_element(ForwardIterator begin, ForwardIterator end) {
  ForwardIterator max_it = begin;

  while (begin != end) {
    if (*begin > *max_it)
      max_it = begin;
  }

  return max_it;
}
```

It turns out that the `upper_bound` function we saw last time also
takes in iterators that model `ForwardIterator`:

```cpp
ForwardIterator upper_bound(ForwardIterator begin,
                            ForwardIterator end,
                            WeakStrictlyOrdered value) {
  while (begin != end && *begin <= value) {
    ++begin;
  }

  return begin;
}
```

This is not a question of _constraints_, though—the above algorithm
would work just fine on `InputIterator`s—but rather one of algorithm
semantics: this algorithm takes in a sorted range and returns the
one-after-end iterator for elements that compare equal to `value`.
Paired with `lower_bound`, which gives you an iterator to the first
element in a sorted range that compares equal to `value`, we get a
subrange of all elements in a sorted range that compare equal to
`value`—but this only makes sense for `ForwardIterator`s, for which
you can iterate over that range again.

At this point, we have a hierarchy of iterator concepts:

     InputIterator
           |
    ForwardIterator

Looking at another algorithm still, we'll see this isn't quite enough,
either:

```cpp
// Reverses a sequence in-place (mutating operation)
template <typename It>
  requires OutputIterator<It> && ForwardIterator<It>
void reverse(It begin, It end) {
  --end;

  while (begin < end) {
    swap(*begin, *end);
    ++begin;
    --end;
  }
}
```

This algorithm makes sense, but We have a new `operator--` that we're
using.  `ForwardIterator`s don't necessarily support an `operator--`,
so our algorithm's concepts are improperly specified.  Intuitively, we
can see this: although we can easily make iterators for a
singly-linked list, we can't use this algorithm to reverse the order
of the elements in that list.  Again, we need to make a new concept,
one that allows us to traverse the range both in the forward
direction, like a normal `ForwardIterator`, and in the reverse
direction.  We call such an iterator a `BidirectionalIterator`, and we
use `operator--` to move a `BidirectionalIterator` in the opposite
direction.  We can correct the specification of our algorithm, and
then add this to the iterator concept hierarchy:

```cpp
// Reverses a sequence in-place (mutating operation)
template <typename It>
  requires OutputIterator<It> && BidirectionalIterator<It>
void reverse(It begin, It end) {
  --end;

  while (begin < end) {
    swap(*begin, *end);
    ++begin;
    --end;
  }
}
```

To save us the exercise of correcting the specification of another
algorithm, I'll mention that there's a final iterator concept called a
`RandomAccessIterator`.  These iterators define `operator+` and
`operator-` which are guaranteed to work in 𝒪(1) time, and which
increment or decrement an iterator by some offset, or which return the
offset between two iterators.  This allows us to optimize our previous
`upper_bound` using a binary search-like algorithm, and get 𝒪(log₂𝑛)
algorithmic complexity.  Our old `upper_bound` still works, so we can
still find the `upper_bound` on a sorted linked list, for example, but
now, for data structures like `vector` or `array` where iterators can
efficiently be incremented or decremented by arbitrary amounts, we can
use the more efficient version here:

```cpp
RandomAccessIterator upper_bound(RandomAccessIterator begin,
                                 RandomAccessIterator end,
                                 WeakStrictlyOrdered  value) {
  // Base case: range of zero size.  To add an element to this range
  // while maintaining its sorted order, we add it to where begin
  // points.
  if (begin == end)
    return begin;

  // middle_dist is always strictly less than distance(begin, end),
  // because of integer division
  auto middle_dist = (end - begin) / 2;
  I middle = begin + middle_dist;

  // Reduce problem size.
  if (*middle <= value)
    // If we're looking at an element smaller than or equal to value,
    // we're definitely not at the upper bound, so look to the right,
    // excluding the middle.
    return upper_bound(mid + 1, end, value);
  else
    // If we're looking at an element larger than value, we might be
    // at the upper bound, so look to the left, including the middle.
    return upper_bound(begin, mid + 1, value);
}
```

This gives us the iterator hierarchy shown below:

        InputIterator
              |
       ForwardIterator
              |
    BidirectionalIterator
              |
    RandomAccessIterator

__Discussion__: We've presented the iterator hierarchy as it currently
exists in the Standard Template Library, but improvements could be
made.  Are there any benefits to adding a more refined
`ContiguousIterator` concept, which guarantees that the elements in
the range are contiguous in memory?

A Note on Algorithms
--------------------

Although we presented the algorithms above to take in pairs of
iterator arguments, most standard library algorithms actually take in
only three iterator arguments: `in`, `in_end`, and `out`.  Instead,
these algorithms have a precondition that the second range is at least
as large as the first range.  It is still beneficial to think in terms
of "a range as a pair of iterators", as that will allow us later to
get much more safety than either the STL as it stands or the
alternative design we used to motivate iterators give.

Constructing Iterators
======================

Given a container like `vector`, `array`, or a C-style array, the
Standard Template library, provides two functions `begin()` and
`end()`, which return iterators to beginning of the container and to
one-past-the-end of the container, respectively.  As an example, we
could rewrite the call to `copy` that we saw in the above section more
clearly:

```cpp
auto source      = std::array<T, 5>{};
auto destination = std::array<T, 10>{};

copy(begin(source), end(source), begin(destination));
```

The standard library follows this convention for all containers (we'll
see more of those later).  To get iterators that are not
`OutputIterator`s even when the container is mutable, you can use
`cbegin()` and `cend()`.  Finally, when the container's iterators
model `BidirectionalIterator`, you can use `rbegin()` and `rend()`
(and `crbegin()` and `crend()`) to get iterators that traverse the
container from back to front.

For iterators that aren't backed by some container, the general
convention is that a default constructed iterator is the
one-past-the-end iterator, and constructing an iterator any other way
will give you the begin iterator.  For example, the input stream
iterator we discussed in the last section exists in the standard
library as [`std::istream_iterator<T>`][istream_iterator].  Similarly,
the output stream iterator is called
[`std::ostream_iterator<T>`][ostream_iterator].  We can combine these
together to make the `cat` program we wrote several lessons ago:

```cpp
#include <iostream>
#include <algorithm>
#include <iterator>

int main() {
  using namespace std;

  copy(istream_iterator<char>{ cin },
       istream_iterator<char>{},
       ostream_iterator<char>{ cout });
}
```

This program lacks all of the subtle problems we found when writing
the loop ourselves.  Although we can still do better in writing more
highly abstracted software that's easier to read, let's pause here
with this set of abstractions to really understand how they work,
which might give us a better sense of where we can make improvements
while preserving the benefits we've just gotten.

[istream_iterator]: http://en.cppreference.com/w/cpp/iterator/istream_iterator
[ostream_iterator]: http://en.cppreference.com/w/cpp/iterator/ostream_iterator

Containers
==========

The Standard Template Library provides the following sequence data
containers:

  - `std::array<T, N>`: a fixed-sized array of `N` elements of type
    `T`.  Iterators model `RandomAccessIterator`.
  - `std::vector<T>`: a growable array of `N` elements of type `T`.
    Iterators model `RandomAccessIterator`.
  - `std::deque<T>`: an array with 𝒪(1) insertion and removal at both
    the front and the back.  Iterators model `RandomAccessIterator`.
  - `std::list<T>`: a doubly-linked list.  Iterators model
    `BidirectionalIterator`.
  - `std::forward_list<T>`: a singly-linked list.  Iterators model
    `ForwardIterator`.

Additionally, the following data types provide iterator types that can
be used with standard library algorithms:

  - `std::string`: a string of characters.  Iterators model
    `RandomAccessIterator`.
  - `std::valarray<T>`: an ordered collection of values that supports
    element-wise mathematical operation and efficient slicing.
    Iterators model `RandomAccessIterator`.

The standard library also provides the following associative
containers and unordered associative containers.  We won't be talking
much about the design of these, but they are useful to know about, and
they provide iterators (which are guaranteed to be to a sorted range
in the normal associative containers), so they are listed here for
completeness:

  - `std::set<T>`/`std::unordered_set<T>`: a set of unique values of
    type `T`.
  - `std::multiset<T>`/`std::unordered_multiset<T>`: a set of
    (possibly non-unique) values of type `T`.
  - `std::map<K,V>`/`std::unordered_map<K,V>`: a map from unique
    values of type `K` to values of type `V`.
  - `std::multimap<K,V>`/`std::unordered_multimap<K,V>`: a map from
    (possibly non-unique) values of type `K` to values of type `V`.

Finally, the standard library provides three container adapters which
change the interface of an underlying sequence data structure, to make
them easier to use in certain contexts.  These containers do not
provide iterators, because they are not meant to be traversed.

  - `std::stack<T>`: A first-in, last-out data structure.
  - `std::queue<T>`: A first-in, first-out data structure.
  - `std::priority_queue<T>`: A data structure that provides 𝒪(1)
    access to its largest element.

The STL is a very extensible design, and you can find more interesting
containers in other libraries, such as in [Boost][boost-container].
These containers work with the standard algorithms as expected,
because they provide iterators that function as the glue between
algorithms and containers.

[boost-container]: http://www.boost.org/doc/libs/1_59_0/doc/html/container.html

Conservative Vector
-------------------

As an example, we will implement a type template called `cvector<T>`
which stores a dynamically-sized contiguous array of elements of type
`T`.  Unlike a `std::vector<T>`, our `cvector<T>` will grow by a
constant amount each time it needs to resize.  This will prevent us
from getting amortized 𝒪(1) time on `push_back`, but such a class
could still be useful, as it conserves memory better than a
`std::vector<T>`.  `cvector<T>` is not quite STL-compatible, as we'll
see in later lessons, but it will serve to illustrate the basic idea
of STL containers and iterators.

The code is presented below.  Please walk through this code and make
sure you understand it.  [cppreference][cppreference] is a great
resource for understanding this, and we are at the point now where we
can start to use it effectively.

```cpp
template <Regular T>
struct cvector {
  // Default construction:
  cvector();

  // Copy construction / assignment
  cvector(const cvector<T>&);
  cvector &operator=(const cvector<T>&);

  // Move construction / assignment
  cvector(cvector<T>&&);
  cvector &operator=(cvector<T>&&);

  // Destruction
  ~cvector();

  // Element insertion
  void push_back(const T&);

  // Element access
  T& operator[](int i);    // precondition: i in range

  // Size
  int size();

  // Iterators
  RandomAccessIterator begin();
  RandomAccessIterator end();

private:
  unique_PR<T[]> data;
  int             Len;
  int             cap;
};


template <Regular T>
cvector<T>::cvector()
  : data{ make_unique<T[]>(3) }    // make buffer of 3 elements
  , len{ 0 }                       // no elements in it
  , cap{ 3 }                       // with room for 3 elements
{}


template <Regular T>
cvector<T>::cvector(const cvector<T>& vec)
  : data{ make_unique<T[]>(vec.len) }
  , len{ vec.len }
  , cap{ vec.cap }
{
  copy(begin(*vec.data), end(*vec.data), begin(*data));
}


template <Regular T>
cvector<T> &cvector<T>::operator=(const cvector<T>& vec) {
  data = make_unique<T[]>(vec.len);
  len  = vec.len;
  cap  = vec.len;

  copy(begin(*vec.data), end(*vec.data), begin(*data));

  return *this;
}


template <Regular T>
cvector<T>::cvector(cvector<T>&& vec)
  : data{ move(vec.data) }
  , len{ vec.len }
  , cap{ vec.cap }
{}


template <Regular T>
cvector<T> &cvector<T>::operator=(cvector<T>&& vec) {
  data = move(vec.data);
  len  = vec.len;
  cap  = vec.len;

  return *this;
}


template <Regular T>
cvector<T>::~cvector() {}


template <Regular T>
void cvector<T>::push_back(const T& v) {
  // Grow the vector if necessary.
  if (len == cap) {
    cap += 3;
    auto new_data = make_unique<T[]>(cap);
    move(begin(*data), end(*data), begin(*new_data));
    data = move(new_data);
  }

  // Put the element in the right spot.
  data[len] = v;
  ++len;
}


template <Regular T>
T& cvector<T>::operator[](int i) {
  assert( i > 0 && i < len);
  return data[i];
}


template <Regular T>
int cvector<T>::size() { return len; }

template <Regular T>
int size(const cvector<T>& v) { return v.size(); }

template <Regular T>
RandomAccessIterator cvector<T>::begin() { return begin(*data); }

template <Regular T>
RandomAccessIterator begin(const cvector<T>& v) { return v.begin(); }

template <Regular T>
RandomAccessIterator cvector<T>::end() { return end(*data); }

template <Regular T>
RandomAccessIterator end(const cvector<T>& v) { return v.end(); }
```

By just providing `begin()` and `end()` functions which forward the
iterators from the underlying array, we have made a sequence container
that works with all algorithms in the standard library, as well as any
that other developers have made, as long as they follow the convention
of the standard library.

[cppreference]: http://cppreference.com/

Documentation
=============

I encourage you all to read up on the [iterators][iterator],
[containers][container], and [algorithms][algorithm] in the STL.  You
will almost always find that when you are writing a raw `for` or
`while` loop, you should be using a standard algorithm.

[iterator]: http://en.cppreference.com/w/cpp/iterator
[container]: http://en.cppreference.com/w/cpp/container
[algorithm]: http://en.cppreference.com/w/cpp/algorithm

You may also find the following article interesting:
[_Rolling Your Own Input Iterators_][input-iters].

[input-iters]: http://ksvanhorn.com/Articles/input_iter.html
