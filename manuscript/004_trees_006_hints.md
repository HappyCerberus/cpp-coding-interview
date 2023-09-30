{full: true, community: true}
## Hints

### Serialise and de-serialise n-ary tree

1. We have covered an approach for (de)serializing a binary tree.
2. Use pre-order traversal with a format that can represent the list of children.
3. You will need to store the number of children or use a terminal value.

### Find all nodes of distance *k* in a binary tree

1. Can you calculate the value for a child if you know the distance for the parent node?
2. Consider whether the child lies on the path towards the target or not.
3. You will need a two-step process: finding the path to the target from the root and then calculating the distances.

### Sum of distances to all nodes

1. Can you calculate the value for the root node?
2. You can use post-order traversal to calculate the sum of distance for the root node from the children trees.
3. What would happen if we removed one edge, calculated the values for the two roots, and then recombined?
4. You should be able to derive a straightforward formula for calculating the sum of distances for a child if you know the value for the parent node.
5. If you consider the values, a straightforward formula for calculating the sum of distances for a child from the parent node will pop out.
6. Once you have the formula, a pre-order traversal will allow you to fill in the missing values.

### Well-behaved paths in a tree

1. You will want to iterate over edges instead of nodes.
2. Suppose we connect two sub-trees with an edge, and the maximum value in both sub-trees is the same. In that case, we can trivially calculate the number of paths this connection generates if you know the number of instances of the maximum value in both trees.
3. You will need the union-find algorithm for efficient lookup.

### Number of reorders of a serialized BST

1. What is the role of the first value in the serialized tree?
2. We end up with two partitions; what operation can we do that doesn't affect the value of these partitions?
3. You will need to pre-calculate the Pascal triangle.