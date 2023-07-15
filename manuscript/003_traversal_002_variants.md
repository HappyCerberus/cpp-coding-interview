{full: true, community: true}
## Notable variants

The traversal algorithms mentioned earlier are largely standalone, ready to be deployed to solve diverse problems with minimal tweaks.

Yet, we frequently encounter a few additional versions. In this section, we'll tackle three such variants: traversing multiple dimensions, adjusting for non-unit costs, and managing the propagation of constraints.

We'll also illustrate each variant using a concrete problem, all of which you can find in the companion repository. Try solving each of them before you read the corresponding solution.

### Multi-dimensional traversal

Applying a depth-first or breadth-first search to a problem with additional spatial dimensions is straightforward. From the algorithm's perspective, additional dimensions only introduce a broader neighborhood for each space. However, in some problems, the additional dimensions will not be that obvious.

Consider the following problem: Given a 2D grid of size *m\*n*, containing *0s* (spaces) and *1s* (obstacles), determine the length of the shortest path from the coordinate *{0,0}* to *{m-1,n-1}*, given that you can remove up to *k* obstacles.

{class: tip}
B> Before you continue reading, try solving this problem yourself. The scaffolding for this problem is located at `traversal/obstacles`. Your goal is to make the following commands pass without any errors: `bazel test //traversal/obstacles/...`, `bazel test --config=addrsan //traversal/obstacles/...`, `bazel test --config=ubsan //traversal/obstacles/...`.

Because we are looking for the shortest path, we don't have a choice of the traversal algorithm. We must use a breadth-first search. But how do we deal with the obstacles?

Let's consider adding a 3rd dimension to the problem. Instead of removing an obstacle, we can virtually move to a new maze floor, where this obstacle never existed. However, this introduces a problem. We can't apply this logic mindlessly since there are potentially *m\*n* obstacles.

Fortunately, we can lean on the behaviour of breadth-first search. When we enter a new floor of the maze, we have a guarantee that we will never revisit the space we entered through. This means we do not have to track which specific obstacles we removed, only how many we can still remove, shrinking the number of floors to *k+1*. Applying a breadth-first search then leaves us with a total time complexity of *O(m\*n\*(k+1))*.

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

            // If we have already removed k obstacles, 
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

In all the problems we discussed, the cost of moving from one space to another was always one unit. The breadth-first search algorithm explicitly relies on this property to process spaces strictly by distance.

Therefore if we are working with a problem that doesn't have unit cost, we must adjust our approach.

Consider the following problem: Given a 2D heightmap of size *m\*n*, where negative integers represent impassable terrain and positive integers represent the terrain height, determine the shortest path between the two given coordinates under the following constraints:

- the path cannot cross impassable terrain
- moving on a level terrain costs two time-units
- moving downhill costs one time-unit
- moving uphill costs four time-units

{class: tip}
B> Before you continue reading, try solving this problem yourself. The scaffolding for this problem is located at `traversal/heightmap`. Your goal is to make the following commands pass without any errors: `bazel test //traversal/heightmap/...`, `bazel test --config=addrsan //traversal/heightmap/...`, `bazel test --config=ubsan //traversal/heightmap/...`.

The primary requirement of BFS is that we process elements in the order of their distance from the start of the path. When all transitions have a unit cost, we can achieve this by relying on a queue. However, with non-unit costs, we must use an ordered structure such as *std::priority_queue*. Note that switching to a priority queue will affect the time complexity as we are moving from *O(1)* *push* and *pop* operations to *O(log(n))* *push* and *pop* operations.

The second guarantee we lose concerns the shortest path when we first push a space into the queue. If we discovered a space with a path length *X* we had a guarantee that all later paths that also lead to this space would, at best equal *X*. Because of this constraint, we could limit ourselves to adding each space into the queue only once. With non-unit costs, this property no longer holds.

It is possible that a later (and longer) path can enter the same space with an overall shorter path length. Consequently, we might need to insert a space multiple times into our queue (bounded by the number of neighbours). However, we still have a slightly weaker but still significant guarantee. The ordered nature of the priority queue guarantees that the first time we pop a space from the queue, it is part of the shortest path that enters this space.

Due to the queue's logarithmic complexity, we end up with *O(m\*n\*log(m\*n))* overall time complexity for the breadth-first search.

