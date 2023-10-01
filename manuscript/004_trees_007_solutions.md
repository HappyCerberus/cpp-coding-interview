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
    // we have to use int64_t to represent all values for uint32_t and -1
    int64_t value; 
    while (s >> value) {
        if (value < 0) // if we read -1, we are done reading 
                       // the children of this node
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

{width: "35%"}
![Example that demonstrates the distance changing between children on the path to the target node or not.](trees/kdistance_distances.png)

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

We will start with a simpler sub-problem: calculate the sum of distances to all nodes for the root node only.

Let's consider a node with a child subtree represented by the left child. When we move from the child to the parent, we increase the distance to all nodes in this subtree by one or put another way, we increase the total distance by *node_count(left_subtree)*.

Therefore, if we want to calculate the sum of distances for the root node, we can do a post-order traversal. At each node, we calculate the sum of distances as *sum_of_distances(left) + node_count(left) + sum_of_distances(right) + node_count(right)*.

Because this approach only gives us the solution to the root node, applying this process to the entire tree would require rotating the tree, but more importantly, it would lead to *O(n\*n)* complexity.

Fortunately, we can do better.

Instead of focusing on the nodes, let's focus on the edges between them. Let's consider a specific edge that we have removed from the tree, and we have calculated the sum of distances for the two nodes originally connected by the removed edge.

{width: "20%"}
![Example of a disconnected tree with the two highlighted nodes for which we have calculated the sum of distances values.](trees/sum_structure.png)

We can reconstruct the sum of distances for the connected tree from the two disjoint values.

- *sum_of_distances(a) = disconnected_sum(a) + disconnected_sum(b) + node_count(b)*
- *sum_of_distances(b) = disconnected_sum(b) + disconnected_sum(a) + node_count(a)*
- *sum_of_distances(a) - sum_of_distances(b) = node_count(b) - node_count(a)*

This formula gives us the opportunity to calculate the answer for a child from the value of a parent.

- *sum_of_distances(child) = sum_of_distances(parent) + node_count(parent) - node_count(child)*
- *sum_of_distances(child) = sum_of_distances(parent) + (total_nodes - node_count(child)) - node_count(child)*
- *sum_of_distances(child) = sum_of_distances(parent) + total_node - 2\*node_count(child)*

After we have calculated the sum of distances for the root node with post-order traversal, we do a second traversal, this time in pre-order, filling in values for all nodes using the above formula.

This gives us a much better *O(n)* time complexity.

{caption: "Solution for the sum of distances problem."}
```cpp
struct TreeInfo {    
    TreeInfo(int n) : subtree_sum(n,0), node_count(n,0), result(n,0) {}
    std::vector<int> subtree_sum;
    std::vector<int> node_count;
    std::vector<int> result;
};

void post_order(int node, int parent, 
                const std::unordered_multimap<int,int>& neighbours, TreeInfo& info) {
    // If there are no children we have zero distance and one node.
    info.subtree_sum[node] = 0;
    info.node_count[node] = 1;

    auto [begin, end] = neighbours.equal_range(node);
    for (auto [from, to] : std::ranges::subrange(begin, end)) {
        // Avoid looping back to the node we came from.
        if (to == parent) continue;
        // post_order traversal, visit children first
        post_order(to, node, neighbours, info);
        // accumulate number of nodes and distances
        info.subtree_sum[node] += info.subtree_sum[to] + info.node_count[to];
        info.node_count[node] += info.node_count[to];
    }
}

void pre_order(int node, int parent,
               const std::unordered_multimap<int,int>& neighbours, TreeInfo& info) {
    // For the root node the subtree_sum matches the result.
    if (parent == -1) {
        info.result[node] = info.subtree_sum[node];
    } else {
        // Otherwise, we can calculate the result from the parent,
        // because in pre-order we visit the parent before the children.
        info.result[node] = info.result[parent] + info.result.size() 
                            - 2*info.node_count[node];
    }        
    // Now visit any children.
    auto [begin, end] = neighbours.equal_range(node);
    for (auto [from, to] : std::ranges::subrange(begin, end)) {
        if (to == parent) continue;
        pre_order(to, node, neighbours, info);
    }
}

std::vector<int> distances_in_tree(
    int n, const std::unordered_multimap<int,int> neighbours) {
    TreeInfo info(n);
    // post-order pass to calculate subtree_sum and node_count
    post_order(0,-1,neighbours,info);
    // pre-order pass to calculate result
    pre_order(0,-1,neighbours,info);
    return info.result;
}
```

<!-- https://compiler-explorer.com/z/59zxjq1ca -->

### Well-behaved paths in a tree

This is a reasonably tricky problem. If the problem was limited to binary trees, we could use post-order traversal, keep track of values not blocked by parent nodes with higher values and then construct intersections from the left and right subtrees.

We can't apply this simple logic to an n-arry tree as we would have to check every subtree against every other subtree at each node, quickly exploding the complexity.

Instead of considering the nodes, let's think in terms of edges. Because we are working with a tree, each edge divides the tree into two trees.

As a reminder, a valid path requires that both ends have the same values and all intermediate nodes are, at most, equal to the ends of the path. This gives us a hint towards using an ordered approach.

Let's start with an empty tree with no edge and slowly add the edges in non-descending order based on the values of the nodes on their ends, i.e. *std::max(value[node_left], value[node_right])*.

This gives us some interesting properties:

- The maximum value in either of the trees we are connecting by adding this edge is at most *std::max(value[node_left], value[node_right])*, and that has to be the maximum value in at least one of the trees (because both nodes already exist in those trees).
- If the maximum value in one of the subtrees is lower than *std::max(value[node_left], value[node_right])*, no valid paths are crossing this edge (since the maximum node creates a barrier).
- If the maximum value in both of the subtrees is *std::max(value[node_left], value[node_right])*, then this edge adds *freq_of_max[left]\*freq_of_max[right]* valid paths. From each node node with the maximum value in the left subtree to each node with the maximum value in the right subtree.

While this looks like a complete solution, we have a big problem. How do we efficiently keep track of the frequencies of the maximum values? Not only that, a new edge might be connecting to any node in a connected subtree, so we also need to be able to retrieve the frequency for a connected subtree based on any node in this subtree.

Fortunately, the UnionFind algorithm offers a solution. Union find can keep track of connected components by keeping track of a representative node for each component. In our case, the components are subtree, and we additionally want the representative node to be one of the nodes with the maximum value.

{caption: "Solution for the well-behaved paths problem."}
```cpp
// UnionFind
int64_t find(std::vector<int64_t>& root, int64_t i) {
    if (root[i] == i) // This is the representative node for this subtree
        return i;
    // Otherwise find the representative node and cache for future lookups.
    // The caching is what provides the O(alpha(n)) complexity.
    return root[i] = find(root, root[i]);
}

int64_t well_behaved_paths(std::vector<int64_t> values, 
                           std::vector<std::pair<int64_t,int64_t>> edges) {
    // Start with all nodes disconnected, each node is the
    // representative node of its subtree.
    std::vector<int64_t> root(values.size(), 0);
    std::ranges::iota(root,0);

    // The frequencies of the maximum value in each subtree.
    std::vector<int64_t> freq(values.size(), 1);

    // Start with trivial paths.
    int64_t result = values.size();

    std::ranges::sort(edges, [&](auto& l, auto& r) {
        return std::max(values[l.first], values[l.second]) < 
            std::max(values[r.first], values[r.second]);
    });

    for (auto &edge : edges) {
        // Find the representative nodes for the two ends.
        // The representative nodes are always the maximum value nodes.
        int64_t l_max = find(root, edge.first);
        int64_t r_max = find(root, edge.second);

        // The maximum in both subtrees is the same.
        if (values[l_max] == values[r_max]) {
            // Add path from each maximum node in left subtree to each
            // maximum node in right subtree.
            result += freq[l_max]*freq[r_max];

            // Merge the trees right into left
            freq[l_max] += freq[r_max];
            root[r_max] = l_max;
        } else if (values[l_max] > values[r_max]) {
            // No new paths, but merge the trees
            // right into left.
            root[r_max] = l_max;
            // This doesn't change the frequency because
            // all nodes in r_max subtree < values[l_max].
        } else { // (values[r_max] > values[l_max])
            // No new paths, but merge the trees
            // left into right.
            root[l_max] = r_max;
            // This doesn't change the frequency because
            // all nodes in l_max subtree < values[r_max].
        }
    }
    return result;
}
```

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

{caption: "Solution for the number of BST reorders problem."}
```cpp
constexpr inline int32_t mod = 1e9+7;

int32_t count(std::span<int> nums, 
              std::vector<std::vector<int>>& coef) {
    if (nums.size() < 3) return 1;

    // Partition into left and right child
    auto rng = std::ranges::stable_partition(nums, 
        [pivot = nums[0]](int v) { return v < pivot; });
    auto left = std::span(nums.begin(), rng.begin());
    // Skip the pivot, since the pivot is the parent node
    auto right = std::span(rng.begin()+1, nums.end());

    // Calculate the number of reorders for both sub-trees
    int64_t left_cnt = count(left, coef) % mod;
    int64_t right_cnt = count(right, coef) % mod;
    // Side note, we need 64bit here because we need
    // to fit 32bit*32bit in the bellow calculation.
  
    // The result is:
    // left * right * number of ways to pick positions
    // for left.size() elements in nums.size()-1 positions.
    return ((left_cnt*right_cnt) % mod)
      *coef[nums.size()-1][left.size()] % mod;
}

int32_t number_of_reorders(std::span<int> nums) {
    // Precalculate binomial coefficients upto nums.size()-1
    std::vector<std::vector<int>> binomial_coefficients;
    binomial_coefficients.resize(nums.size());
    for (int64_t i = 0; i < std::ssize(nums); ++i) {
        binomial_coefficients[i].resize(i+1, 1);
        for (int64_t j = 1; j < i; ++j)
            // Pascal's triangle
            binomial_coefficients[i][j] = 
                (binomial_coefficients[i-1][j-1] + 
                 binomial_coefficients[i-1][j]) % mod;
    }

    return (count(nums, binomial_coefficients)-1) % mod;
}
```

<!-- https://compiler-explorer.com/z/shvje5T8j -->