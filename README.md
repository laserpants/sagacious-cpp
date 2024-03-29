# Sagacious C++

Modern C++ can be sagacious.

## Move semantics
### Rvalue references
C++ distinguishes between lvalues and rvalues. This goes back to C, and the idea of an lvalue as one that can appear on the *left*-hand side of an assignment. 
```cpp
int x = 1 + 1; // here, x is an lvalue, and 1 + 1 an rvalue
```
Unfortunately, the left-right analogy doesn't quite hold up, and with the introduction of rvalue *references*, things get even more complicated. As a starting point for this discussion, we will think of it in the following way:
* Lvalues are named objects that last longer than a single expression (as a useful mnemonic, think ‘l’ as in lingering :smiley:). All *variables* are lvalues.
* Rvalues are temporaries. The lifetime of an rvalue is limited to the expression in which it is defined. 

Rvalues cannot be used as a left operand of an assignment. The following will therefore *not* work:
```cpp
1 + 1 = x;
```
My compiler says:
```
error: lvalue required as left operand of assignment
     1 + 1 = x;
           ^
```
An lvalue *reference* creates an alias for an existing object.

```cpp
int x = 5;
int& y = x;  // y refers to x
y = 0;
cout << x << endl;  // 0
```

The lvalue reference declarator (`&`) is a syntactic addition introduced in C++. It doesn't exist in C (though pointers can be used in C to achieve the same result). In a function signature, the &-declarator expresses that an argument should be passed by reference:

```cpp
static void do_stuff_with(int& x)
{
    x = 42;
}

int main()
{
    int x = 4;
    do_stuff_with(x);
    cout << x << endl;
    return 0;
}
```

The output of this program is `42`.

const ref

Lvalue references and pointers have similar behavior. They are mostly interchangeable, but there are some key differences to be aware of:

* Unlike pointers; once a reference has been created, it cannot be changed to reference some other object.
```cpp
int m = 1;
int n = 5;
int* p = &m;
p = &n;     // I changed my mind.

int& r = m; // There is no corresponding syntax here
// r = n    // Results in the value of n (5) being assigned to m
// &r = n   // Just give up! :)
```
* References must be initialized when created, and cannot be uninitialized.
```cpp
int* p;  // okay
int& x;  // not okay
```
* There is no such thing as a null reference. A reference can be invalid, but the programmer will have to go to some lengths to create one.
```cpp
int* p = new int{123};
int& x = *p;           // x is a local reference to the "pointee" of p
x = 5;
 
cout << *p << endl;    // 5
 
delete p;
p = nullptr;           // p is now a nullptr
 
if (p) {               // We can't do this with a reference
   *p = 1;
}
 
x = 3;                 // Undefined behavior
```
* Pointers are subject to arithmetic manipulation.
```cpp
int vals[11] = {1, 1, 1, 1, 100, 1, 1, 1, 1, 1};

int* p = vals;
int* q = p + 4;

int& r = vals[0];
int& s = vals[3];   // r + 4 doesn't work
```
* A pointer can point to another pointer. In other words, pointers allow for multiple levels of indirection, while you cannot have a reference to a reference. 
```cpp
int* p = new int{1234};
int** q = &p;
int*** r = &q;
int**** s = &r;
cout << ****s << endl; // 1234
```
  You can, however, have a reference to a pointer (*&).
```cpp
{
    int* p = new int{1};
    cout << *p << endl;        // 1
    int* q = p;
    q = new int{2};            
    cout << *p << endl;        // 1
}
{
    int* p = new int{1};
    cout << *p << endl;        // 1
    int*& q = p;
    q = new int{2};
    cout << *p << endl;        // 2
}
```
The use of references leads to more a declarative programming style, which is safer, but less powerful than equivalent code using pointers.

An *rvalue reference* is...

<!--
```cpp
#include <cstring>
#include <iostream>
#include <memory>

using std::cout;
using std::endl;

static int created = 0;
static int destroyed = 0;

class buffer
{
public:
    buffer();
    ~buffer();
    buffer(const buffer& other);
    buffer& operator=(const buffer& other);

private:
    size_t size;
    int* data;
};

// Constructor
buffer::buffer() : size{1024}, data{new int[size]}
{
    created++;
}

// Destructor
buffer::~buffer()
{
    if (nullptr != data) {
        delete[] data;
        data = nullptr;
        size = 0;
    }
    destroyed++;
}

// Copy constructor
buffer::buffer(const buffer& other) : size{other.size}, data{new int[size]}
{
    memcpy(data, other.data, size * sizeof(int));
    created++;
}

// Copy assignment operator
buffer& buffer::operator=(const buffer& other)
{
    if (&other != this) {
        buffer copy(other);
        std::swap(size, copy.size);
        std::swap(data, copy.data);
    }
    return *this;
}

static buffer make_buffer()
{
    buffer buf;
    return buf;
}

int main()
{
    {
        buffer buf = make_buffer();
    }

    cout << "created   : " << created   << endl;
    cout << "destroyed : " << destroyed << endl;

    return 0;
}
```

