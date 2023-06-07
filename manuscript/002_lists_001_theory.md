{class: part, full: true, community: true}
# Linked Lists

While rare in practical applications, linked lists crop up frequently in interviews. Partly this is because the node structure lends itself to formulating tricky problems, similar to trees and graphs, without the added topological complexity.

## *std::list* and *std::forward_list*

The standard library offers two list types, *std::list* - a doubly-linked list and *std::forward_list* - a singly-linked list.
The *std::forward_list* exists primarily as a space optimization, saving 8 bytes per element on 64-bit architectures.

Both offer perfect iterator and reference stability, i.e., the only operation that invalidates iterators or references is the erasure of an element, and only for the removed element. The stability does extend even to moving elements between lists.

{caption: "Iterator stability with std::list."}
```cpp
#include <list>

std::list<int> first{1,2,3};
std::list<int> second{4,5,6};

// Get iterator to the element with value 2.
auto it = std::next(first.begin());

// Move the element to the begining of the second list.
second.splice(second.begin(), first, it);

// first == {1, 3}, second == {2,4,5,6}

// iterator still valid
// *it == 2
```
<!-- https://compiler-explorer.com/z/av4Wzaq5T -->

The iterator stability is one of the use cases where we would use a *std::list* or *std::forward_list* in practical applications. The only reasonable alternative would be wrapping each element in a *std::unique_ptr*, which does offer reference stability (but not iterator stability) irrespective of the wrapping container.

{caption: "Reference stability using std::unique_ptr."}
```cpp
#include <vector>
#include <memory>

std::vector<std::unique_ptr<int>> stable;
stable.push_back(std::make_unique<int>(1));
stable.push_back(std::make_unique<int>(2));
stable.push_back(std::make_unique<int>(3));

// get a stable weak reference (or pointer) to an element
int *it = stable[1].get();
stable.erase(stable.begin()); // invalidates all iterators
// it still valid, *it == 2
```
<!-- https://compiler-explorer.com/z/bs7YWbzK7 -->

Of course, we do pay for this stability with performance. Linked lists are node-based containers, meaning each element is allocated in a separate node, potentially very distant from each other in memory. When we combine this with the inherent overhead of the indirection, traversing a *std::list* can regularly end up 5x-10x slower than an equivalent flat *std::vector*.

Aside from iterator stability, we also get access to a suite of `O(1)`$ operations, and these can potentially outweigh the inherent overhead of a *std::list*.

{caption: "O(1) operations using a std::list and std::forward_list."}
```cpp
#include <list>

std::list<int> data{1,2,3,4,5};

// O(1) splicing between lists, or within one list

// effectively rotate left by one element
data.splice(data.end(), data, data.begin());
// data == {2,3,4,5,1}

// O(1) erase

// iterator to element with value 4
auto it = std::next(data.begin(), 2);
data.erase(it);
// data == {2,3,5,1}

// O(1) insertion

// effectively push_front()
data.insert(data.begin(), 42);
// data = {42,2,3,5,1}
```
<!-- https://compiler-explorer.com/z/GbesEWY73 -->

Because *std::list* is a bidirectional range and *std::forward_list* is a forward range, we lose access to some standard algorithms.
Both lists expose custom implementations of *sort*, *unique*, *merge*, *reverse*, *remove*, and *remove_if* as member functions.

{caption: "List specific algorithms."}
```cpp
#include <list>

std::list<int> data{1,2,3,4,5};

data.reverse();
// data = {5,4,3,2,1}

data.sort();
// data = {1,2,3,4,5}

data.remove_if([](int v) { return v % 2 == 0; });
// data == {1,3,5}
```
<!-- https://compiler-explorer.com/z/hbT51zoc7 -->

The *std::forward_list* has an additional quirk; since we can only erase and insert after an iterator, the *std::forward_list* offers a modified interface.

{caption: "Modified interface of std::forward_list."}
```cpp
#include <forward_list>

std::forward_list<int> data{1,2,3,4,5};

// before_begin() iterator
auto it = data.before_begin();

// insert and erase only possible after the iterator
data.insert_after(it, 42);
// data == {42,1,2,3,4,5}
data.erase_after(it);
// data == {1,2,3,4,5}
```
<!-- https://compiler-explorer.com/z/sozrMh8MT -->

## Custom lists

When implementing a custom linked list, you might be tempted to implement a singly-linked list like this:

Sadly, this approach isn't quite correct. The approach exhibits a design problem; we mix resource management with structural information. In this case, this problem manifests during destruction. Since we have tied the ownership to the list structure, the destruction will be recursive, potentially running out of stack space and leading to a crash.

In the case of a linked list, we don't have the option of working around manual resource management as long as we want to maintain O(1) complexity for splicing and iterator and reference stability.

We can replicate the directional structure with reference stability without O(1) splicing operations using a std::vector of std::unique_ptr.