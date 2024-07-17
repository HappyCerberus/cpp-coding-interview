{full: true, community: true}
## Canonical problems

The typical interview questions that you will encounter will have you exercise the standard binary search algorithms we discussed in the theory section.

Instead of rehashing the basics, the following problems explore the more niche uses of binary search.

### Sonar

You are given a rectangular map and access to a sonar system with a simple "ping" method.

Determine the number of ships on the map as quickly as possible. The ping method takes a rectangular area as an argument and returns presence information (whether any ships are in the given area).

{width: "40%"}
![Example of four sonar queries. Queries returning true in green, queries returning false in red.](binary_search/sonar.png)

The area specified for both the input and the ping method is border-inclusive.

{class: information}
B> The scaffolding for this problem is located at `binary_search/sonar`. Your goal is to make the following commands pass without any errors: `bazel test //binary_search/sonar/...`, `bazel test --config=addrsan //binary_search/sonar/...`, `bazel test --config=ubsan //binary_search/sonar/...`.