```
created   : 1
destroyed : 1
```

```cpp
static buffer make_buffer(const int n = 2)
{
    if (n > 1) {
        buffer buf;
        return buf;
    }

    buffer buf;
    return buf;
}
```

```
created   : 2
destroyed : 2
```

```cpp
class buffer
{
public:
    // ...
    buffer(buffer&& other);
    buffer& operator=(buffer&& other);

    // ...
};
 
// Move constructor
buffer::buffer(buffer&& other)  
{
    size = other.size;
    data = other.data;
    other.size = 0;
    other.data = nullptr;
}

// Move assignment operator
buffer& buffer::operator=(buffer&& other)  
{
    std::swap(size, other.size);
    std::swap(data, other.data);
    return *this;
}
```
-->

```cpp
#include <iostream>

using std::cout;
using std::endl;

class classic
{
public:
    classic() : _data{new int[1024]} { cout << "allocating resource" << endl; }

    virtual ~classic() 
    { 
        if (_data) {
            delete _data;
            _data = nullptr;
            cout << "releasing resource" << endl; 
        }
    }

    classic(const classic& other) : _data{new int[1024]}
    { 
        std::copy(other._data, other._data + 1024, _data);  
        cout << "allocating resource (copying)" << endl; 
    }

    classic& operator=(const classic& other)
    { 
        if (&other != this) {
            classic copy(other);
            std::swap(_data, copy._data);
        }
        return *this; 
    }

protected:
    int* _data;
};

class modern : public classic
{
public:
    modern() = default;

    modern(modern&& other) 
    { 
        _data = other._data;
        other._data = nullptr;
        cout << "moving resource" << endl; 
    }

    modern& operator=(modern&& other) 
    { 
        if (&other != this) {
            std::swap(_data, other._data);
            cout << "moving resource (assignm.)" << endl; 
        }
        return *this; 
    }
};

class classic_gadget
{
public:
    void set_gizmo(const classic& gizmo) { _gizmo = gizmo; }

private:
    classic _gizmo;
};

class modern_gadget
{
public:
    void set_gizmo(modern&& gizmo) { _gizmo = std::move(gizmo); }

private:
    modern _gizmo;
};

int main()
{
    {
        cout << "[1]" << endl;
        classic_gadget g;
        cout << "[2]" << endl;
        g.set_gizmo(classic{});
        cout << "[3]" << endl;
    }
    {
        cout << "[4]" << endl;
        modern_gadget g;
        cout << "[5]" << endl;
        g.set_gizmo(modern{});
        cout << "[6]" << endl;
    }
    return 0;
}
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
In line with modern C++ best practices, direct use of `new` and `delete` in client code [should be avoided](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rr-newdelete). Passing around raw pointers makes code more unpredictable and exposes unnecessary low-level details to the outside world. A smart pointer is a data type that emulates the behavior of a pointer (by encapsulating the raw pointer type) but automatically manages allocation and deallocation of the memory it is responsible for. The memory management technique introduced by smart pointers constitutes a basic form of [garbage collection](https://en.wikipedia.org/wiki/Garbage_collection_(computer_science)). (Sophisticated garbage collection algorithms often need to be a lot "smarter" to be efficient.) Smart pointers have been around for some time in libraries like Boost, and in the Qt framework. With C++11, the `shared_ptr`, `unique_ptr`, and `weak_ptr` class templates were added to the standard library.
### RAII
Smart pointers implement the RAII idiom, which is a way of simplifying lifetime management of resources, more generally. These resources may be memory, database connections, sockets, or files; for example. RAII, which stands for *Resource Acquisition Is Initialization*, delegates the responsibility of releasing the resource, when it is no longer needed, to objects of a class specially designed for this purpose. These objects take *ownership* of the resource in question.

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

Note that we do not allow these objects to be copied. More about this later. (The `= delete` specifier introduces, what is known as, a *deleted function declaration*. This syntax is discussed under [Deleted and defaulted functions](#deleted-and-defaulted-functions).)

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

The `pants` object is automatically destroyed when the `managed_ptr` goes out of scope. To illustrate why this is beneficial, here is another example where a raw pointer is used instead:

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

The above code induces a memory leak. In this case, the problem is easy to spot, and fixed merely by inserting `delete the_pants` before the `return` statement. But consider something a bit more subtle, such as the following program.

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

Again, the `delete` call is never reached, but this time the problem is not as easy to detect. Using the `managed_ptr` approach, the code would look like this:

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

```cpp
void bad()
{
    int* p = new int{1};

    std::unique_ptr<int> foo{p};
    std::unique_ptr<int> baz{p};  // Bad idea!
}
```

```
*** Error in `./main': double free or corruption (fasttop): 0x000000000244ec20 ***
```

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
## Working with JSON
## Style
