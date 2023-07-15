{full: true, community: true}
## Hints

### Locked rooms

1. Think about the keys.
2. The question we need to answer is whether we can collect all keys.
3. We do not need to find the shortest route; therefore, a depth-first search will be good enough.

### Bus routes

1. Don't think in terms of bus stops.
2. You will need to pre-compute something.
3. Pre-computing a connection mapping, which other buses we can switch to, will allow you to traverse over the buses.
4. We need the shortest path and must use a breadth-first search.

### Counting islands

1. If we traverse a potential island, we will visit all its and neighbouring spaces.
2. What type of space will we not encounter if the landmass is an island?

### All valid parentheses

1. What are the properties that hold true for a valid parentheses sequence?
2. We only have n pairs of parentheses. This leads to one constraint.
3. We can only add a right parenthesis under specific circumstances. This leads to one constraint.
4. We need the backtracking algorithm to find all paths that satisfy the above constraints.

### Sudoku solver

1. We have walked through the solution for a very similar problem.
2. Try modifying the solution for N-Queens.
3. What constraints can we propagate to improve the solving performance?