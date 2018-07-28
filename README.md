# Sagacious C++

Modern C++ can be sagacious.

## Move semantics
### Rvalue references
Expressions in C++ are always either an *lvalue*, or an *rvalue*.
* Lvalues are named objects who may last beyond a single expression. All variables are lvalues.
* Rvalues are temporaries. The lifetime of an rvalue is limited to the expression in which it is defined.

```cpp
int x = 1 + 1;
```

```cpp
1 + 1 = x;
```

```
error: lvalue required as left operand of assignment
     1 + 1 = x;
           ^
```

## Functional programming
### Lambda expressions

```
[capture](params)->return_type { body }
```

map

```cpp
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

template <typename Dom, typename Cod>
std::vector<Cod> map(const std::vector<Dom>& xs, std::function<Cod(const Dom)> op)
{
    std::vector<Cod> ys;
    ys.resize(xs.size());
    std::transform(xs.begin(), xs.end(), ys.begin(), op);
    return ys;
}

int main()
{
    std::vector<int> xs{10, 20, 30, 40, 50, 60, 70, 80};

    auto ys = map<int, double>(xs, [](const int i) { 
        return sqrt(i); 
    });
    std::for_each(ys.begin(), ys.end(), [](const double& r) { 
        std::cout << r << std::endl; 
    });
}
```

## Regexps
Regular expressions are part of the C++11 standard.

```cpp
#include <regex>
```

## Smart pointers
In line with modern C++ best practices, direct use of `new` and `delete` in client code [should be avoided](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rr-newdelete). Passing around raw pointers makes code more unpredictable and exposes unnecessary low-level details to the users of a class. A smart pointer is a data type that emulates the behavior of a pointer (by encapsulating the raw pointer type) and automatically manages allocation and deallocation of the memory it is responsible for. Smart pointers have been around for some time in libraries like Boost, and in the Qt framework. In C++11, the `shared_ptr`, `unique_ptr`, and `weak_ptr` class templates have been promoted to first class citizens of the standard library.
### RAII
Smart pointers implement the RAII idiom, which is a way of simplifying lifetime management of resources; such as memory, database connections, sockets, or files. RAII, which stands for *Resource Acquisition Is Initialization*, delegate the responsibility of releasing the resource, when it is no longer needed, to objects of the RAII class. These objects become *owners* of the resource in question.

```cpp
class raii_object
{
    raii_object() { /* acquires some resource */ }
    ~raii_object() { /* ... clean up */ }
}

void fun()
{
    raii_object obj;
    
    // ...
}
```

In the above example, since `obj` has [automatic storage duration](http://www.enseignement.polytechnique.fr/informatique/INF478/docs/Cpp/en/c/language/automatic_storage_duration.html), both its scope and its lifetime are limited by the block in which it appears. 

```cpp
template <typename T> class managed_ptr
{
public:
    managed_ptr(T* p) : _ptr{p} {}
    managed_ptr() : _ptr{new T} {}
    managed_ptr(const managed_ptr&) = delete;
    managed_ptr& operator=(const managed_ptr&) = delete;
    ~managed_ptr() { delete _ptr; }

    T* operator->() const { return _ptr; }
    T& operator*() const { return *_ptr; }

private:
    T* const _ptr;
};
```

(The `= delete` specifier introduces, what is known as, a *deleted function declaration*. This syntax is discussed under [Deleted and defaulted functions](#deleted-and-defaulted-functions).)

Here is how we can use the `managed_ptr` class.

```cpp
#include <iostream>

using std::cout;
using std::endl;

class pants
{
public:
    pants() { cout << "pants created" << endl; }
    ~pants() { cout << "pants destroyed" << endl; }
    void wear() { cout << "pants worn" << endl; }
};

static void wear_pants()
{
    managed_ptr<pants> pants;
    pants->wear();
}

int main()
{
    wear_pants();
    return 0;
}
```

Note that through overloading the `*` and `->` operators we can dereference the `managed_ptr` type just as if it was an ordinary pointer. The output of this program is:

```
pants created
pants worn
pants destroyed
```

The `pants` object is automatically destroyed when the `managed_ptr` goes out of scope. To illustrate why this is beneficial, here is an example where a raw pointer is used instead:

```cpp
static void casually_wear_pants()
{
    pants* the_pants = new pants;
    if (the_pants->are_too_tight()) {
        return;                          // Oops! We forgot to delete the_pants
    }
    the_pants->wear();
    delete the_pants;
}
```

The above code introduces a memory leak. In this case, the problem is easy to spot, and fixed merely by inserting `delete the_pants` before the `return` statement. But consider something a bit more subtle, such as the following program.

```cpp
struct tight_pants_exception : public std::exception
{
    const char* what() const throw() { return "too tight"; }
};

class pants
{
public:
    pants() { cout << "pants created" << endl; }
    ~pants() { cout << "pants destroyed" << endl; }
    void wear();
    bool too_tight() const { return true; }
};

void pants::wear() 
{ 
    if (too_tight()) {
        throw tight_pants_exception{};
    } else {
        cout << "pants worn" << endl; 
    }
}

static void casually_wear_pants()
{
    pants* the_pants = new pants;
    the_pants->wear();
    delete the_pants;
}

int main()
{
    try {
        casually_wear_pants();
    } catch (tight_pants_exception& e) {
        cout << e.what() << endl;
    }
    return 0;
}
```

```
pants created
too tight
```

This time, the problem is not as easy to detect.

```cpp
static void casually_wear_pants()
{
    managed_ptr<pants> pants;
    pants->wear();
}
```

```
pants created
pants destroyed
too tight
```

The kind of smart pointer described here is know as [`unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr) in the C++11 standard library and as `scoped_ptr` in Boost. The `unique_ptr` is *unique* in the sense that it holds the *only reference* to the memory it is pointing to. To contrast this, let's look at what it means if we were to allow for this reference to be shared between objects. 

## Memory model
### std::thread
### std::async
## Initializer lists
## Type inference
### `auto` keyword
### `decltype` keyword
## Deleted and defaulted functions
## nullptr
## constexpr
## Fixed width integer types
## Streams
### C++14 Lazy streams
<!-- https://www.infoq.com/news/2014/07/cpp14-streams-lazy-functional -->
## Common patterns
### Class interfaces
## Algorithms library
## Style
