{full: true, community: true}
## Solutions

### Locked rooms

Let's start with our goal. We want to determine whether we can visit all the locked rooms. However, this is a bit too complex, as we would need to consider both rooms and keys. We can simplify the problem by reformulating our goal: collect a complete set of keys.

Because we are not concerned with the optimality of our solution, only whether it is possible to collect all keys, we can choose depth-first search as our base algorithm. We will use one key in each step of our solution. Using a key will potentially give us access to more keys.

![Example of one possible DFS execution on the example problem.](traversal/locked_rooms_dfs.png)

Once we run out of keys, we can check whether we have collected a complete set. With a complete set of keys, we can visit all rooms.

{caption: "Solution for the locked rooms problem."}
```cpp
bool locked_rooms(const std::vector<std::vector<int>>& rooms) {
    // Keep track of the keys we have collected
    std::vector<bool> keys(rooms.size(),false);
    
    std::stack<int> keys_to_use;
    keys_to_use.push(0); // We start with the key to room 0
    keys[0] = true;

    while (!keys_to_use.empty()) {
        // Use the key to open a room
        int key = keys_to_use.top();
        keys_to_use.pop();
        
        // Check if any of the keys in the room are new
        for (int k : rooms[key])
        if (!keys[k]) {
            keys_to_use.push(k);
            keys[k] = true;
        }
    }

    // Do we have all the keys?
    return std::ranges::all_of(keys, std::identity{});
}
```

<!-- https://compiler-explorer.com/z/bY9q9xd85 -->

### Bus routes

We are trying to find the shortest path that minimizes the number of changes at bus stops. We could therefore use a breadth-first search and search across bus stops.

However, that poses a problem. We don't have a convenient way to determine which bus stops we can reach. We could construct a data structure representing which bus stops can be reached by a single connection. However, such a data structure would grow based on the overall number of bus stops.

We can do a lot better. Instead of considering bus stops, we can think in terms of bus lines. We still need to build a data structure that will provide the mapping of connections, i.e., for each bus line, list all other bus lines that we can switch to directly from this line. However, the big difference is that now the size of our data structure scales with the number of bus lines, not bus stops.

To construct the bus line mapping, we can sort the list of bus stops for each line and then check each pair of buses for overlap. This leads to *O(s\*logs)* complexity for the sort and *O(b\*b\*s)* for the construction of the line mapping.

Executing the breadth-first search on the bus line mapping will require *O(b\*b)* time.

{caption: "Solution for the Bus routes problem."}
```cpp
// There is no convenient is_overlapping algorithm unfortunately
bool overlaps(const std::vector<int>& left, const std::vector<int>& right) {
    ptrdiff_t i = 0; ptrdiff_t j = 0;
    while (i < std::ssize(left) && j < std::ssize(right)) {
        while (i < std::ssize(left) && 
               left[i] < right[j])
            ++i;
        while (i < std::ssize(left) && 
               j < std::ssize(right) && 
               left[i] > right[j])
            ++j;
        if (i < std::ssize(left) && 
            j < std::ssize(right) && 
            left[i] == right[j])
            return true;
    }
    return false;
}

int min_tickets(std::vector<std::vector<int>> routes, int source, int target) {
    if (source == target) { return 0; }

    // Map of bus -> connecting busses
    std::vector<std::vector<int>> connections(routes.size());
    for (auto &route : routes)
        std::ranges::sort(route);
    
    // Flag for whether a bus stops at target
    std::vector<bool> is_dest(routes.size(), false);
    // Flag for whether this bus was already visited
    std::vector<bool> visited(routes.size(), false);
    // Queue for BFS
    std::queue<std::pair<int,int>> q;

    for (ptrdiff_t i = 0; i < std::ssize(routes); ++i) {
        // The bus stops at source, one of our starting buses
        if (std::ranges::binary_search(routes[i], source)) {
            q.push({i,1});
            visited[i] = true;
        }
        // The bus stops at target
        if (std::ranges::binary_search(routes[i], target))
            is_dest[i] = true;

        // Find all other busses that connect to this bus
        for (ptrdiff_t j = i+1; j < std::ssize(routes); ++j) {
            if (overlaps(routes[i],routes[j])) {
                connections[i].push_back(j);
                connections[j].push_back(i);
            }
        }
    }
    
    // BFS
    while (!q.empty()) {
        auto [current,len] = q.front();
        q.pop();
        if (is_dest[current])
            return len;

        for (auto bus : connections[current]) {
            if (visited[bus]) continue;
            q.push({bus, len+1});
            visited[bus] = true;
        }
    }

    return -1;
}
```

