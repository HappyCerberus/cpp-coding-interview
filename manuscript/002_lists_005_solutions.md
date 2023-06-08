{full: true, community: true}
## Solutions

### Reverse k-groups in a list

There isn’t anything algorithmically complex in this problem. However, there are many opportunities to trip up and make a mistake.

We talked about reversing a singly-linked list in the simple operations section. This operation will be our base for reversing the group of k elements.

{caption: "Reversing a group of k elements."}
```cpp
List::Node* reverse(List::Node *head, int64_t k) {
    List::Node *result = nullptr;
    List::Node *iter = head;

    for (int64_t i = 0; i < k; ++i)
        iter = std::exchange(iter->next, std::exchange(result, iter));
        /* we want to put iter to the front of the list
            but we want the original value of result to be the new iter->next
            but we want the original value of iter->next to be the new iter
        */

    return result;
}
```

The complexity lies in applying this operation multiple times in sequence. For that, we need to keep track of terminating nodes:

- the head of the already processed part; this will be our final result
- the tail of the already processed part; this is where we will attach each reversed section as we iterate
- the head of the unprocessed part; both for iteration and to link the tail of the processed part

{caption: "Solution"}
```cpp
void reverse_groups(List &list, int64_t k) {
    List::Node *unprocessed_head = list.head;
    List::Node *processed_tail = nullptr;
    List::Node *result = nullptr;
    List::Node *iter = list.head;

    while (iter != nullptr) {
        // advance by k elements
        int cnt = 0;
        iter = unprocessed_head;
        while (cnt < k && iter != nullptr) {
            iter = iter->next;
            ++cnt;
        }

        // if we have a full set of k elements
        if (cnt == k) {
            List::Node *processed_head = reverse(unprocessed_head, k);
            // initialize the result if this is the first group
            if (result == nullptr)
                result = processed_head;
            // if this isn't the first group, link the existing tail
            if (processed_tail != nullptr)
                processed_tail->next = processed_head;

            // what was head is now the tail of the reversed section
            processed_tail = unprocessed_head;
            // and iter is the new head
            unprocessed_head = iter;
        }
    }
    if (processed_tail != nullptr)
        processed_tail->next = unprocessed_head;
    list.head = result == nullptr ? unprocessed_head : result;
}
```

We access each element at most twice. Once when advancing by k elements and the second when we are reversing a group of k elements. This means that our time complexity is `$O(n)$`, and since we only store the terminal nodes, our space complexity is `$O(1)$`.

<!-- https://compiler-explorer.com/z/crzYq3jjn -->

### Merge a list of sorted lists

We already discussed merging two lists in the simple operations section. However, we need to be careful here. If we would merge-in each list in a loop, we would end up with `$O(k*n)$` time complexity.

The desired `$O(n\log k)$` time complexity should point you towards some form of a sorted structure (`std::priority_queue`, heap algorithms, `std::set`). However, we have another option. If we merge lists in pairs, we also end up with `$O(\log k)$` iterations, leading to a total `$O(n\log k)$` time complexity.

In either case, we must be careful not to introduce accidental copies.

{caption: "Solution using an always-sorted structure."}
```cpp
std::forward_list<int64_t> merge(std::forward_list<std::forward_list<int64_t>> in) {
    using fwlst = std::forward_list<int64_t>;

    // Custom comparator, compare based on the first element
    auto cmp = [](const fwlst& l, const fwlst& r) {
        return l.front() < r.front();
    };
    // View of non-empty lists, if we filter here, 
    // we don't have to check later.
    auto non_empty = in |
      std::views::filter([](auto& l) { return !l.empty(); }) |
      std::views::common;
    // Consume the input using std::move_iterator, 
    // avoids making copies of the lists.
    std::multiset<fwlst,decltype(cmp)> q(
        std::move_iterator(non_empty.begin()), 
        std::move_iterator(non_empty.end()), 
        cmp);

    fwlst result;
    fwlst::iterator tail = result.before_begin();
    while (!q.empty()) {
        // Extract the node that holds the element, 
        // without making a copy
        auto top = q.extract(q.begin());

        // Splice the first element of the top list to the result
        result.splice_after(tail,
                            top.value(), top.value().before_begin());
        ++tail;

        if (!top.value().empty())
           q.insert(std::move(top)); // put back
    }
    return result;
}
```

Because we extract each element once and each extract operation involves `$O(\log k)$` insertion operation, we end up with `$O(n\log k)$` time complexity. Our `std::multiset` will use `$O(k)$` memory.

{caption: "Solution using pairwise merging."}
```cpp
std::forward_list<int64_t> merge(std::forward_list<std::forward_list<int64_t>> in) {
    std::forward_list<std::forward_list<int64_t>> merged;
    // While we have more than one list
    while (std::next(in.begin()) != in.end()) {
        auto it = in.begin();
        // Take a pair of lists
        while (it != in.end() && std::next(it) != in.end()) {
            // Merge
            it->merge(*std::next(it));
            merged.emplace_front(std::move(*it));
            std::advance(it, 2);
        }
        // If we have odd number of lists
        if (it != in.end())
            merged.emplace_front(std::move(*it));
        // Switch the lists for the next iteration
        in = std::move(merged); 
    }
    return std::move(in.front());
}
```

We merge `$n$` elements in every iteration, repeating this for `$\log k$` iterations, leading to `$O(n\log k)$` time complexity. The only additional memory we use is to store the partially merged lists; therefore, we end up with `$O(k)$` space complexity.

<!-- https://compiler-explorer.com/z/r7GrhvcMv -->


### Remove the kth element from the end of a list

The trivial approach would be to check whether we are at the kth element from the back and, if not, advance to the next element. However, this approach would have `$O(n*n)$` time complexity.

Consider that once we have checked whether we are at the kth element from the back, we have to calculate the kth element. Moving to the next pair of elements that are k elements apart only requires that we advance both iterators at the same time.

Extending this idea allows us to implement an `$O(n)` solution. We calculate the first pair of elements that k apart, and from there, we advance both iterators in step until we reach the end of the list.

```cpp
void remove_kth_from_end(std::forward_list<int64_t>& head, int64_t k) {
    auto node = head.before_begin();
    auto tail = head.begin();
    // advance the tail by k-1 steps
    for (int64_t i = 1; i < k && tail != head.end(); ++i)
        ++tail;

    // there is no k-the element from the back
    if (tail == head.end()) return;

    // advance both node and tail, until we reach the end with tail
    // this means that node is poiting to the k-th element from the back
    while (std::next(tail) != head.end()) {
        ++node;
        ++tail;
    }

    // remove the node
    head.erase_after(node);
}
```

<!-- https://compiler-explorer.com/z/1TYj6q6x5 -->

### Find a loop in a linked list

