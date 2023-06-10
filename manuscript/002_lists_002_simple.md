{full: true, community: true}
## Simple operations

Let's explore some basic operations frequently used as the base for a more complex solution.
The three most frequent operations are:

- merge two sorted lists
- reverse a list
- scan with two pointers

Both *std::list* and *std::forward_list* come with a built-in merge operation.

{caption: "Merging two sorted lists using merge()."}
```cpp
#include <list>
#include <forward_list>

{
std::list<int> left{2,4,5};
std::list<int> right{1,3,9};
left.merge(right);
// left == {1,2,3,4,5,9}
// right == {}
}

{
std::forward_list<int> left{2,4,5};
std::forward_list<int> right{1,3,9};
left.merge(right);
// left == {1,2,3,4,5,9}
// right == {}
}
```

<!-- https://compiler-explorer.com/z/q7z5r4PxY -->

However, implementing one from scratch isn’t particularly complicated either. We consume the merged-in list, one element at a time, advancing the insertion position as needed{i: "lists|merge"}.

{caption: "Custom merge operation."}
```cpp
#include <forward_list>

std::forward_list dst{1, 3, 5, 6};
std::forward_list src{2, 4, 7};

auto dst_it = dst.begin();

while (!src.empty()) {
    if (std::next(dst_it) == dst.end() || 
        *std::next(dst_it) >= src.front()) {
        dst.splice_after(dst_it, src, src.before_begin());
    } else {
        ++dst_it;
    }
}
// dst == {1,2,3,4,5,6,7}
// src == {}
```

<!-- https://compiler-explorer.com/z/M1KP7hGo9 -->

The same situation applies to reversing a list. Both lists provide a built-in in-place reverse operation{i: "lists|reverse"}.

{caption: "Built-in in place reverse."}
```cpp
#include <forward_list>

std::forward_list<int> src{1,2,3,4,5,6,7};

src.reverse();
// src == {7,6,5,4,3,2,1}
```

Implementing a custom reverse is straightforward if we use a second list. However, the in-place version can be tricky.

{caption: "Custom implementations of linked list reverse."}
```cpp
#include <forward_list>
#include <iostream>

std::forward_list<int> src{1,2,3,4,5,6,7};

// Custom reverse using a second list
std::forward_list<int> dst;
while (!src.empty())
    dst.splice_after(dst.before_begin(), src, src.before_begin());
// dst == {7,6,5,4,3,2,1}
// src == {}

// Custom in-place reverse
auto tail = dst.begin();
if (tail != dst.end())
while (std::next(tail) != dst.end())
    dst.splice_after(dst.before_begin(), dst, tail);
// dst == {1,2,3,4,5,6,7}
```
<!-- https://compiler-explorer.com/z/73cxch9Y6 -->

The in-place reverse takes advantage of the fact that the first element will be the last once the list is reversed.

![Process of reversing a forward list in place.](linked_list/list_reverse_01.png)

Finally, scanning with two iterators is a common search technique for finding a sequence of elements that conform to a particular property. As long as this property is calculated strictly from elements entering and leaving the sequence, we do not need to access the elements currently in the sequence{i: "two pointers|sliding window"}.

{caption: "Find the longest subsequence with sum less than 4."}
```cpp
#include <forward_list>

std::forward_list<int> data{4,2,1,1,1,3,5};

// two iterators denoting the sequence [left, right)
auto left = data.begin();
auto right = data.begin();
int sum = 0;
int len = 0;
int max = 0;

while (right != data.end()) {
    // extend right, until we break the property
    while (sum < 4 && right != data.end()) {
        max = std::max(max, len);
        ++len;
        sum += *right;
        ++right;
    }
    // shrink from left, until the property is restored
    while (sum >= 4 && left != right) {
        sum -= *left;
        --len;
        ++left;
    }
}
// max == 3, i.e. {1,1,1}
```

<!-- https://compiler-explorer.com/z/4MT3Tez5z -->