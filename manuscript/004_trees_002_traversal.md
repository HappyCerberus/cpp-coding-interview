{full: true, community: true}
## Tree traversals

Before you read this section, I encourage you to familiarize yourself with [depth-first](#dfs) and [breadth-first](#bfs) search.
Both searches are suitable for traversing a tree.

However, for binary trees in particular, the property we care about is the specific order in which we visit the nodes of the tree.
We will start with three traversals that are all based on the depth-first search.

### Pre-order traversal

In pre-order traversal, we visit each node before visiting its children.

{caption: "Pre-order traversal on a binary tree."}
```cpp
void pre_order(Node *node, const std::function<void(Node*)>& visitor) {
    if (node == nullptr) return;
    visitor(node);
    pre_order(node->left, visitor);
    pre_order(node->right, visitor);
}
```

![Order of visiting nodes using pre-order traversal in a full binary tree.](trees/pre_order_traversal.png)

A typical use case for pre-order traversal is when we need to serialise or deserialise a tree.
In the following example, we serialise a binary tree as a series of space-delimited integers with missing child nodes represented by zeroes.

{caption: "Serializing a binary tree of integers into a stream of space-delimited integers."}
```cpp
// Serialize using pre-order traversal.
void serialize(Node *node, std::ostream& s) {
    if (node == nullptr) {
        s << 0 << " ";
        return;
    }
    s << node->value << " ";
    serialize(node->left, s);
    serialize(node->right, s);
}
```

We must have already deserialised the parent node before we can insert its children into the tree. This is why pre-order traversal is a natural fit for this use case.

{caption: "Deserializing a binary tree from a stream of space-delimited integers."}
```cpp
// Helper for deserializing a single node.
Tree<int>::Node *deserialize_single(Tree<int>& tree, std::istream& s) {
    int value = 0;
    if (!(s >> value) || value <= 0) return nullptr;
    return tree.add(value);
}

// Deserialize using pre-order traversal.
Tree<int>::Node *deserialize(Tree<int>& tree, std::istream& s) {
    auto node = deserialize_single(tree, s);
    if (node == nullptr) return node;
    node->left = deserialize(tree, s);
    node->right = deserialize(tree, s);
    return node;
}
```

<!-- https://compiler-explorer.com/z/eKrsWPMz7 -->

#### Non-recursive pre-order

With a recursive approach, we can run into the same stack exhaustion problem we faced during tree destruction.
Fortunately, similar to the baseline depth-first-search, we can switch to a non-recursive implementation by relying on a *std::stack* or *std::vector* to store the traversal state.

{caption: "Non-recursive implementation of pre-order traversal."}
```cpp
void pre_order_stack(Node* root, const std::function<void(Node*)>& visitor) {
    std::stack<Node*> stack;
    stack.push(root);
    while (!stack.empty()) {
        Node *curr = stack.top();
        stack.pop();
        visitor(curr);
        // We visit "null" nodes with this approach, which might be helpful.
        if (curr == nullptr) continue;
        // Alternatively, we could move this condition to the push:
        // if (curr->right != nullptr) stack.push(curr->right);

        // We must insert in reverse to maintain the same 
        // ordering as recursive pre-order.
        stack.push(curr->right);
        stack.push(curr->left);
    }
}
```

### Post-order traversal

In post-order traversal, we visit each node after its children.

{caption: "Recursive post-order traversal of a binary tree."}
```cpp
void post_order(Node *node, const std::function<void(Node*)>& visitor) {
    if (node == nullptr) return;
    post_order(node->left, visitor);
    post_order(node->right, visitor);
    visitor(node);
}
```

![Order of visiting nodes using post-order traversal in a full binary tree.](trees/pre_order_traversal.png)

Because of this ordering, one use case for post-order is in expression trees, where we can only evaluate the parent expression if both its children were already evaluated.

{caption: "Example of a simple expression tree implementation where each node contains a value or a simple operation."}
```cpp
struct Eventual {
    std::variant<int, // value
        // or operation
        std::function<int(const Eventual& l, const Eventual& r)>> content;
};

Tree<Eventual> tree;
auto plus = [](const Eventual& l, const Eventual& r) {
    return get<0>(l.content) + get<0>(r.content);
};
auto minus = [](const Eventual& l, const Eventual& r) {
    return get<0>(l.content) - get<0>(r.content);
};
auto times = [](const Eventual& l, const Eventual& r) {
    return get<0>(l.content) * get<0>(r.content);
};
// encode (4-2)*(2+1)
auto root = tree.root = tree.add(Eventual{times});
auto left = root->left = tree.add(Eventual{minus});
auto right = root->right = tree.add(Eventual{plus});
left->left = tree.add(Eventual{4});
left->right = tree.add(Eventual{2});
right->left = tree.add(Eventual{2});
right->right = tree.add(Eventual{1});

post_order(tree.root, [](Node* node) {
    // If this node already has a result value, we don't have to do anything.
    if (std::holds_alternative<int>(node->value.content)) return;
    // If it is an operation, then evaluate.
    // Post-order guarantees that node->left->value 
    // and node->right->value are both values.
    node->value.content = std::get<1>(node->value.content)(
        node->left->value, node->right->value);
});
// get<0>(root->value.content) == 6
```

<!-- https://compiler-explorer.com/z/hjr7rj7EK -->

#### Non-recursive post-order

For a non-recursive approach, we could visit all nodes in pre-order, remembering each, and then iterate over the nodes in reverse order.
However, we can do better.

The main problem we must solve is remembering enough information to correctly decide whether it is time to visit the parent node. The following approach eagerly explores the left sub-tree, remembering both the right sibling and the parent node. When we revisit the parent node, we can decide whether it is time to visit it based on the presence of the right sibling.

{caption: "Non-recursive post-order traversal with only partial memoization."}
```cpp
void post_order_nonrecursive(Node *root, const std::function<void(Node*)>& visitor) {
    std::stack<Node*> s;
    Node *current = root;
    while (true) {
        // Explore left, but remember node & right child.
        if (current != nullptr) {
            if (current->right != nullptr)
                s.push(current->right);
            s.push(current);
            current = current->left;
            continue;
        }
        // current == nullptr
        if (s.empty()) return;
        current = s.top();
        s.pop();
        // If we have the right child remembered, 
        // it would be on the top of the stack.
        if (current->right && !s.empty() && current->right == s.top()) {
            // If it is, we must visit it (and it's children) first.
            s.pop();
            s.push(current);
            current = current->right;
        } else {
            // If the top of the stack is not the right child, 
            // we have already visited it.
            visitor(current);
            current = nullptr;
        }
    }
}
```

### In-order traversal

In in-order traversal, we visit each node in between visiting its left and right children.

Unlike pre- and post-order traversals that are relatively general, and we can easily apply them to n-ary trees, in-order traversal only makes sense in the context of binary trees.

{caption: "Recursive in-order traversal of a binary tree."}
```cpp
// in-order traversal
void in_order(Node* node, const std::function<void(Node*)>& visitor) {
    if (node == nullptr) return;
    in_order(node->left, visitor);
    visitor(node);
    in_order(node->right, visitor);
}
```

![Order of visiting nodes using in-order traversal in a full binary tree.](trees/pre_order_traversal.png)

The typical use case for in-order traversal is for traversing binary trees that encode an ordering of elements. The in-order traversal naturally maintains this order during the traversal.

{caption: "Traversing a BST to produce a sorted output."}
```cpp
// Insert an element into the tree in sorted order
void add_sorted(Tree<int64_t>& tree, Node* node, int64_t value) {
    if (value <= node->value) {
        if (node->left == nullptr)
            node->left = tree.add(value);
        else
            add_sorted(tree, node->left, value);
    } else {
        if (node->right == nullptr)
            node->right = tree.add(value);
        else
            add_sorted(tree, node->right, value);
    }
}

Tree<int64_t> tree;
// Generate a sorted binary tree with 10 nodes
std::mt19937 gen(0); // change the seed for a different output
std::uniform_int_distribution<> dist(0,1000);
tree.root = tree.add(dist(gen));
for (int i = 0; i < 9; i++) {
    add_sorted(tree, tree.root, dist(gen));
}

// in-order traversal will print the values in sorted order
in_order(tree.root, [](Node* node) {
    std::cout << node->value << " ";
});
std::cout << "\n";
// stdlibc++: 424 545 549 593 603 624 715 845 848 858 
// libc++: 9 192 359 559 629 684 707 723 763 835
```

#### Non-recursive in-order

The non-recursive approach is similar to post-order, but we avoid the complexity of remembering the right child.

{caption: "Non-recursive in-order traversal implementation."}
```cpp
void in_order_nonrecursive(Node *root, const std::function<void(Node*)>& visitor) {
    std::stack<Node*> s;
    Node *current = root;
    while (current != nullptr || !s.empty()) {
        // Explore left
        while (current != nullptr) {
            s.push(current);
            current = current->left;
        }
        // Now going back up the left path visit each node, 
        // then explore the right child. 
        // This works, because the left child was already
        // visited as we go up the path.
        current = s.top();
        s.pop();
        visitor(current);
        current = current->right;
    }
}
```

<!-- https://compiler-explorer.com/z/6f9xnTe3z -->

### Rank-order traversal

The rank-order or level-order traversal traverses nodes in the order of their distance from the root node.

All the previous traversals: pre-order, post-order and in-order are based on depth-first search, rank-order traversal is based on breadth-first search, naturally avoiding the recursion problem.

{caption: "Rank-order traversal implementation."}
```cpp
void rank_order(Node* root, const std::function<void(Node*)>& visitor) {
    std::queue<Node*> q;
    if (root != nullptr)
        q.push(root);
    while (!q.empty()) {
        Node* current = q.front();
        q.pop();
        if (current == nullptr) continue;
        visitor(current);
        q.push(current->left);
        q.push(current->right);
    }
}
```

![Order of visiting nodes using rank-order traversal in a full binary tree.](trees/pre_order_traversal.png)

Rank-order traversal typically comes up as part of more complex problems.
By default, it can be used to find the closest node to the root that satisfies particular criteria or calculate the nodes' distance from the root.

{caption: "Calculating the maximum node value at each level of the tree."}
```cpp
std::vector<int> max_at_level(Node* root) {
    std::vector<int> result;
    std::queue<std::pair<Node*,size_t>> q;
    if (root != nullptr)
        q.push({root,0});
    while (!q.empty()) {
        auto [node,rank] = q.front();
        q.pop();
        if (result.size() <= rank)
            result.push_back(node->value);
        else
            result[rank] = std::max(result[rank], node->value);
        if (node->left != nullptr)
            q.push({node->left,rank+1});
        if (node->right != nullptr)
            q.push({node->right, rank+1});
    }
    return result;
}
```

<!-- https://compiler-explorer.com/z/WP4Tsdeeh -->