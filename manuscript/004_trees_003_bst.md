{full: true, community: true}
## BST: Binary Search Tree

Binary trees are a commonly used data structure as they can efficiently encode decisions (at each node, we can progress to the left or right child), leading to log(n) complexity (for a balanced tree).

One specific type of tree you can encounter during interviews is a binary search tree. This tree encodes a simple property. For each node, all nodes in the left subtree are of lower values than the value of this node, and all nodes in the right subtree are of higher values than the value of this node.

![Example of a balanced binary search tree.](trees/bst_example.png)

A balanced binary search tree can be used as a quick lookup table, as we can lookup any value using log(n) operations; however, whether we will arrive at a balanced tree very much depends on the order in which elements are inserted into the tree, as the binary search tree doesn't come with any self-balancing algorithms (for that we would have to go to Red-Black trees, which is outside the scope of this book).

### Constructing a BST

To construct a binary search tree, we follow the lookup logic to find a null node where the added value should be located.

{caption: "Constructing a BST from a range."}
```cpp
Node*& find_place_for(Node*& root, int value) {
    // The first empty (null) node we encounter
    // is the place where we want to insert.
    if (root == nullptr)
        return root;
    // Higher values go to the right
    if (root->value > value)
        return find_place_for(root->left, value);
    // Lower values go to the left
    if (root->value <= value) // == is for equivalent values
        return find_place_for(root->right, value);
    return root;
}

Tree construct_bst(const std::vector<int>& rng) {
    Tree t;
    for (int v : rng)
        find_place_for(t.root, v) = t.add(v);
    return t;
}
```

<!-- https://compiler-explorer.com/z/3nTYdEob5 -->

As mentioned above, the binary search tree doesn't come with any self-balancing algorithms; we can, therefore, end up in pathological situations, notably when constructing a binary search tree from a sorted input.

![Example of an unballaned tree fromed by inserting elements {1,2,3,4}.](trees/bst_unballanced.png)

### Validating a BST

Binary search trees frequently appear during coding interviews as they are still simple, yet they encode an interesting property.

The most straightforward problem (aside from constructing a BST) is validating whether a binary tree is a binary search tree.

<!-- block with path to the problem and solution -->

<!-- solution -->