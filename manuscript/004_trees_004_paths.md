{full: true, community: true}
## Paths in trees

Another ubiquitous category of tree-oriented problems is dealing with paths in trees. Notably, paths in trees can be non-trivial to reason about, but at the same time, the tree structure still offers the possibility for very efficient solutions.

Let's demonstrate this on a concrete interview problem.

### Maximum path in a tree

Given a binary tree, where each node has an integer value, determine the maximum path in this tree.

A path is a sequence of nodes where every two consecutive nodes have a parent/child relationship, and each node is visited at most once. The value of a path is then simply the total of all the node values visited.

<!-- Block pointing to the problem in the repo. -->
<!-- https://simontoth.substack.com/p/daily-bite-of-c-maximum-path-in-a -->

Let's consider a single node in the tree. Only four possible paths can be the maximum path that crosses this node:

- a single-node path that contains this node only
- a path coming from the left child, terminating in this node
- a path coming from the right child, terminating in this node
- a path crossing this node, i.e. going from one child, crossing this node, continuing to the other child

Considering the above list, we can limit the information we need to calculate the maximum path in a sub-tree whose root is the above node.

If the maximum path doesn't cross this node, then the maximum path in this tree exists inside one of the child subtrees.

If the path crosses this node, we can calculate the maximum path by using the information about maximum paths that terminate in the left and right child.

- a single-node path is simply the value of the node
- a path coming from the left child terminating in this node is the value of the maximum path terminating in the left child, plus the value of this node
- a path coming from the right child terminating in this node is the value of the maximum path terminating in the right child, plus the value of this node
- a path crossing this node is the value of the maximum path terminating on the right child, plus the value of the maximum path terminating in the left child, plus the value of this node

The maximum path crossing this node is the maximum of the above paths.

Now that we know what to calculate, we can traverse the tree in post-order (visiting the children before the parent node) while keeping track of the aforementioned values.