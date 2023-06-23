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

We can flatten the recursive version using a stack data structure.

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