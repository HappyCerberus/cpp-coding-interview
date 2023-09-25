{full: true, community: true}
## Paths in trees

Another ubiquitous category of tree-oriented problems is dealing with paths in trees. Notably, paths in trees can be non-trivial to reason about, but at the same time, the tree structure still offers the possibility for very efficient solutions.

A path is a sequence of nodes where every two consecutive nodes have a parent/child relationship, and each node is visited at most once.

![Example of a tree with two paths.](trees/paths_in_trees.png)

Let's demonstrate this on a concrete interview problem.

### Maximum path in a tree

Given a binary tree, where each node has an integer value, determine the maximum path in this tree. The value of a path is the total of all the node values visited.

{class: information}
B> Before you continue reading, I encourage you to try to solve it yourself.
B> The scaffolding for this problem is located at `trees/maximum_path`. Your goal is to make the following commands pass without any errors: `bazel test //trees/maximum_path/...`, `bazel test --config=addrsan //trees/maximum_path/...`, `bazel test --config=ubsan //trees/maximum_path/...`.

Let's consider a single node in the tree. Only four possible paths can be the maximum path that crosses this node:

- a single-node path that contains this node only
- a path coming from the left child, terminating in this node
- a path coming from the right child, terminating in this node
- a path crossing this node, i.e. going from one child, crossing this node, continuing to the other child

![The four possible paths crossing a node.](trees/paths_crossing_a_node.png)

Considering the above list, we can limit the information we need to calculate the maximum path in a sub-tree whose root is the above node.

If the maximum path doesn't cross this node, then the maximum path in this tree exists inside one of the child subtrees.

If the path crosses this node, we can calculate the maximum path by using the information about maximum paths that terminate in the left and right child.

- a single-node path is simply the value of the node
- a path coming from the left child terminating in this node is the value of the maximum path terminating in the left child, plus the value of this node
- a path coming from the right child terminating in this node is the value of the maximum path terminating in the right child, plus the value of this node
- a path crossing this node is the value of the maximum path terminating on the right child, plus the value of the maximum path terminating in the left child, plus the value of this node

The maximum path crossing this node is the maximum of the above paths.

Now that we know what to calculate, we can traverse the tree in post-order (visiting the children before the parent node) while keeping track of the aforementioned values.

{caption: "Solution using post-order traversal."}
```cpp
// We return two values:
//  - the maximum path that terminates in this node
//  - the maximum path in this sub-tree
std::pair<int,int> maxPath(Tree::Node* node) {
    // initialize with single-node paths
    int max_path = node->value;
    int max_subtree = node->value;
    int full_path = node->value;

    if (node->left != nullptr) {
        // Calculate recursive values for the left path
        auto [path,tree] = maxPath(node->left);
        // Path terminating in this node: max of case 1 and 2
        max_path = std::max(max_path, path + node->value);
        // maximum path might not be crossing this node,
        // contained in the left subtree
        max_subtree = std::max(max_subtree, tree);
        // value of the crossing path (case 4)
        full_path += path;
    }
    if (node->right != nullptr) {
        // Calculate recursive values for the right path
        auto [path,tree] = maxPath(node->right);
        // Path terminating in this node: max of case 1 and 3
        // note, we already included the case 2 in the left-node if
        max_path = std::max(max_path, path + node->value);
        // maximum path might not be crossing this node,
        // contained in the right subtree
        max_subtree = std::max(max_subtree, tree);
        // value of the crossing path (case 4)
        full_path += path;
    }
    // the full path is the path starting in the left subtree, 
    // crossing this node, continuing into the right subtree
    // the maximum path in this subtree is any of the paths
    max_subtree = std::max(max_subtree, std::max(full_path, max_path));
    // max_path is the longest path terminating in this node
    return {max_path, max_subtree};
}

// Final computation, simply return the maximum
int maxPath(const Tree& t) {
    auto [path,tree] = maxPath(t.root);
    return tree;
}
```

<!-- https://compiler-explorer.com/z/bfddTMs3W -->