### Counting islands

Our first objective is to figure out a way to determine that a connected piece of land is an island.

If we consider this problem from the perspective of traversing a piece of land, we will encounter not only the spaces this piece of land occupies but also all neighbouring spaces (otherwise, we could miss a piece of land). Therefore, we can reformulate this property.

A piece of land is an island if we do not encounter the map boundary during our traversal. Encountering a land space extends this land mass, and encountering water maintains the island property.

To ensure that we check all possible islands, we have to scan through the entire map, and when we encounter a space that hasn't been traversed yet, we start a new traversal to determine whether this land mass is an island.

So far, I haven't specified whether we should use a depth-first or a breadth-first search. Unlike most other problems, where there is a clear preference towards one or the other, in this case, both end up equal in both time and space complexity. The example solution relies on a depth-first search.

{caption: "Solution for the counting islands problem."}
```cpp
// depth-first search
bool island(int64_t i, int64_t j, std::vector<std::vector<char>>& grid) {
    // If we managed to reach out of bounds, this is not an island
    if (i == -1 || i == std::ssize(grid) || j == -1 || j == std::ssize(grid[i]))
        return false;
    // If this space is not land, ignore
    if (grid[i][j] != 'L')
        return true;
    // Mark this space as visited
    grid[i][j] = 'V';

    // We can only return true (this is an island) if all four
    // directions of our DFS return true. However, at the same time
    // even if this is not an island we want to explore all spaces
    // of the land mass, just to mark it as visited.
    // If we used a boolean expression, we would run into
    // short-circuiting, the first "false" result would stop 
    // the evaluation.
    // Here we take advantage of the bool->int conversion:
    // false == 0, true == 1
    return (island(i-1,j,grid) + island(i+1,j,grid)
        + island(i,j-1,grid) + island(i,j+1,grid)) == 4;
}

int count_islands(std::vector<std::vector<char>> grid) {
    int cnt = 0;
    // For every space
    for (int64_t i = 0; i < std::ssize(grid); ++i)
        for (int64_t j = 0; j < std::ssize(grid[i]); ++j)
            // If it is an unvisited land space, check if it is an island
            if (grid[i][j] == 'L' && island(i,j,grid))
                ++cnt;
    return cnt;
}
```

<!-- https://compiler-explorer.com/z/57czf9j3G -->

### All valid parentheses sequences

Enumerating all possible combinations under a specific constraint is a canonical problem for backtracking. Our first objective is to formulate our constraints.

The first constraint follows from the input. Because we only have n pairs of parentheses, we can only add n left and n right parentheses.

The second constraint encodes the validity of a parentheses sequence. Adding a left parenthesis will never produce an invalid sequence; however, each right parenthesis must match a previous left parenthesis, meaning we can never have more right parentheses than left parentheses.

Finally, we need to make sure that we keep track of the values required to validate both constraints as we go, to avoid continually recounting the number of parentheses.

{caption: "Solution for the valid parentheses problem."}
```cpp
void generate(std::vector<std::string>& solutions, size_t n,
              std::string& prefix, size_t left, size_t right) {
    // n parentheses, we have solution
    if (prefix.length() == 2*n)
        solutions.push_back(prefix);
    
    // We can only add a left parenthesis if we haven't used all of them.
    if (left < n) {
        prefix.push_back('(');
        // Explore all solutions with this prefix
        generate(solutions, n, prefix, left + 1, right);
        prefix.pop_back();
    }
    // We can only add a left parenthesis if we have used more left
    // than right parentheses.
    if (left > right) {
        prefix.push_back(')');
        // Explore all solutions with this prefix
        generate(solutions, n, prefix, left, right + 1);
        prefix.pop_back();
    }
}

std::vector<std::string> valid_parentheses(size_t n) {
    std::vector<std::string> solutions;
    std::string prefix;
    generate(solutions, n, prefix, 0, 0);
    return solutions;
}
```

### Sudoku solver

One of the requirements for a proper Sudoku puzzle is that it can be solved entirely without guessing simply by propagating the constraints.

