{full: true, community: true}
## BST: Binary Search Tree

Binary trees are a commonly used data structure as they can efficiently encode decisions (at each node, we can progress to the left or right child), leading to log(n) complexity (for a balanced tree).

One specific type of tree you can encounter during interviews is a binary search tree. This tree encodes a simple property. For each node, all nodes in the left subtree are of lower values than the value of this node, and all nodes in the right subtree are of higher values than the value of this node.

A balanced binary search tree can be used as a quick lookup table, as we can lookup any value in log(n) value; however, whether we will arrive at a balanced tree very much depends on the order in which elements are inserted into the tree, as the binary search tree doesn't come with any self-balancing algorithms (for that we would have to go to Red-Black trees, which is outside the scope of this book).

### Constructing a BST

To construct a binary search tree, we follow the lookup logic to find the null node where the added value should be located.

<!-- image of a simple BST -->

<!-- code to construct a BST -->

As mentioned above, the binary search tree doesn't come with any self-balancing algorithms; we can, therefore, end up in pathological situations, notably when constructing a binary search tree from a sorted input.

<!-- image of a pathological situation -->

### Validating a BST

Binary search trees frequently appear during coding interviews as they are still simple, yet they encode an interesting property.

The most straightforward problem (aside from constructing a BST) is validating whether a binary tree is a binary search tree.

<!-- block with path to the problem and solution -->

<!-- solution -->