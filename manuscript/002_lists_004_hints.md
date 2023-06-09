{full: true, community: true}
## Hints

### Reverse k-groups in a list

1. You will need a function to reverse a list.
2. When you reverse k elements, what needs to re-connect to what? Drawing a picture may help.
3. When we reverse a group of k elements, what was previously the first element is now the last element of the group.

### Merge a list of sorted lists

1. There are two ways to achieve *log* scaling.
2. If you can decrease the number of unmerged lists by a factor of two in each step, you will get *log* scaling.
3. Picking the lowest element from a sorted range is *log(n)*.

### Remove the kth element from the end of a singly-linked list

1. How can you check that an element is the kth element from the end of the list?
2. How can you re-use the information you calculated to check the next element?

### Find a loop in a linked list