{caption: "Breadth-first search using a priority queue to handle non-unit costs."}
```cpp
#include <vector>
#include <queue>
#include <cstdint>

struct Coord {
    int64_t row;
    int64_t col;
};

int64_t shortest_path(const std::vector<std::vector<int>>& map, 
                        Coord start, Coord end) {
    struct Pos {
        int64_t length;
        Coord coord;
    };

    // For tracking visited spaces
    std::vector<std::vector<bool>> visited(map.size(), 
        std::vector<bool>(map[0].size(), false));

    // Helper to check whether a space can be stepped on
    // not out of bounds, not impassable and not visited
    auto can_step = [&map, &visited](Coord coord) {
        auto [row, col] = coord;
        return row >= 0 && col >= 0 && 
            row < std::ssize(map) && col < std::ssize(map[row]) && 
            map[row][col] >= 0 && 
            !visited[row][col];
    };

    // Priority queue instead of a simple queue
    std::priority_queue<Pos, std::vector<Pos>, 
        decltype([](const Pos& l, const Pos& r) {
            return l.length > r.length;
        })> q;
    // Start with path length zero at start
    q.push({0,start});

    // Helper to determine the cost of moving between two spaces
    auto step_cost = [&map](Coord from, Coord to) {
        if (map[from.row][from.col] < map[to.row][to.col]) return 4;
        if (map[from.row][from.col] > map[to.row][to.col]) return 1;
        return 2;
    };

    while (!q.empty()) {
        // Grab the position closest to the start
        auto [length, pos] = q.top();
        q.pop();

        if (visited[pos.row][pos.col]) continue;
        // The first time we grab a position from the queue is guaranteed
        // to be the shortest path, so now we need to mark it as visited.
        // If we later visit the same position (already in queue at this point)
        // with a longer path, we skip it based on the above check.
        visited[pos.row][pos.col] = true;

        // First time we would try to exit the end space is the shortest path.
        if (pos.row == end.row && pos.col == end.col)
            return length;

        // Expand to all four directions
        for (auto next : {Coord{pos.row-1,pos.col}, 
                          Coord{pos.row+1, pos.col},
                          Coord{pos.row, pos.col-1},
                          Coord{pos.row, pos.col+1}}) {
            if (!can_step(next)) continue;
            q.push({length + step_cost(pos, next), next});
        }
    }

    return -1;
}
```

<!-- https://compiler-explorer.com/z/3PjxPz8Tx -->

### Constraint propagation

In the previous section, we used backtracking to solve the N-Queens problem. However, if you look at the implementation, we repeatedly check each new queen against all previously placed queens. We can do better.

When working with backtracking, we cannot escape the inherent exponential complexity of the worst case. However, we can often significantly reduce the exponents by propagating the problem's constraints forward. The main objective is to remove as many options from the consideration altogether by ensuring that the constraints are maintained

{class: tip}
B> Before you continue reading, try modifying the previous version yourself. The scaffolding for this problem is located at `traversal/queens`. Your goal is to make the following commands pass without any errors: `bazel test //traversal/queens/...`, `bazel test --config=addrsan //traversal/queens/...`, `bazel test --config=ubsan //traversal/queens/...`.

Specifically for the N-Queens problem, we have *N* rows, *N* columns, *2\*N-1* NorthWest, and *2\*N-1* NorthEast diagonals. Placing a queen translates to claiming one row, column, and the corresponding diagonals. Instead of checking each queen against all previous queens, we can limit ourselves to checking whether the corresponding row, column, or one of the two diagonals was already claimed.

{caption: "Solving the N-Queens problem with backtracking and constraint propagation."}
```cpp
// Helper to store the current state:
struct State {
    State(int64_t n) : n(n), solution{}, cols(n), nw_dia(2*n-1), ne_dia(2*n-1) {}
    // Size of the problem.
    int64_t n;
    // Partial solution
    std::vector<int64_t> solution;
    // Occupied columns
    std::vector<bool> cols;
    // Occupied NorthWest diagonals
    std::vector<bool> nw_dia;
    // Occupied NorthEast diagonals
    std::vector<bool> ne_dia;
    // Check column, and both diagonals
    bool available(int64_t row, int64_t col) const {
        return !cols[col] && !nw_dia[row-col+n-1] && !ne_dia[row+col];
    }
    // Mark this position as occupied and add it to the partial solution
    void mark(int64_t row, int64_t col) {
        solution.push_back(col);
        cols[col] = true;
        nw_dia[row-col+n-1] = true;
        ne_dia[row+col] = true;
    }
    // Unmark this position as occupied and remove it from the partial solution
    void erase(int64_t row, int64_t col) {
        solution.pop_back();
        cols[col] = false;
        nw_dia[row-col+n-1] = false;
        ne_dia[row+col] = false;
    }
};

bool backtrack(auto& state, int64_t row, int64_t n) {
    // All Queens have their positions, we have solution
    if (row == n) return true;

    // Try to find a feasible column on this row
    for (int c = 0; c < n; ++c) {
        if (!state.available(row,c))
            continue;
        // Mark this position
        state.mark(row,c);
        // Recurse to the next row
        if (backtrack(state, row+1, n))
            return true; // We found a solution on this path
        // This position lead to dead-end, erase and try another
        state.erase(row,c);
    }
    // This is dead-end
    return false;
}
```