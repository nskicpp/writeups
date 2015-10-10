C++ Coding Standards
====================

This document is going to contain best practices we'll discover when
writing C++ code.  These should be considered guidelines, because all
guidelines need to be broken at some point.

Naming Conventions
------------------

Naming conventions tend to be subjective, and because of the lack of a
C++ community for a long time, there are several styles in use.
Recently, there has been a push to follow the the convention that most
of the C++ Standard Library and that Boost follow; this convention
actually has some nice benefits over other conventions, but we will be
following it because it's becoming the most common, and because we
have no reason to follow the other conventions.

I'll try to motivate some of it, but with naming, it's much harder to
do that.  Most of it boils down to the prevailing convention,
especially with new features.

This is a brief overview of them:

### Variables, functions, types

All variables, functions, and types should be named in lowercase, with
words separated by underscores.  This includes immutable variables
declared with `const` or `constexpr`!

We will see that there are times when we want to erase the difference
between type constructors, free functions, and variables.  This will
actually allow us to do some interesting techniques transparently to
the user of our code.  That transparency is not possible to get when
we make a naming distinction between these abstractions.

Including constants in this is explained later.

Some examples:

```cpp
class json_stream;
color from_hsl(int h, int s, int l);
int user_id = get_user_id(some_user);
const string header{"FILE FORMAT VERSION 1.0"s};
```

### Namespaces

Namespaces are not first-class in C++, so there are no cases where we
can substitute them with variables, functions, or types.  It is still
conventional to name them in lowercase with underscores.

Resist the urge to create large nested namespace hierarchies.
Namespaces aren't modules, so don't try to express modules in terms of
namespaces.

Some examples:

```cpp
namespace cipra {
namespace details { // for semantically private things that can't be private :(
/* ... */
}
}
```

### Template Parameters

Template parameters should either be single capital letters, starting
with `T` for type, `F` for `Callable`, or `N` for integers (strongly
preferred), or be named in CamelCase suffixed with a `T`.  This is a
slight modification to the common style.

Naming type parameters with a single letter is an _extremely_ common
style, to the point where there are few other styles, especially when
there are no non-obvious concept requirements on the type.

When there are strict concept requirements on the template parameter,
the current style is to use a CamelCase name that specifies the
concept, like `Integral`.  I require that we add a `T` suffix to it to
disambiguate from the actual concept name (for when Concepts Lite gets
added later this year).

Sometimes, when the template parameters have specific meanings that
aren't concepts, it may make sense to use another CamelCase name:

```cpp
template <typename KeyT, ValT> class map;
// is usually better than
template <typename T, typename U> class map;
```

Some examples:

```cpp
template <typename T> class vector;
template <typename IteratorT, typename PredicateT>
  bool all_of(IteratorT begin, IteratorT end, PredicateT pred);
template <typename T, typename U> U special_cast(const T&);
```

If you use concepts in your code, prefer to give the template
parameter name either a simple name like `T` or a name like `KeyT`
which gives more semantics than the concept alone.

### Concepts

Concept should be named in CamelCase.  Prefer nominal or adjectival
names for concepts.  The names should be evocative of the abstraction
they are describing.

Some examples:

```cpp
template <typename T>
concept bool Regular = requires (…) { … };

template <typename T>
concept bool Hashable = requires (…) { … };

template <typename F>
concept bool UnaryPredicate = requires (…) { … };
```
