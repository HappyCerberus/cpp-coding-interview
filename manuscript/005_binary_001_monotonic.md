{full: true, community: true}
# Binary search, Divide and Conquer

In this chapter, we will expand on the idea of dividing a search space, which we have touched upon in the Tree chapter.

Consider a binary search tree.

As a reminder, a binary search tree is a tree where the values of nodes in the left child are always lower than the parent node, and the values of nodes in the right child are always higher or equal to the parent node.

The structure of a binary search tree gives us a straightforward algorithm for looking up a specific value. If the value is equal to the current node, we have found our node; if the value is lower than the current node, we explore the left child, and if it is higher, we explore the right child.

{width: "30%"}
![Demonstration of binary search using a binary search tree.](binary_search/tree_search.png)

For a balanced tree, this will lead to *log(n)* lookup complexity as we divide our search space into half at each node.

## Binary search on monotonic sequences

Instead of working with a tree, we can apply the same logic to a sorted sequence of elements. Or more formaly a non-decreasing or non-increasing sequence of elements.

If we want to divide the number of elements we are searching through into half at each step, we need to compare against the middle element, which will point us to the left or right half of the sequence.

{Caption: "Straightforward implementation of binary search in a contiguous range."}
```cpp
bool contains(std::span<int64_t> data, int64_t value) {
    auto begin = data.begin();
    auto end = data.end();
    while (begin != end) {
        // Determine the midpoint
        auto mid = begin + (end-begin)/2;
        // We have found our value
        if (*mid == value)
            return true;
        // The value can only be in the right half
        if (*mid < value)
            begin = std::next(mid);
        // The value can only be in the left half
        if (*mid > value)
            end = mid;
    }
    return false;
}
```

<!-- https://compiler-explorer.com/z/Ms6Gjxxhn -->

Fortunately, we do not have to implement binary search from scratch since the standard library contains a comprehensive set of binary search algorithms.

### *std::partition_point*

It may seem strange that we are starting with the *std::partition_point* algorithm. However, *std::partition_point* is the fundamental binary search algorithm, and we can implement all other binary search algorithms in terms of *std::partition_point*.

The *std::partition_point* will binary search for the first element in a partitioned range that does not satisfy the given predicate. For example, if we want to be binary search for a specific value, we can search using the predicate *e < value*. *std::partition_point* will find the first element that is not lower than the value, which means the element will be higher or equal.

{Caption: "Binary search implemented in terms of std::partition_point."}
```cpp
bool binary_search(std::span<int> data, int value) {
    auto it = std::partition_point(
        data.begin(), data.end(), 
        [value](int e) {
            return e < value;
        });
    return it != data.end() && *it == value;
}
```

<!-- https://compiler-explorer.com/z/YvT4adnqG -->

### *std::lower_bound*

The binary search for the first element that is not lower than the given value has a named algorithm, *std::lower_bound*.

{Caption: "Finding the first element not ordered before the value using std::lower_bound.}
```cpp
std::vector<int> data{1,2,3,4,5,5,5,6,6,7,8,9};

auto it = std::lower_bound(data.begin(), data.end(), 5);
// *it == 5
// it - data.begin() == 4
```

<!-- https://compiler-explorer.com/z/MPdfs198z -->

### *std::upper_bound*

The other important element is the first one ordered after the given value (i.e. *value < e*), which we can search for using the *std::upper_bound* algorithm.

{Caption: "Finding the first element ordered after the value using std::upper_bound.}
```cpp
std::vector<int> data{1,2,3,4,5,5,5,6,6,7,8,9};

auto it = std::upper_bound(data.begin(), data.end(), 5);
// *it == 6
// it-data.begin() == 7
```

<!-- https://compiler-explorer.com/z/YWKTMsMde -->

### *std::equal_range*

If we invoke *std::lower_bound* and *std::upper_bound* using the same value, we end up with a range of elements equal to the given value. However, we can avoid the two-step process and directly call the *std::equal_range* algorithm, which returns a pair of lower and upper bounds.

{Caption: "Using std::equal_range to obtain both the lower and upper bounds."}
```cpp
std::vector<int> data{1,2,3,4,5,5,5,6,6,7,8,9};

auto [lower, upper] = std::equal_range(data.begin(), data.end(), 5);
// *lower == 5, *upper = 6
// lower - data.begin() == 4
// upper - data.begin() == 7
```

<!-- https://compiler-explorer.com/z/zMfMj6cv3 -->

### *std::binary_search*

The final algorithm in the family of binary searches is a simple presence check algorithm, *std::binary_search*.

{Captions: "Checking for presence of values in a sorted range using std::binary_search."}
```cpp
std::vector<int> data{1,2,3,4,6,6,7,8,9};

bool t1 = std::binary_search(data.begin(), data.end(), 5);
// t1 == false
bool t2 = std::binary_search(data.begin(), data.end(), 6);
// t2 == true
```

<!-- https://compiler-explorer.com/z/h74x19GG9 -->