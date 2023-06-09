{full: true, community: true}
## Canonical problems

Now that we've covered the basics, let's move on to real-world problems often seen in technical interviews. This next section will cover four linked list challenges: reversing k-groups in a list, merging a list of sorted lists, removing the kth element from the end, and finding a loop in a corrupted list.

It's a step up from what we've done so far, but with the foundation you've built, you should be well-prepared to handle these tasks. Let's get started.

### Reverse k-groups in a list

Our first challenge is all about diligence. Given a singly-linked list and a positive integer k, transpose the list so that each group of k elements is in reversed internal order. If k doesn't divide the number of elements without a remainder, the remainder of nodes should be left in their original order{i: "lists!reverse"}.

![Example of reversing groups of 3 elements.](linked_list/list_reverse_k_groups_01.png)

You should be able to implement a version that operates in *O(n)* time and *O(1)* additional space, where n is the number of elements in the list.

{class: information}
B> The scaffolding for this problem is located at `lists/k_groups`. Your goal is to make the following commands pass without any errors: `bazel test //lists/k_groups/...`, `bazel test --config=addrsan //lists/k_groups/...`, `bazel test --config=ubsan //lists/k_groups/...`.

### Merge a list of sorted lists

Given a list of sorted lists, return a merged sorted list{i: "divide and conquer"}{i: "always-sorted data structure"}.

![Example of merging three sorted lists.](linked_list/list_merge_k_lists_01.png)

You should be able to implement a version that operates in *O(n\*log(k))* time and uses *O(k)* additional memory, where n is the total number of elements and k is the number of lists we are merging.

{class: information}
B> The scaffolding for this problem is located at `lists/merge`. Your goal is to make the following commands pass without any errors: `bazel test //lists/merge/...`, `bazel test --config=addrsan //lists/merge/...`, `bazel test --config=ubsan //lists/merge/...`.

### Remove the kth element from the end of a singly-linked list

Given a singly-linked list and a positive integer k, remove the kth element (counted) from the end of the list{i: "two pointers!sliding window"}.

![Example of removing the 3rd element from the end of the list.](linked_list/list_remove_kth_element_01.png)

You should be able to implement a version that operates in *O(n)* time and uses *O(1)* additional memory, where *n* is the number of elements in the list.

{class: information}
B> The scaffolding for this problem is located at `lists/end_of_list`. Your goal is to make the following commands pass without any errors: `bazel test //lists/end_of_list/...`, `bazel test --config=addrsan //lists/end_of_list/...`, `bazel test --config=ubsan //lists/end_of_list/...`.

### Find a loop in a linked list

Given a potentially corrupted singly-linked list, determine whether it is corrupted, i.e., it forms a loop{i: "two pointers!slow and fast"}.

![Example of a corrupted list.](linked_list/list_loop_01.png)

- Progression A: return the node that is the first node on the loop.
- Progression B: fix the list.

You should be able to implement a version that operates in *O(n)* and uses *O(1)* additional memory, where n is the number of elements in the list.

{class: information}
B> The scaffolding for this problem is located at `lists/loop` for the basic version and `lists/loop_node`, `lists/loop_fix` for the two progressions. Your goal is to make the following commands pass without any errors: `bazel test //lists/loop/...`, `bazel test --config=addrsan //lists/loop/...`, `bazel test --config=ubsan //lists/loop/...`. Adjust the directory to `loop_node` and `loop_fix` for the relevant progression.