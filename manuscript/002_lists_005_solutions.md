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
- the head of the unprocessed part; for iteration and we also need it to relink the reversed group of k elements

![Keeping track of nodes](linked_list/list_reverse_kgroup_solution_01.png)

The algorithm repeats these steps:

![Calculate the kth element by advancing k steps from the unprocessed head.](linked_list/list_reverse_kgroup_solution_02.png)
![Reverse k elements starting from the unprocessed head.](linked_list/list_reverse_kgroup_solution_03.png)
![Connect the reversed group to the tail of the processed part. Connect the unprocessed head to the kth element.](linked_list/list_reverse_kgroup_solution_04.png)
![The new tail is the unprocessed head. The new unprocessed head is the kth element.](linked_list/list_reverse_kgroup_solution_05.png)

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
<!-- https://compiler-explorer.com/z/crzYq3jjn -->

We access each element at most twice. Once when advancing by k elements and the second when we are reversing a group of k elements. This means that our time complexity is *$O(n)*, and since we only store the terminal nodes, our space complexity is *O(1)*.

### Merge a list of sorted lists

We already discussed merging two lists in the simple operations section. However, we need to be careful here. If we would merge-in each list in a loop, we would end up with *$O(k\*n)* time complexity, where *k* is the number of lists and *n* is the number of nodes.

The desired *O(n\*log(k))* time complexity should point you towards some form of a sorted structure(*std::priority_queue*, heap algorithms, *std::set*). A sorted structure would give us *log(k)* lookup, which we can then repeat *n* times to traverse all the elements in order.

The second way to reach *log* scaling is with an exponential factor. If we merge lists in pairs, we will half the number of lists in each step, with total *log(k)* steps, leaving us again with *O(n\*log(k))* complexity.

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

Because we extract each element once and each extract operation involves *O(log(k))* insertion operation, we end up with *O(n\*log(k))* time complexity. Our *std::multiset* will use *O(k)* memory.

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

We merge *n* elements in every iteration, repeating this for *log(k)* iterations, leading to *O(n\*log(k))* time complexity. The only additional memory we use is to store the partially merged lists; therefore, we end up with *O(k)* space complexity.

<!-- https://compiler-explorer.com/z/r7GrhvcMv -->

### Remove the kth element from the end of a list

The trivial approach would be to check whether we are at the kth element from the back and, if not, advance to the next element. However, this approach would have *O(n\*k)* time complexity.

Once we check whether an element is the kth element from the back, we have two iterators that are *k-1* elements apart.
To check the next element, we do not have to repeat the entire process; instead, we can advance the iterator pointing to the previous *k-1* apart element, again ending with a pair of elements that are *k-1* apart.

Extending this idea allows us to implement an *O(n)* solution. We calculate the first pair of elements that k apart, and from there, we advance both iterators in step until we reach the end of the list.

{caption: "Solution"}
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

This problem is the prototypical problem for the fast and slow pointer technique (a.k.a. Floyd’s tortoise and hare).

We will eventually reach the end if we iterate over the list without a loop. However, if there is a loop, we will end up stuck in the loop.

The tricky part is detecting that we are stuck in the loop. If we use two pointers to iterate, one slow one, iterating normally and one fast one, iterating over two nodes in each step, we have a guarantee that if they get stuck in a loop, they will eventually meet.

![Initial configuration: slow and fast pointers are pointing to the head of the list.](linked_list/list_loop_detect_01.png)

![Configuration after two steps.](linked_list/list_loop_detect_02.png)

![Configuration after four steps.](linked_list/list_loop_detect_03.png)

![Configuration after six steps. Loop detected.](linked_list/list_loop_detect_04.png)

{captions: "Detecting a loop"}
```cpp
bool loop_detection(const List &list) {
    List::Node *slow = list.head;
    List::Node *fast = list.head;
    do {
        // nullptr == no loop
        if (slow == nullptr)
            return false;
        if (fast == nullptr || fast->next == nullptr)
            return false;
        slow = slow->next;
        fast = fast->next->next;
    } while (slow != fast);

    return true;
}
```

#### Identifying the start of the loop

To detect the start of the loop, we must look at how many steps both pointers made before they met up.

Consider that the slow pointer moved *x* steps before entering the loop and then *y* steps after entering the loop for a *slow = x + y* total.

The fast pointer moved similarly. It also moved *x* steps before entering the loop and then *y* steps after entering the loop when it met up with the slow pointer; however, before that, it did an unknown number of loops: *fast = x + n\*loop + y*. Importantly, we also know that the fast pointer also did *2\*slow* steps.

If we put this together, we end up with the following:

- *2\*(x + y) = x + n\*loop + y*
- *x = n\*loop - y*

This means that the number of steps to reach the loop is the same as the number of steps remaining from where the pointers met up to the start of the loop.

So to find the start of the loop, we can iterate from the start and the meeting point. Once these two new pointers meet, we have our loop start.

![One pointer at the meeting point, one at the list head.](linked_list/list_loop_start_01.png)

![The loop start is identified after two steps.](linked_list/list_loop_start_02.png)

{caption: "Solution with start detection."}
```cpp
List::Node *loop_start(const List &list) {
    // Phase 1, detect the loop.
    List::Node *slow = list.head;
    List::Node *fast = list.head;
    do {
        // nullptr == no loop
        if (slow == nullptr)
            return nullptr;
        if (fast == nullptr || fast->next == nullptr)
            return nullptr;
        slow = slow->next;
        fast = fast->next->next;
    } while (slow != fast);

    // Phase 2, iterate from head and from meeting point.
    List::Node *onloop = slow;
    List::Node *offloop = list.head;
    while (onloop != offloop) {
        onloop = onloop->next;
        offloop = offloop->next;
    }
    return onloop;
}
```

#### Fixing the list

The main difficulty in fixing the list is that we are working with a singly-linked list. Fixing the list means that we must unlink node one before the start of the loop.

{caption: "Solution for fixing the list."}
```cpp
void loop_fix(List &list) {
    // Phase 1, detect the loop.
    List::Node *slow = list.head;
    List::Node *fast = list.head;
    List::Node *before = nullptr;
    do {
        // nullptr == no loop
        if (slow == nullptr)
            return;
        if (fast == nullptr || fast->next == nullptr)
            return;
        slow = slow->next;
        // Keep track of the node one before the fast pointer
        before = fast->next;
        fast = fast->next->next;
    } while (slow != fast);

    // Phase 2, iterate from head and from meeting point.
    List::Node *onloop = slow;
    List::Node *offloop = list.head;
    while (onloop != offloop) {
        // Keep track of the node one before the onloop pointer
        before = onloop;
        onloop = onloop->next;
        offloop = offloop->next;
    }

    // Phase 3, fix the list, before != nullptr
    before->next = nullptr;
}
```