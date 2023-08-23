{full: true, community: true}
# Linked Lists

While rare in practical applications, linked lists crop up frequently in interviews. Partly this is because the node structure lends itself to formulating tricky problems, similar to trees and graphs, without the added topological complexity.

## *std::list* and *std::forward_list*

The standard library offers two list types, *std::list*{i: std::list} - a doubly-linked list and *std::forward_list*{i: std::forward\_list} - a singly-linked list.
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

Aside from iterator stability, we also get access to a suite of *O(1)* operations, and these can potentially outweigh the inherent overhead of a *std::list*.

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

{#custom_lists}
## Custom lists

When implementing a simple custom linked list, you might be tempted to use a straightforward implementation using a *std::unique_ptr*.

{caption: "A naive approach to a linked list."}
```cpp
#include <memory>

struct Node {
    int value;
    std::unique_ptr<Node> next;
};

std::unique_ptr<Node> head = std::make_unique<Node>(20,nullptr);
head->next = std::make_unique<Node>(42,nullptr);
// head->value == 20
// head->next->value == 42
```

<!-- https://compiler-explorer.com/z/Mr71dfKME -->


Sadly, this approach isn’t usable. The fundamental problem here is the design. We are mixing ownership with structural information. In this case, this problem manifests during destruction. Because we have tied the ownership with the structure, the destruction of a list will be recursive, potentially leading to stack exhaustion and a crash.

{caption: "A demonstration of a problem caused by recursive destruction."}
```cpp
#include <memory>

struct Node {
    int value;
    std::unique_ptr<Node> next;
};

{
std::unique_ptr<Node> head = std::make_unique<Node>(0,nullptr);
// Depending on the architecture/compiler, the specific number
// of elements we can handle without crash will differ.
Node* it = head.get();
for (int i = 0; i < 100000; ++i)
    it = (it->next = std::make_unique<Node>(0,nullptr)).get();
} // BOOM
```

<!-- https://compiler-explorer.com/z/jcca17d7b -->

{class: information}
B> The recursive nature comes from chaining *std::unique_ptr*. As part of destroying a *std::unique_ptr\<Node\>* we need first to destroy the nested next pointer, which in turn needs to destroy its nested next pointer, and so on.
B> A destruction of the linked list means a full expansion of destructors until we reach the end of the list.
B> After reaching the end of the list, we can finally finish the destruction of the trailing node, propagating back towards the front.
B> Each program has a limited stack space, and a sufficiently long naive linked list can easily exhaust this space.

If we desire both the *O(1)* operations and iterator stability, the only option is to rely on manual resource management (at which point we might as well use *std::list* or *std::forward_list*).

However, if we limit ourselves, there are a few alternatives to *std::list* and *std::forward_list*.

If we want to capture the structure of a linked list with reference stability, we can rely on the previously mentioned combination of a *std::vector* and a *std::unique_ptr*. This approach doesn't give us any *O(1)* operations or iterator stability; however, this approach is often used during interviews{i: "lists!custom simple"}.

{caption: "Representing the structure of a linked list using a std::vector and std::unique_ptr."}
```cpp
#include <vector>
#include <memory>

struct List {
    struct Node {
        int value;
        Node* next;
    };
    Node *head = nullptr;
    Node *new_after(Node* prev, int value) {
        nodes_.push_back(std::make_unique<Node>(value, nullptr));
        if (prev == nullptr)
            return head = nodes_.back().get();
        else
            return prev->next = nodes_.back().get();
    }
private:
    std::vector<std::unique_ptr<Node>> nodes_;
};


List list;
auto it = list.new_after(nullptr, 1);
it = list.new_after(it, 2);
it = list.new_after(it, 3);

// list.head->value == 1
// list.head->next->value == 2
// list.head->next->next->value == 3
```

The crucial difference from the naive approach is that the list data structure owns all nodes, and the structure is encoded only using weak pointers.

Finally, if we do not require stable iterators or references but do require *O(1)* operations, we can use a flat list approach.
We can store all elements directly in a *std::vector* and represent information about the next and previous nodes using indexes{i: "lists!custom flat"}.

However, this introduces a problem. Erasing an element from the middle of a *std::vector* is *O(n)* because we need to shift successive elements to fill the gap. Since we are encoding the list structure, we can swap the to-be-erased element with the last element and only then erase it in *O(1)*.

{caption: "Erase an element from the middle of a flat list in O(1)."}
```cpp
#include <vector>

inline constexpr ptrdiff_t nill = -1;

struct List {  
    struct Node {
        int value;
        ptrdiff_t next;
        ptrdiff_t prev;
    };
    ptrdiff_t new_after(ptrdiff_t prev, int value) {
        storage.push_back({value, nill, prev});
        if (prev != nill)
            storage[prev].next = std::ssize(storage)-1;
        else
            head = std::ssize(storage)-1;
        return std::ssize(storage)-1;
    }
    void erase(ptrdiff_t idx) {
        // move head
        if (idx == head)
            head = storage[idx].next;
        // unlink the erased element
        if (storage[idx].next != nill)
            storage[storage[idx].next].prev = storage[idx].prev;
        if (storage[idx].prev != nill)
            storage[storage[idx].prev].next = storage[idx].next;
        // relink the last element
        if (idx != std::ssize(storage)-1) {
            if (storage.back().next != nill)
                storage[storage.back().next].prev = idx;
            if (storage.back().prev != nill)
                storage[storage.back().prev].next = idx;
        }
        // swap and O(1) erase
        std::swap(storage[idx],storage.back());
        storage.pop_back();
    }
    ptrdiff_t get_head() { return head; }
    Node& at(ptrdiff_t idx) { return storage[idx]; }
private:
    ptrdiff_t head = nill;
    std::vector<Node> storage;
};


List list;
ptrdiff_t idx = list.new_after(nill, 1);
idx = list.new_after(idx, 2);
idx = list.new_after(idx, 3);
idx = list.new_after(idx, 4);
idx = list.new_after(idx, 5);
// list == {1,2,3,4,5}

idx = list.get_head();
list.erase(idx);
// list == {2,3,4,5}
```

<!-- https://compiler-explorer.com/z/on997o5zb -->