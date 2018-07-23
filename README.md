# Sagacious C++

Modern C++ is sagacious.

## Move semantics
### Rvalue references
## Functional programming
### Lambda expressions

```
[capture](params)->return_type { body }
```

## Regexps
## Smart pointers
### RAII
The RAII pattern is a way of simplifying lifetime management of resources, such as heap memory, database connections, sockets, or files. 

```cpp
template <class T> class managed_ptr
{
public:
    managed_ptr(T* p) : _ptr{p} {}
    managed_ptr() : _ptr{new T} {}
    managed_ptr(const managed_ptr&) = delete;
    ~managed_ptr() { delete _ptr; }

    T* operator ->() const { return _ptr; }

private:
    T* const _ptr;
};
```

(The `= delete` specifier introduces, what is known as a, *deleted function declaration*; and it is discussed under [Deleted and defaulted functions](#deleted-and-defaulted-functions).)

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

```
pants created
pants worn
pants destroyed
```

## Memory model
## Initializer lists
## Type inference
### `auto` keyword
### `decltype` keyword
## Deleted and defaulted functions
## nullptr
## Streams
### C++14 Lazy streams
<!-- https://www.infoq.com/news/2014/07/cpp14-streams-lazy-functional -->
## Common patterns
### Class interfaces
## Algorithms library
