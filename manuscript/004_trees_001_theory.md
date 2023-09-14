{full: true, community: true}
# Trees

Interview questions that include trees can be tricky, notably in C++. On a fundamental level, linked lists and trees are both directed graphs. Therefore you might expect that solving a problem involving a tree would compare in difficulty to a problem involving a linked list. Unfortunately, trees get no support from the standard library. There is no "tree" data structure or algorithms[^heapalgo].

[^heapalgo]: You could argue that heap algorithms fit into this category.

## Representing trees

Since we cannot rely on the standard library to provide a tree data structure, we must build our own. The design options mirror our approaches when implementing a custom linked list (see. [Custom lists](#custom_lists)).

The most straightforward approach for a binary tree would again be to rely on *std::unique_ptr* and have each node own its children.

{caption: "Flawed approach for implementing a binary tree."}
```cpp
template <typename T>
struct TreeNode {
    T value = T{};
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;
};

auto root = std::make_unique<TreeNode<std::string>>(
    "root node", nullptr, nullptr);
// root->value == "root node"
root->left = std::make_unique<TreeNode<std::string>>(
    "left node", nullptr, nullptr);
// root->left->value == "left node"
root->right = std::make_unique<TreeNode<std::string>>(
    "right node", nullptr, nullptr);
// root->right->value == "right node"
```

<!-- https://compiler-explorer.com/z/KrTej6nxa -->

While this might be tempting, and notably, this approach even makes sense from an ownership perspective, this approach suffers the same recursive destruction problem as the linked list.

When working with well-balanced trees, this problem might not manifest; however, a forward-only linked list is a valid binary tree. Therefore we can still easily trigger the problem.

{caption: "A demonstration of a problem caused by recursive destruction."}
```cpp
template <typename T>
struct TreeNode {
    T value = T{};
    std::unique_ptr<TreeNode<T>> left;
    std::unique_ptr<TreeNode<T>> right;
};

{
auto root = std::make_unique<TreeNode<int>>(0,nullptr);
// Depending on the architecture/compiler, the specific number
// of elements we can handle without crash will differ.
TreeNode<int>* it = root.get();
for (int i = 0; i < 100000; ++i)
    it = (it->left = std::make_unique<TreeNode<int>>(0,nullptr)).get();
} // BOOM
```

<!-- https://compiler-explorer.com/z/MPYhf3T77 -->

{class: information}
B> As a reminder: The recursive nature comes from chaining *std::unique_ptr*. As part of destroying a *std::unique_ptr\<TreeNode\<int\>\>* we need first to destroy the child node, which in turn needs to destroy its child, and so on.
B> Each program has a limited stack space, and a sufficiently deep naive binary tree can easily exhaust this space.

While the above approach isn't quite suitable for production code, it does offer a convenient interface. For example, splicing the tree requires only calling *std::swap* on the source and destination *std::unique_ptr*, which will work even across trees.

To avoid recursive destruction, we can separate the encoding of the structure of the tree from resource ownership.

{caption: "A binary tree with structure and resource ownership separated."}
```cpp
template <typename T>
struct Tree {
    struct Node {
        T value = T{};
        Node* left = nullptr;
        Node* right = nullptr;
    };
    Node* add(auto&& ... args) {
        storage_.push_back(std::make_unique<Node>(
            std::forward<decltype(args)>(args)...));
        return storage_.back().get();
    }
    Node* root;
private:
    std::vector<std::unique_ptr<Node>> storage_;
};

Tree<std::string> tree;
tree.root = tree.add("root node");
// tree.root->value == "root node"
tree.root->left = tree.add("left node");
// tree.root->left->value == "left node"
tree.root->right = tree.add("right node");
// tree.root->right->value == "right node"
```

<!-- https://compiler-explorer.com/z/sWeG659cr -->

This approach does completely remove the recursive destruction; however, we pay for that.
While we can still easily splice within a single tree, splicing between multiple trees becomes cumbersome (because it involves splicing between the resource pools).

In the context of C++, neither of the above solutions is particularly performance-friendly. The biggest problem is that we are allocating each node separately, which means that they can be allocated far apart, in the worst-case situation, each node mapping to a different cache line.

Conceptually, the solution is obvious: flatten the tree. However, as we are talking about performance-sensitive design, the specific details of the approach matter a lot and should consider the data access patterns.

The following is one possible approach for a binary tree.

{caption: "One possible approach for representing a binary tree using flat data structures."}
```cpp
constexpr inline size_t nillnode = 
    std::numeric_limits<size_t>::max();

template <typename T>
struct Tree {
    struct Children {
        size_t left = nillnode;
        size_t right = nillnode;
    };

    std::vector<T> data;
    std::vector<Children> children;

    size_t add(auto&&... args) {
        data.emplace_back(std::forward<decltype(args)>(args)...);
        children.push_back(Children{});
        return data.size()-1;
    }
    size_t add_as_left_child(size_t idx, auto&&... args) {
        size_t cid = add(std::forward<decltype(args)>(args)...);
        children[idx].left = cid;
        return cid;
    }
    size_t add_as_right_child(size_t idx, auto&&... args) {
        size_t cid = add(std::forward<decltype(args)>(args)...);
        children[idx].right = cid;
        return cid;
    }
};

Tree<std::string> tree;
auto root = tree.add("root node");
// tree.data[root] == "root node"
auto left = tree.add_as_left_child(root, "left node");
// tree.data[left] == "left node", tree.children[root].left == left
auto right = tree.add_as_right_child(root, "right node");
// tree.data[right] == "right node", tree.children[root].right == right
```

<!-- https://compiler-explorer.com/z/6PoxzcYz7 -->

As usual, we pay for the added performance by increased complexity. We must refer to nodes through their indices since both iterators and references get invalidated during a *std::vector* reallocation. On top of that, implementing splicing for a flat tree would be non-trivial and not particularly performant as it involves re-indexing.