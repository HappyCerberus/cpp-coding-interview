{full: true, community: true}
## Solutions

### Serialize and de-serialize n-ary tree

There are many possible approaches, as we can choose any format for serialization. However, all approaches should use a pre-order traversal since we need the parent node when we process its children.

The following approach uses a recursive pre-order traversal and a "terminal value" format. The format terminates the list of children with the value -1, which is outside of the domain of uint32_t. For example, a single node tree with the root value of 0 will serialize into "0 -1".

We have to choose how to handle an empty tree. We can either serialize an empty tree as "-1" or leave the output unchanged, representing an empty tree as an empty string. The former has the benefit of explicitly denoting an empty tree, allowing us to store a specific number of empty trees in serialized form (where an empty output is simply empty).

To deserialize a node, we deserialize children until we read a negative value, processing the entire input using pre-order traversal.

{caption: "Solution for serializing and de-serializing an n-ary tree."}
```cpp
void serialize(Node* root, std::ostream& s) {
    // each node is serialized into "value {children} -1"
    s << root->value << " ";
    for (auto c : root->children)
        serialize(c, s);
    s << "-1 ";
}

std::ostream& operator<<(std::ostream& s, Tree& tree) {
    if (tree.root != nullptr)
        serialize(tree.root, s);
    else
        // serialize empty tree as "-1"
        s << "-1 "; 
    return s;
}

Node* deserialize(Tree& tree, Node* parent, uint32_t parent_value, std::istream& s) {
    // pre-reading the value allows for cleaner code
    Node* result = tree.add_node(parent_value, parent);
    int64_t value; // we have to use int64_t to represent all values for uint32_t and -1
    while (s >> value) {
        if (value < 0) // if we read -1, we are done reading the children of this node
            return result;
        // otherwise recursively de-serialize this child
        deserialize(tree, result, value, s);
    }
    return result;
}

std::istream& operator>>(std::istream& s, Tree& tree) {
    int64_t value;
    if (s >> value && value >= 0)
        deserialize(tree, nullptr, value, s);
    return s;
}
```

<!-- https://compiler-explorer.com/z/h5Kbe4avY -->

### Find all nodes of distance *k* in a binary tree

One option is to translate the binary tree, essentially a directed graph, into an undirected graph, in which we can easily find k-distance nodes by applying a breadth-first search.

However, we have a simpler option based on the following observation. Consider a node with one of its children.

If the child doesn’t lie on the path between the node and our target, its distance to the target is simply one more than the distance of the parent node. If it does lie on the path between the node and the target, its distance is one less.

If we explore the graph using pre-order traversal, we will also have a second guarantee that a node is only on the path if it is also on the path between the root and the target node.

Using these observations, we can construct a two-pass solution.

First, we find our target and initialize the distances for all nodes on the path between the target and the tree's root.

In the second pass, if we have a precomputed value for a node, we know that it is on the path, which allows us to distinguish between the two situations. Also, when we encounter a node with the appropriate distance, we remember it.

{caption: "Solution"}
```cpp
// Search for target and build distances to root
int distance_search(Node* root, Node* target, 
                    std::unordered_map<int,int>& distances) {
    if (root == nullptr)
        return -1;
    if (root == target) {
        distances[root->value] = 0;
        return 0;
    }
    // Target in the left sub-tree
    if (int left = distance_search(root->left, target, distances);
        left >= 0) {
        distances[root->value] = left + 1;
        return left + 1;
    }
    // Target in the right sub-tree
    if (int right = distance_search(root->right, target, distances);
        right >= 0) {
        distances[root->value] = right + 1;
        return right + 1;
    }
    // Target not in this sub-tree
    return -1;
}

// Second pass traversal. 
void dfs(Node* root, Node* target, int k, int dist,
         std::unordered_map<int,int>& distances,
         std::vector<int>& result) {
    if (root == nullptr) return;
    // Check if this node is on the path to target.
    auto it = distances.find(root->value);
    // Node is on the path to target, update distance.
    if (it != distances.end())
        dist = it->second;
    // This node is k distance from the target.
    if (dist == k)
        result.push_back(root->value);

    // Distances to children are one more, unless they are on the path
    // which is handled above.
    dfs(root->left, target, k, dist + 1, distances, result);
    dfs(root->right, target, k, dist + 1, distances, result);
}

std::vector<int> find_distance_k_nodes(Node* root, Node* target, int k) {
    // First pass
    std::unordered_map<int,int> distances;
    distance_search(root, target, distances);
    // Second pass
    std::vector<int> result;
    dfs(root, target, k, distances[root->value], distances, result);
    return result;
}
```

<!-- https://compiler-explorer.com/z/36x3Y6sP7 -->

### Sum of distances to all nodes

Let's start with a sub-optimal solution. Consider a subtree for whose root we have already calculated the sum of distances. Moving to the parent node means we are one step further from all the nodes in this subtree.

