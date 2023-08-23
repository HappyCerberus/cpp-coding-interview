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

auto root = std::make_unique<TreeNode<std::string>>("root node", nullptr, nullptr);
// root->value == "root node"
root->left = std::make_unique<TreeNode<std::string>>("left node", nullptr, nullptr);
// root->left->value == "left node"
root->right = std::make_unique<TreeNode<std::string>>("right node", nullptr, nullptr);
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

