{full: true, community: true}
## BST: Binary Search Tree

Binary trees are a commonly used data structure as they can efficiently encode decisions (at each node, we can progress to the left or right child), leading to *log(n)* complexity (for a balanced tree).

One specific type of tree you can encounter during interviews is a binary search tree. This tree encodes a simple property. For each node, all children in the left subtree are of lower values than the value of this node, and all children in the right subtree are of higher values than the value of this node.

{width: "50%"}
![Example of a balanced binary search tree.](trees/bst_example.png)

A balanced binary search tree can be used as a quick lookup table, as we can lookup any value using *log(n)* operations; however, whether we will arrive at a balanced tree very much depends on the order in which elements are inserted into the tree, as the binary search tree doesn't come with any self-balancing algorithms (for that we would have to go to Red-Black trees, which is outside the scope of this book).

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

{width: "35%"}
![Example of an unbalanced tree formed by inserting elements {1,2,3,4}.](trees/bst_unballanced.png)

### Validating a BST

Binary search trees frequently appear during coding interviews as they are relatively simple, yet they encode an interesting property.

The most straightforward problem (aside from constructing a BST) is validating whether a binary tree is a binary search tree.

{class: information}
B> Before you continue reading, I encourage you to try to solve it yourself.
B> The scaffolding for this problem is located at `trees/validate_bst`. Your goal is to make the following commands pass without any errors: `bazel test //trees/validate_bst/...`, `bazel test --config=addrsan //trees/validate_bst/...`, `bazel test --config=ubsan //trees/validate_bst/...`.

If we are checking a particular node in a binary search tree, going to the left subtree sets an upper bound on all the values in the left subtree and going to the right subtree sets a lower bound on all the values in the right subtree.

{width: "50%"}
![Example of partitioning of values imposed by nodes in a binary search tree.](trees/bst_partitions.png)

If we traverse the tree, keeping track and verifying these bounds, we will validate the BST. If we do not discover any violations, the tree is a BST; if we do, it isn't.

{caption: "Validating a binary search tree."}
```cpp
bool is_valid_bst(Node* root, int min, int max) {
    // Is this node within the bounds?
    if (root->value > max || root->value < min)
        return false;
    // Explore left subtree with the updated bounds
    if (root->left != nullptr) {
        if (root->value == INT_MIN) // avoid underflow
            return false;
        if (!is_valid_bst(root->left, min, root->value - 1))
            return false;
    }
    // Explore right subtree with the updated bounds
    if (root->right != nullptr) {
        if (root->value == INT_MAX) // avoid overflow
            return false;
        if (!is_valid_bst(root->right, root->value + 1, max))
            return false;
    }
    return true;
}

bool is_valid_bst(const Tree& tree) {
    // Root can be any value
    return is_valid_bst(tree.root, INT_MIN, INT_MAX);
}
```

Note that this solution assumes no repeated values. To support duplicate values we would have adjust the check in the left branch, recursing with the same limit value, instead of *-1*.