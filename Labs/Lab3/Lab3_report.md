# Lab 3 : Binary Search Trees

**Course:** CS20009.04 Data Structure
**Name:** Yijia Chen
**Student Number:** 24300240127
**Date:** October 21, 2025

<br>

## Introduction

This lab aims to implement the following operations on binary search trees:

- Searching a node with a given key.
- Finding the successor and predecessor of a given node.
- Inorder, preorder, and postorder tree walk.

<br>

## Implementation

### Search

The `searchRecursive` or `searchIterative` procedure follows a simple path from the root `x` downwards. The BST property is used to determine the next step, to go left if `key < x` or go right otherwise. The function terminates when either `x == key` or the path reaches a leaf node.

### Successor and Predecessor

`successor` returns the node with the smallest key greater than `x`'s key if it exists. There are two cases:

- If the right subtree of `x` is not empty, the successor `y` is the smallest element, which is also the leftmost node, in the right subtree.
- If the right subtree of `x` is empty, the successor `y` is "the lowest ancestor of `x` whose left child is also an ancestor of `x`".

`predecessor` returns the node with the largest key less than `x`'s key if it exists. It is symmetric to `successor`.

### Tree Walk

`inorderTreeWalk` prints out all the keys in a BST in inorder, because of the BST property that, the left subtree of a node contains all keys less than its key, and the right subtree contains all keys greater than its key.

The implementation of `inorderTreeWalk`, `preorderTreeWalk` and `postorderTreeWalk` are similar. The only change is the order of visiting the current node and its two subtrees.

<!-- @import "BST.h" -->

<br>

The following code tests the correctness of the above functions.

<!-- @import "test.cpp" -->

<br>

## References

Cormen, T. H., Leiserson, C. E., Rivest, R. L., and Stein, C. (2022).
    *Introduction to Algorithms* (4th ed.). MIT Press.

<br>

## Acknowledgement

ChatGPT-5 provided valuable feedback and suggestions on [the test code implementation](https://chatgpt.com/s/t_68f6d96c61848191b80dc9741361e5ba), [wording and sentence structure](https://chatgpt.com/s/t_68f6da724bb081919be07330e143b3a0).

## Appendix: Lab Specification

Write code for the following methods of binary search trees.
(1) SEARCH(x, k) (recursion and iterative version); (20%)
(2) SUCCESSOR(x) and PREDECESSOR(x); (30%)
(3) INORDER-TREE-WALK(T), PREORDER-TREE-WALK(T), POSTORDER-TREE-WALK(T); (50%)
Note. T points the root of a binary search tree, x points any node in a binary search tress, and k denotes a key.