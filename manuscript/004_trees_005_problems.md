{full: true, community: true}
## Canonical problems

Tree problems can cover quite a range, from simple variants of the basic traversals through various variants of paths in trees to tricky problems that require non-trivial analysis for an efficient solution.

This section covers three medium complexity problems: (de)serializing an n-ary tree, all nodes' k-distance, and the number of reorders of a BST. The section also covers two tricky problems: sum of distances to all nodes and well-behaved paths.

### Serialise and de-serialise n-ary tree

Given an n-ary tree data structure, implement stream extraction and insertion operations that serialise and deserialise the tree. The choice of format is part of the assignment.

{caption: "The tree data structure."}
```cpp
struct Node {
    uint32_t value;
    std::vector<Node*> children;
};

struct Tree {
    Node* root = nullptr;
    // Add node to the tree, when parent == nullptr, the method sets the tree root
    Node* add_node(uint32_t value, Node* parent = nullptr);

    friend std::istream& operator>>(std::istream& s, Tree& tree);
    friend std::ostream& operator<<(std::ostream& s, Tree& tree);
private: 
    std::vector<std::unique_ptr<Node>> storage_;
};
```

{width: "100%"}
![Serialising and deserialising an n-ary tree.](trees/serialize.png)

Each node stores a *uint32_t* value and a vector of weak pointers to children. To add a node to the tree, use the *add_node* method (the method will set the tree root when the parent is *nullptr*).

{class: information}
B> The scaffolding for this problem is located at `trees/nary_tree`. Your goal is to make the following commands pass without any errors: `bazel test //trees/nary_tree/...`, `bazel test --config=addrsan //trees/nary_tree/...`, `bazel test --config=ubsan //trees/nary_tree/...`.

### Find all nodes of distance *k* in a binary tree

Given a binary tree containing unique integer values, return all nodes that are *k* distance from the given node *n*.

{width: "35%"}
![Example tree with highlighted nodes distance two from the node with value *9*.](trees/kdistance.png)

{class: information}
B> The scaffolding for this problem is located at `trees/kdistance`. Your goal is to make the following commands pass without any errors: `bazel test //trees/kdistance/...`, `bazel test --config=addrsan //trees/kdistance/...`, `bazel test --config=ubsan //trees/kdistance/...`.

### Sum of distances to all nodes

Given a tree with n nodes, represented as a graph using a neighbourhood map, calculate the sum of distances to all other nodes for each node.

{width: "50%"}
![Example of a tree with four nodes and the corresponding calculated sums of distances.](trees/sum_of_distances.png)

The node ids are in the range *\[0,n\)*.

{class: information}
B> The scaffolding for this problem is located at `trees/sum_distances`. Your goal is to make the following commands pass without any errors: `bazel test //trees/sum_distances/...`, `bazel test --config=addrsan //trees/sum_distances/...`, `bazel test --config=ubsan //trees/sum_distances/...`.

### Well-behaved paths in a tree

Given a tree, represented using two arrays of length *n*:

- an array of node values, with values represented by positive integers
- an array of edges represented as pairs of indexes

Return the number of well-behaved paths. A well-behaved path begins and ends in a node with the same value, with all intermediate nodes being either lower or equal to the values at the ends of the path.

{width: "30%"}
![Example of a tree with five single-node well-behaved paths and one four-node (dashed line) well-behaved path.](trees/well_behaved.png)

{class: information}
B> The scaffolding for this problem is located at `trees/well_behaved`. Your goal is to make the following commands pass without any errors: `bazel test //trees/well_behaved/...`, `bazel test --config=addrsan //trees/well_behaved/...`, `bazel test --config=ubsan //trees/well_behaved/...`.

### Number of reorders of a serialized BST

Given a permutation of *1..N* as *std::vector<int>*, return the number of other permutations that produce the same BST. The BST is produced by inserting elements in the order of their indexes (i.e. left-to-right).

Because the number of permutations can be high, return the result as modulo *10^9+7*.

{width: "20%"}
![Example of two reorders that lead to the same binary search tree.](trees/bst_reorder.png)

{class: information}
B> The scaffolding for this problem is located at `trees/bst_reorders`. Your goal is to make the following commands pass without any errors: `bazel test //trees/bst_reorders/...`, `bazel test --config=addrsan //trees/bst_reorders/...`, `bazel test --config=ubsan //trees/bst_reorders/...`.