{full: true, community: true}
# Traversal algorithms

In this chapter, we will discuss the three foundational algorithms we will keep revisiting throughout the rest of the book.

Let's start with a problem: imagine you need to find a path in a maze. How would you do it?

![Maze with one entrance and one exit.](traversal/maze_base.png)

Your first intuition might follow the depth-first search algorithm, opportunistically exploring until you reach a dead-end, then returning to the last crossroads and taking a different path.

## Depth-first search

The depth-first search opportunistically picks a direction at each space and explores that direction fully before returning to this space and picking a different path.

A typical approach would use a consistent strategy for picking the direction order: e.g., north, south, west, east; however, as long as the algorithm explores every direction, the order doesn't matter and can be randomized.

![Depth-first search using the NSWE strategy.](traversal/maze_dfs.png)

Because of the repeating nested nature, a recursive implementation is a natural fit for the depth-first search.

{caption: "Recursive implementation of a depth-first search."}
```cpp
bool dfs(int64_t row, int64_t col,
         std::vector<std::vector<char>>& map) {
    // Check for out of bounds.
    if (row < 0 || row == std::ssize(map) ||
        col < 0 || col == std::ssize(map[row]))
        return false;

    // If we have reached the exit, terminate the search.
    if (map[row][col] == 'E')
        return true;
    // If this is not an unvisited space, do not
    // terminate but, also do not continue.
    if (map[row][col] != ' ')
        return false;

    // Mark this space as visited.
    map[row][col] = '.';

    return dfs(row-1,col,map) || // North 
           dfs(row+1,col,map) || // South
           dfs(row,col-1,map) || // West
           dfs(row,col+1,map);   // East
}
```

<!-- https://compiler-explorer.com/z/TMGzEoE6a -->

We can flatten the recursive version using a stack data structure. However, we need to remember the LIFO nature of a stack. The order of exploration will be inversed from the order in which we insert the elements into the stack.

{caption: "Implementation of depth-first search using a std::stack."}
```cpp
bool dfs(int64_t row, int64_t col,
         std::vector<std::vector<char>>& map) {
    std::stack<std::pair<int64_t,int64_t>> next;
    next.push({row,col});

    // As long as we have spaces to explore.
    while (!next.empty()) {
        auto [row,col] = next.top();
        next.pop();

        // If this space is the exit, we have found our path.
        if (map[row][col] == 'E')
            return true;

        // Mark as visited
        map[row][col] = '.';

        // Helper to check if a space can be stepped on
        // i.e. not out of bounds and either empty or exit.
        auto is_path = [&map](int64_t row, int64_t col) {
            return row >= 0 && row < std::ssize(map) &&
                col >= 0 && col < std::ssize(map[row]) &&
                (map[row][col] == ' ' || map[row][col] == 'E');
        };
        
        // Due to the stack data structure we need to insert 
        // elements in the reverse order we want to explore.
        if (is_path(row,col+1)) // East
            next.push({row,col+1}); 
        if (is_path(row,col-1)) // West
            next.push({row,col-1});
        if (is_path(row+1,col)) // South
            next.push({row+1,col});
        if (is_path(row-1,col)) // North
            next.push({row-1,col});
    }

    // We have explored all reachable spaces 
    // and didn't find the exit.
    return false;
}
```

<!-- https://compiler-explorer.com/z/vds8Wh5Yd -->

## Breadth-first search

While the depth-first search is excellent for finding a path, we don't necessarily get the shortest path. To find the shortest path, we can use the breadth-first search algorithm.

The algorithm visits spaces in lock-step, first visiting all spaces next to the starting point, then all spaces next to those, i.e., two spaces away from the start, then three, four, etc. To visualize, you can think about how water would flood the maze from the starting point.

![Breadth-first search demonstration.](traversal/maze_bfs.png)

When implementing a breadth-first search, we need a data structure that will allow us to process the elements in the strict order we discover them, a queue.

{caption: "Implementation of breadth-first search using a std::queue."}
```cpp
int64_t bfs(int64_t row, int64_t col, std::vector<std::vector<char>>& map) {
    std::queue<std::tuple<int64_t,int64_t,int64_t>> next;
    next.push({row,col,0});

    // As long as we have spaces to explore.
    while (!next.empty()) {
        auto [row,col,dist] = next.front();
        next.pop();

        // If this space is the exit, we have found our path.
        // Return the current length.
        if (map[row][col] == 'E')
            return dist;

        // Mark as visited
        map[row][col] = '.';

        // Helper to check if a space can be stepped on
        // i.e. not out of bounds and either empty or exit.
        auto is_path = [&map](int64_t row, int64_t col) {
            return row >= 0 && row < std::ssize(map) &&
                col >= 0 && col < std::ssize(map[row]) &&
                (map[row][col] == ' ' || map[row][col] == 'E');
        };
        
        if (is_path(row-1,col)) // North
            next.push({row-1,col,dist+1});
        if (is_path(row+1,col)) // South
            next.push({row+1,col,dist+1});
        if (is_path(row,col-1)) // West
            next.push({row,col-1,dist+1});
        if (is_path(row,col+1)) // East
            next.push({row,col+1,dist+1});
    }

    // We have explored all reachable spaces 
    // and didn't find the exit.
    return -1;
}
```

<!-- https://compiler-explorer.com/z/nvsdKsqcd -->

## Backtracking

Both depth-first and breadth-first searches are traversal algorithms that attempt to reach a specific goal. The difference between algorithms is only in the order in which they traverse the space.

However, in some situations, we may not know the goal and only know the properties the path toward the goal must fulfill.

The backtracking algorithm explores the solution space in a depth-first order, discarding paths that do not fulfill the requirements.

Let's take a look at a concrete example: The N-Queens problem. The goal is to place N-Queens onto an NxN chessboard without any of the queens attacking each other, i.e., no queens sharing a row, column, or diagonal.

![Demonstration of backtracking for the 4-Queens problem.](traversal/backtracking.png)

The paths we explore are partial but valid solutions that build upon each other. In the above example, we traverse the solution space in row order. First, we pick a position for a queen in the first row, then second, then third, and finally fourth. The example also demonstrates two dead-ends we reach if we place the queen in the first row into the first column.

{caption: "Example implementation of backtracking."}
```cpp
#include <vector>
#include <cstdint>

// Check if we can place a queen in the specified row and column
bool available(std::vector<int64_t>& solution, 
               int64_t row, int64_t col) {
    for (int64_t queen = 0; queen < std::ssize(solution); ++queen) {
        // Column occupied
        if (solution[queen] == col)
            return false;
        // NorthEast/SouthWest diagonal occupied
        if (row + col == queen + solution[queen])
            return false;
        // NorthWest/SouthEast diagonal occupied
        if (row - col == queen - solution[queen])
            return false;
    }
    return true;
}

bool backtrack(std::vector<int64_t>& solution, int64_t n) {
    if (std::ssize(solution) == n)
        return true;

    // We are trying to fit a queen on row std::ssize(solution)
    for (int64_t column = 0; column < n; ++column) {
        if (!available(solution, std::ssize(solution), column))
            continue;
      
        // This space is not in conflict
        solution.push_back(column);
        // We found a solution, exit
        if (backtrack(solution, n))
            return true;
        // This was a dead-end, remove the queen from this position
        solution.pop_back();
    }

  	// This is a dead-end
    return false;
}
```

<!-- https://compiler-explorer.com/z/WGeh6abv5 -->