However, implementing a non-guessing Sudoku solver is not something you could do within a coding interview; therefore, we will need to limit our scope and do at least some guessing. At the same time, we do not want to completely brute force the puzzle, as that will be pretty slow.

A good middle ground is applying the primary Sudoku constraint: each number can only appear once in each row, column, and box. Consequently, if we are guessing a number for a particular space, we can skip all the numbers already present in that row, column, and box.

![Example of the effect of primary Sudoku constraints. The highlighted cell has only two possible values: six and seven.](traversal/sudoku_constraints.png)

The implementation mirrors the solution for the N-Queens problem with constraint propagation; however, because we are working with a statically sized puzzle (9x9), we can additionally take advantage of the fastest C++ containers: *std::array* and *std::bitset*.

Each Sudoku puzzle has nine rows, nine columns, and nine boxes. Each of which we can represent with a *std::bitset*, where 1s represent digits already present in the corresponding row, column, or box.

{caption: "Solution for the Sudoku solver problem."}
```cpp
/* Calculate the corresponding box for row/col coordinates:
0 1 2
3 4 5
6 7 8

Any mapping will work, as long as it is consistent.
*/
int64_t get_box(int64_t row, int64_t col) {
    return (row/3)*3+col/3;
}

struct State {
    // Initialize the state with given digits
    State(const std::vector<std::vector<char>>& puzzle) {
        for (int64_t i = 0; i < 9; ++i)
        for (int64_t j = 0; j < 9; ++j)
        if (puzzle[i][j] != ' ')
            mark(i, j, puzzle[i][j]-'1');
    }

    std::array<std::bitset<9>,9> row;
    std::array<std::bitset<9>,9> col;
    std::array<std::bitset<9>,9> box;

    // Get the already used digits for a specific space.
    std::bitset<9> used(int64_t r_idx, int64_t c_idx) {
        return row[r_idx] | col[c_idx] | box[get_box(r_idx,c_idx)];
    }
    // Mark this digit as used in the corresponding row, column and box.
    void mark(int64_t r_idx, int64_t c_idx, int64_t digit) {
        row[r_idx][digit] = true;
        col[c_idx][digit] = true;
        box[get_box(r_idx, c_idx)][digit] = true;
    }
    // Mark this digit as unused in the corresponding row, column and box.
    void unmark(int64_t r_idx, int64_t c_idx, int64_t digit) {
        row[r_idx][digit] = false;
        col[c_idx][digit] = false;
        box[get_box(r_idx, c_idx)][digit] = false;
    }
};

// Get the next empty space after {row,col}
std::pair<int64_t,int64_t> next(
    const std::vector<std::vector<char>>& puzzle,
    int64_t row, int64_t col) {
    int64_t start = col;
    for (int64_t i = row; i < std::ssize(puzzle); ++i)
        for (int64_t j = std::exchange(start,0); j < std::ssize(puzzle[i]); ++j)
            if (puzzle[i][j] == ' ')
                return {i,j};
    return {-1,-1};
}

bool backtrack(
    std::vector<std::vector<char>>& puzzle,
    State& state,
    int64_t r_curr, int64_t c_curr) {

    // next coordinate to fill
    auto [r_next, c_next] = next(puzzle, r_curr, c_curr);
    // {-1,-1} means there is no unfilled space, 
    // i.e. we have solved the puzzle
    if (r_next == -1 && c_next == -1)
        return true;

    // The candidate numbers for this space cannot
    // repeat in the row, column or box.
    auto used = state.used(r_next, c_next);

    // Guess a number
    for (int64_t i = 0; i < 9; ++i) {
        // Already in a row, column or box
        if (used[i]) continue;

        // Mark it on the puzzle
        puzzle[r_next][c_next] = '1'+i;
        state.mark(r_next,c_next,i);

        if (backtrack(puzzle,state,r_next,c_next))
            return true;
        // we get false if this was a guess
        // that didn't lead to a solution

        // Unmark from the puzzle
        state.unmark(r_next,c_next,i);
        puzzle[r_next][c_next] = ' ';
        // And try the next digit
    }
    return false;
}

bool solve(std::vector<std::vector<char>>& puzzle) {
    State state(puzzle);
    return backtrack(puzzle,state,0,0);
}
```

<!-- https://compiler-explorer.com/z/arb7G9nGo -->