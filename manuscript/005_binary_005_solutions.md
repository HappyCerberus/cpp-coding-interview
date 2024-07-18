{full: true, community: true}
## Solutions

### Sonar

Let's start with a simplified version of the problem. What if we were looking for a single ship on a 1D map?

Consider that when we binary search in an array, we use a predicate to decide which half of the range we should recurse in. Instead of checking against a value, we can reformulate the question to "Is the value in the left half of the range?" If we get true, we know we need to recurse in the left side of the range; if we get false, we recurse in the right side of the range.

{width: "40%"}
![Example of a binary search using a "presence" predicate when searching for value 5 (pivots underlined).](binary_search/sonar_binary_search.png)

This means we can use the sonar ping function in place of a predicate. However, we still need to deal with the fact that we are looking for potentially many ships on a 2D map.

The many ships situation means we can potentially get true for both the left and right half of the range, so instead of recursing into one half, we may need to recurse into both the left and right halves.

The 2D map increases the dimensionality of our binary search. Instead of dividing a range into two parts, a pivot now divides the map into four quadrants.

Putting this together we end up with recursive quad-search:

{caption: "Solution for the Sonar problem."}
```cpp
size_t count_ships(const Sonar& sonar, Area area) {
    // for non-square area we can end up overflowing
    // the limit in one of the dimensions
    if (area.bottom > area.top) return 0;
    if (area.left > area.right) return 0;

    // nothing in the area
    if (!sonar.ping(area)) return 0;

    // there is something and this is area of size one
    if (area.bottom == area.top && area.left == area.right)
        return 1;
    
    // otherwise calculate the four quadrants and recurse
    size_t mid_x = std::midpoint(area.left,area.right);
    size_t mid_y = std::midpoint(area.bottom,area.top);

    size_t count = 0;
    count += count_ships(sonar, 
        {.bottom = area.bottom, .top = mid_y, 
        .left = area.left, .right = mid_x});
    count += count_ships(sonar,
        {.bottom = mid_y + 1, .top = area.top, 
        .left = area.left, .right = mid_x});
    count += count_ships(sonar,
        {.bottom = area.bottom, .top = mid_y, 
        .left = mid_x+1, .right = area.right});
    count += count_ships(sonar,
        {.bottom = mid_y + 1, .top = area.top, 
        .left = mid_x+1, .right = area.right});
    return count;
}
```