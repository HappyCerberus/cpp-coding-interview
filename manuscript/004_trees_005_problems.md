{full: true, community: true}
## Canonical problems

### Serialize and de-serialize n-ary tree

Given an n-ary tree data structure, we must implement stream extraction and insertion operations to serialize and deserialize the tree.

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

The choice of format is part of the problem. Each node stores a *uint32_t* value and a vector of weak pointers to children. To add a node to the tree, use the *add_node* method (the method will set the tree root when the parent is *nullptr*).

### Find all nodes of distance *k* in a binary tree

Given a binary tree containing unique integer values, return all nodes that are *k* distance from the given node *n*.

### Sum of distances to all nodes

Given a tree with n nodes, represented as a graph using a neighbourhood map, calculate the sum of distances to all other nodes for each node.

The node ids are in the range *\[0,n\)*.

### Well-behaved paths in a tree

Given a tree, represented using two arrays of length *n*:

- an array of node values, with values represented by positive integers
- an array of edges represented as pairs of indexes

Return the number of well-behaved paths. A well-behaved path begins and ends in a node with the same value, with all intermediate nodes being either lower or equal to the values at the ends of the path.

### Number of reorders of a serialized BST

Given a permutation of *1..N* as *std::vector<int>*, return the number of other permutations that produce the same BST. The BST is produced by inserting elements in the order of their indexes (i.e. left-to-right).

Because the number of permutations can be high, return the result as modulo *10^9+7*.


