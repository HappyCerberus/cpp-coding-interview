{class: part, full: true, community: true}
# Linked Lists

While rare in practical applications, linked lists crop up frequently in interviews. Partly this is because the node structure lends itself to formulating tricky problems, similar to trees and graphs, without the added topological complexity.

## *std::list* and *std::forward_list*

The standard library offers two list types, *std::list* - a doubly-linked list and *std::forward_list* - a singly-linked list.
The *std::forward_list* exists primarily as a space optimization, saving 8 bytes per element on 64-bit architectures.

Both offer perfect iterator and reference stability, i.e., the only operation that invalidates iterators or references is the erasure of an element, and only for the removed element. The stability does extend even to moving elements between lists.

{caption: "Iterator stability with *std::list*."}
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

{caption: "Reference stability using *std::unique_ptr*."}
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


## Custom lists

When implementing a custom linked list, you might be tempted to implement a singly-linked list like this:

Sadly, this approach isn't quite correct. The approach exhibits a design problem; we mix resource management with structural information. In this case, this problem manifests during destruction. Since we have tied the ownership to the list structure, the destruction will be recursive, potentially running out of stack space and leading to a crash.

In the case of a linked list, we don't have the option of working around manual resource management as long as we want to maintain O(1) complexity for splicing and iterator and reference stability.

We can replicate the directional structure with reference stability without O(1) splicing operations using a std::vector of std::unique_ptr.