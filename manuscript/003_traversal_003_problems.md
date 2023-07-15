{full: true, community: true}
## Canonical problems

Traversal algorithms are possibly the most frequent algorithms during technical interviews. In this section, we will limit ourselves to only five problems that exemplify the different variants of traversal algorithms we have discussed in the last two sections.

### Locked rooms

Given an array of n locked rooms, each room containing *0..n* distinct keys, determine whether you can visit each room. You are given the key to room zero, and each room can only be opened with the corresponding key (however, there may be *0..n* copies of that key).

Assume you can freely move between rooms; the key is the only thing you need.

{class: information}
B> The scaffolding for this problem is located at `traversal/locked`. Your goal is to make the following commands pass without any errors: `bazel test //traversal/locked/...`, `bazel test --config=addrsan //traversal/locked/...`, `bazel test --config=ubsan //traversal/locked/...`.

![Example of a situation where each room can be reached.](traversal/locked_rooms.png)

For example, in the above situation, we can open the red lock to collect the blue and green keys, then the green lock to collect the brown key, and finally, open the remaining blue and brown locks.

### Bus routes

Given a list of bus routes, where *route[i] = {b1,b2,b3}* means that bus *i* stops at stops *b1*, *b2*, and *b3*, determine the smallest number of buses you need to reach the target bus stop starting at the source.

Return -1 if the target is unreachable.

{class: information}
B> The scaffolding for this problem is located at `traversal/buses`. Your goal is to make the following commands pass without any errors: `bazel test //traversal/buses/...`, `bazel test --config=addrsan //traversal/buses/...`, `bazel test --config=ubsan //traversal/buses/...`.

![Example of possible sequences of bus trips for different combinations of source and target stops.](traversal/bus_routes.png)

In the above situation, we can reach stop six from stop one by first taking the red bus and then switching to the blue bus at stop four.

### Counting islands

Given a map as a *std::vector\<std::vector\<char\>\>* where *'L'* represents land and *'W'* represents water, return the number of islands on the map.

An island is an orthogonally (four directions) connected area of land spaces that is fully (orthogonally) surrounded by water.

{class: information}
B> The scaffolding for this problem is located at `traversal/islands`. Your goal is to make the following commands pass without any errors: `bazel test //traversal/islands/...`, `bazel test --config=addrsan //traversal/islands/...`, `bazel test --config=ubsan //traversal/islands/...`.

![Example of a 4x4 map with only a single island.](traversal/counting_islands.png)

For example, in the above map, we only have one island since no other land masses are fully surrounded by water.

### All valid parentheses sequences

Given n pairs of parentheses, generate all valid combinations of these parentheses.

{class: information}
B> The scaffolding for this problem is located at `traversal/parentheses`. Your goal is to make the following commands pass without any errors: `bazel test //traversal/parentheses/...`, `bazel test --config=addrsan //traversal/parentheses/...`, `bazel test --config=ubsan //traversal/parentheses/...`.

![Example of all possible valid combinations of parentheses for three pairs of parentheses.](traversal/valid_parentheses.png)

For example, for *n==3* all valid combinations are: *()()()*, *(()())*, *((()))*, *(())()* and *()(())*.

### Sudoku solver

Given a Sudoku puzzle as *std::vector\<std::vector\<char\>\>*, where unfilled spaces are represented as a space, solve the puzzle.

In a solved Sudoku puzzle, each of the nine rows, columns, and *3x3* boxes must contain all digits *1..9*.

{class: information}
B> The scaffolding for this problem is located at `traversal/sudoku`. Your goal is to make the following commands pass without any errors: `bazel test //traversal/sudoku/...`, `bazel test --config=addrsan //traversal/sudoku/...`, `bazel test --config=ubsan //traversal/sudoku/...`.

![Example of Sudoku puzzle.](traversal/sudoku_solver.png)