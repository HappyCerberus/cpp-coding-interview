{full: true, community: true}
# Traversal algorithms

In this chapter, we will discuss the three foundational algorithms we will keep revisiting throughout the rest of the book.

Let's start with a problem: imagine you need to find a path in a maze. How would you do it?

![Maze with one entrance and one exit.](traversal/maze_base.png)

Your first intuition might follow the depth-first search algorithm, opportunistically exploring until you reach a dead-end, then returning to the last crossroads and taking a different path.

## Depth-first search

The depth-first search opportunistically picks a direction at each space and explores that direction fully before returning to this space and picking a different path. A typical approach would use a consistent strategy for picking the direction order: e.g., north, south, west, east.

![Depth-first search using the NSWE strategy.](traversal/maze_dfs.png)