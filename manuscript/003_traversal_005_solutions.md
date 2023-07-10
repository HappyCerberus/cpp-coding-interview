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