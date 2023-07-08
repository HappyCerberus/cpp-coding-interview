{full: true, community: true}
## Canonical problems

### Locked rooms

Given an array of n locked rooms, each room containing *0..n* distinct keys, determine whether you can visit each room. You are given the key to room zero, and each room can only be opened with the corresponding key (however, there may be *0..n* copies of that key).

Assume that you can freely move between rooms (i.e. the only thing you need is the key).

![Example of a situation where each room can be reached.](traversal/locked_rooms.png)

For example, in the above situation, we can open the red lock to collect the blue and green keys, then the green lock to collect the brown key, and finally, the remaining blue and brown locks.

### Bus routes

Given a list of bus routes, where *route[i] = {b1,b2,b3}() means that bus *i* stops at stops *b1*, *b2*, and *b3*, determine the smallest number of buses you need to reach the target bus stop starting at the source.

Return -1 if the target is unreachable.

![Example of possible sequences of bus trips for different combinations of source and target stops.](traversal/bus_routes.png)

### Counting islands

Given a map as a *std::vector\<std::vector\<char\>\>* where *'L'* represents land and *'W'* represents water, return the number of islands on the map.

An island is an orthogonally (four directions) connected area of land spaces that is fully (orthogonally) surrounded by water.

![Example of a 4x4 map with only a single island.](traversal/counting_islands.png)

For example, in the above map, we only have one island since no other land masses are fully surrounded by water.

## All valid parentheses sequences

Given n pairs of parentheses, generate all valid combinations of these parentheses.

![Example of all possible valid combinations of parentheses for three pairs of parentheses.](traversal/valid_parentheses.png)

For example, for *n==3* all valid combinations are: *()()()*, *(()())*, *((()))*, *(())()* and *()(())*.

### Sudoku solver

Given a Sudoku puzzle as *std::vector\<std::vector\<char\>\>*, where unfilled spaces are represented as a space, solve the puzzle.

Sudoku rules:

- each of the nine rows, columns and *3x3* boxes must contain all digits *1..9*

![Example of Sudoku puzzle.](traversal/sudoku_solver.png)