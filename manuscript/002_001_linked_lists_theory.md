{class: part}
# Linked Lists

While rare in practical applications, linked lists crop up frequently in interviews. Partly this is because the node structure lends itself to formulating tricky problems, similar to trees and graphs, without the added topological complexity.

## `std::list` and `std::forward_list`

The standard library offers two list types, std::list - a doubly-linked list and std::forward_list - a singly-linked list.

## Custom lists

When implementing a custom linked list, you might be tempted to implement a singly-linked list like this:

Sadly, this approach isn't quite correct. The approach exhibits a design problem; we mix resource management with structural information. In this case, this problem manifests during destruction. Since we have tied the ownership to the list structure, the destruction will be recursive, potentially running out of stack space and leading to a crash.

In the case of a linked list, we don't have the option of working around manual resource management as long as we want to maintain O(1) complexity for splicing and iterator and reference stability.

We can replicate the directional structure with reference stability without O(1) splicing operations using a std::vector of std::unique_ptr.