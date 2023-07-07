{full: true, community: true}
## Notable variants

All three traversal algorithms discussed in the previous section are reasonably self-contained. Typically traversal algorithms can be applied as solutions to various problems without substantial modifications.

However, a couple of more advanced variants are also reasonably frequent. This section will discuss three: working with multiple dimensions, non-unit costs, and propagating constraints.

### Multi-dimensional traversal

Applying a depth-first or breadth-first search to a problem with additional spatial dimensions should be straightforward. From the algorithm's perspective, additional dimensions only introduce a broader neighborhood for each space. In some problems, the additional dimensions will not be that obvious.

Consider the following problem: Given a 2D grid of size *m\*n*, containing *0s* (spaces) and *1s* (obstacles), determine the length of the shortest path from the coordinate *{0,0}* to *{m-1,n-1}*, given that you can remove up to *k* obstacles.

{class: tip}
B> Before you continue reading, try solving this problem yourself. The scaffolding for this problem is located at `traversal/obstacles`. Your goal is to make the following commands pass without any errors: `bazel test //traversal/obstacles/...`, `bazel test --config=addrsan //traversal/obstacles/...`, `bazel test --config=ubsan //traversal/obstacles/...`.

Because we are looking for the shortest path, we don't have a choice of the traversal algorithm. We must use a breadth-first search. But how do we deal with the obstacles?

Let's consider adding a 3rd dimension to the problem. When we remove an obstacle, we move to a new floor of the maze where that obstacle never existed. However, we can't apply this logic mindlessly since there are potentially *m\*n* obstacles.

Fortunately, we can lean on the behaviour of breadth-first search. When we enter a new floor of the maze, we have a guarantee that we will never revisit the space we entered through. This means we do not have to track which specific obstacles we removed, only how many we can still remove. Therefore we end up with a total complexity of *m\*n\*(k+1)*.

{caption: "Breadth-first search in a maze with obstacle removal."}
```cpp
#include <vector>
#include <queue>
#include <cstdint>

struct Dir {
    int64_t row;
    int64_t col;
};

struct Pos {
    int64_t row;
    int64_t col;
    int64_t k;
    int64_t distance;
};

int shortest_path(const std::vector<std::vector<int>>& grid, int64_t k) {
    // Keep track of visited spaces, initialize all spaces as unvisited.
    std::vector<std::vector<std::vector<bool>>> visited(
        grid.size(), std::vector<std::vector<bool>> (
            grid[0].size(), std::vector<bool>(k+1, false)
        )
    );

    // BFS
    std::queue<Pos> q;
    // start in {0,0} with zero removed obstacles
    q.push(Pos{0,0,0,0});
    visited[0][0][0] = true;

    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        // The first time we visit the end coordinate is the shortest path
        if (current.row == std::ssize(grid)-1 && 
            current.col == std::ssize(grid[current.row])-1) {
            return current.distance;
        }

        // For every direction, try to move there
        for (auto dir : {Dir{-1,0}, Dir{1,0}, Dir{0,-1}, Dir{0,1}}) {
            // This space is out of bounds, ignore.
            if ((current.row + dir.row < 0) || 
                (current.col + dir.col < 0) ||
                (current.row + dir.row >= std::ssize(grid)) ||
                (current.col + dir.col >= std::ssize(grid[0])))
                continue;

            // If the space in the current direction is an empty space:
            Pos empty = {current.row + dir.row, current.col + dir.col, 
                         current.k, current.distance + 1};
            if (grid[empty.row][empty.col] == 0 && 
                !visited[empty.row][empty.col][empty.k]) {
                // add it to the queue
                q.push(empty);
                // and mark as visited
                visited[empty.row][empty.col][empty.k] = true;
            }

            // If we have have already removed k obstacles, 
            // we don't consider removing more.
            if (current.k == k)
                continue;

            // If the space in the current direction is an obstacle:
            Pos wall = {current.row + dir.row, current.col + dir.col,
                        current.k + 1, current.distance + 1};
            if (grid[wall.row][wall.col] == 1 && 
                !visited[wall.row][wall.col][wall.k]) {
                // add it to the queue
                q.push(wall);
                // and mark as visited
                visited[wall.row][wall.col][wall.k] = true;
            }
        }
    }

    // If we are here, we did not reach the end coordinate.
    return -1;
}
```

<!-- https://compiler-explorer.com/z/WbY87xodG -->

### Shortest path with non-unit costs

<!-- uneven terrain dfs -->

### Constraint propagation

<!-- n-queens with propagation -->