Therefore if we do a straightforward post-order traversal of the tree, we will be able to calculate the sum of distances for the root node by simply summing up the distances of the children subtrees plus the number of nodes in the subtrees (since being one step further will contribute one per node).

Importantly, this gives us the sum of distances only for the root node, so we must repeat the process for each node (treating each node as the tree's root). Because traversal is an *O(n)* operation, we end up with *O(n\*n)* time complexity.

Fortunately, there is room for improvement. Let’s consider two neighbour nodes in the tree.

When we would calculate their corresponding sum of distances, the formulas would be:

- *distance_sum(x) = subtree_sum(x) + subtree_sum(y) + node_count(y)*
- *distance_sum(y) = subtree_sum(y) + subtree_sum(x) + node_count(x)*
- *distance_sum(x) - distance_sum(y) = node_count(y) - node_count(x)*

This formula gives us the opportunity to calculate the answer for a child from the value of a parent.

- *distance_sum(child) = distance_sum(parent) + node_count(parent) - node_count(child)*
- *distance_sum(child) = distance_sum(parent) + (total_nodes - node_count(child)) - node_count(child)*
- *distance_sum(child) = distance_sum(parent) + total_node - 2\*node_count(child)*

Therefore, after we calculate the sum of distances for one root node using the post-order traversal, we can traverse the tree using pre-order traversal, filling in the missing values for the non-root nodes.

This leaves us with the optimal *O(n)* solution.

<!-- https://compiler-explorer.com/z/59zxjq1ca -->

### Well-behaved paths in a tree

This is a reasonably tricky problem. If we had only a binary tree, we could use a DFS and calculate the number of paths for each parent node based on the available values and the number of paths in the subtrees. Unfortunately, this approach would blow up in complexity with a general tree. So we need something more sophisticated.

Instead of considering the parent nodes as connection points, let’s consider the edges (each edge divides the tree into two sub-trees).

As a reminder, a valid path requires that both ends have the same value, and all intermediate nodes are, at most, equal to the ends of the path. This hints at an ordered approach.

And, if we process the edges in the order of their maximum value, i.e. *std::max(value[node_left], value[node_right])*, we get some interesting guarantees.

- The maximum value in either of the connected subtrees is at most *std::max(value[node_left], value[node_right])*, and in fact, that has to be the maximum value in at least one of the subtrees.
- If the maximum value in one of the subtrees is lower than *std::max(value[node_left], value[node_right])*, no valid paths are crossing this edge (since the maximum node creates a natural barrier).
- If the maximum value in both of the subtrees is *std::max(value[node_left], value[node_right])*, then this edge adds *freq_of_max[left]\*freq_of_max[right]* valid paths. From each node with maximum value in the left subtree to each node with maximum value in the right subtree.

While this sounds like we have solved the problem, we have created a big issue for ourselves. We need to track the frequencies for the maximum (fortunately, only the maximum) value in each connected subtree, and we need to do it in a way that allows us to look up this value based on any node that is part of this tree (since the newly added edge can connect to any of them).

Fortunately, the UnionFind algorithm allows us to keep track of a connected subtree by tracking a representative node for each subtree (and each node of that subtree). In our case, we want the representative node to have the maximum value. The part that we care about here is the complexity of the Find operation, which is *O(α(n))*, where α is the inverse Ackerman function (which is <5 for all practical inputs), i.e. O(1) for practical purposes.

<!-- https://compiler-explorer.com/z/5boshjqv1 -->

### Number of reorders of a serialized BST

First, let’s remind ourselves how Binary Search trees operate. For each node, all the nodes in the left subtree are lower than the value of this node, and all nodes in the right subtree are higher than the value of this node.

This allows us to find any node with a specific value in *log(n)*.

Determining the number of reorderings that produce the same BST is much less obvious. However, the first fairly straightforward observation is that the first node will always be the root node, and it creates a partition over the other nodes (for the left and right subtrees).

This points to the first part contributing to the total count of reorderings. Any reordering of elements that produces the same stable partition will lead to the same BST.

Let’s consider the permutation *{3,1,2,4,5}*. Changing the order of elements within each partition (i.e. *{1,2}*, *{4,5}*) would produce different partitions; however, we can freely interleave these partitions without changing the result. More formally, we are looking for the number of ways to pick the positions for the left (or right) partition out of all positions, i.e. *C(n-1,k)* (binomial coefficient), where n is the total number of elements in the permutation and *k* is the number of elements in the left partition.

The second point we have not considered is the number of reorderings in the two sub-tree, which we can calculate recursively.

This leads to a total formula: *reorderings(left)\*reorderings(right)\*coeff(n-1,left.size())*.

This implies that we will have to pre-calculate the binomial coefficients, which we can do using Pascal’s triangle.

Finally, we need to apply the requested modulo operation where appropriate.

<!-- https://compiler-explorer.com/z/shvje5T8j -->