#include "avl_tree.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <functional>

// Return the larger of two integers
static inline int maxint(int a, int b) { return a > b ? a : b; }

// Constructor: Initialize root node to null
AVLTree::AVLTree() : root(nullptr) {}
// Destructor: Destroy the entire tree
AVLTree::~AVLTree() { destroy(root); }

// Recursively destroy subtree rooted at x
void AVLTree::destroy(AVLNode *x) {
    if (!x) return;
    destroy(x->left);
    destroy(x->right);
    delete x;
}

// Get the height of a node, return 0 if node is null
int AVLTree::height(AVLNode *n) const { return n ? n->height : 0; }

// Calculate balance factor (left subtree height - right subtree height)
int AVLTree::balance_factor(AVLNode *n) const {
    return n ? height(n->left) - height(n->right) : 0;
}

// ---------- Rotations ----------
// Right rotation operation: used to handle left-left case
AVLNode* AVLTree::rotate_right(AVLNode* y) {
    AVLNode* x = y->left;       // x becomes new root of this subtree
    AVLNode* T2 = x->right;     // T2 is x's right subtree. It will become y->left.

    x->right = y;   // y moves down to the right child of x
    y->left = T2;   // T2 is now the left child of y

    y->height = 1 + maxint(height(y->left), height(y->right));
    x->height = 1 + maxint(height(x->left), height(x->right));

    return x;
}

// Left rotation operation: used to handle right-right case
AVLNode* AVLTree::rotate_left(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = maxint(height(x->left), height(x->right)) + 1;
    y->height = maxint(height(y->left), height(y->right)) + 1;
    return y;
}

// ---------- Insert ----------
// Insert key-value pair into AVL tree
void AVLTree::insert(const std::string &key, const std::string &meaning) {
    root = insert_rec(root, key, meaning);
}

// Recursive node insertion
AVLNode* AVLTree::insert_rec(AVLNode* node, const std::string &k, const std::string &m) {
    // Standard BST insert
    // If current node is null, create new node
    if (!node) return new AVLNode(k, m);

    // Decide whether to insert into left or right subtree based on key value
    if (k < node->key)
        node->left = insert_rec(node->left, k, m);
    else if (k > node->key)
        node->right = insert_rec(node->right, k, m);
    else {
        node->meaning = m;  // Update existing key value
        return node;
    }

    // Update node height
    node->height = 1 + maxint(height(node->left), height(node->right));
    // Calculate balance factor
    int bf = balance_factor(node);

    // Perform rotations based on different imbalance situations
    // Left Left case
    if (bf > 1 && k < node->left->key)
        return rotate_right(node);
    // Right Right case
    if (bf < -1 && k > node->right->key)
        return rotate_left(node);
    // Left Right case
    if (bf > 1 && k > node->left->key) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    // Right Left case
    if (bf < -1 && k < node->right->key) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    return node;            // new root of the subtree
}

// ---------- Delete ----------
// Delete specified key value from AVL tree
void AVLTree::remove(const std::string &key) {
    root = remove_rec(root, key);
}

// Find the minimum node in subtree rooted at node
AVLNode* AVLTree::min_node(AVLNode* node) const {
    AVLNode* cur = node;
    while (cur && cur->left)
        cur = cur->left;
    return cur;
}

// Recursive node deletion
AVLNode* AVLTree::remove_rec(AVLNode* node, const std::string &k) {
    // If node is null, return directly
    if (!node) return node;

    // Standard BST search
    if (k < node->key)
        node->left = remove_rec(node->left, k);
    else if (k > node->key)
        node->right = remove_rec(node->right, k);
    else {
        // Found node to delete
        if (!node->left || !node->right) {  // Node has at most one child
            AVLNode* child = node->left ? node->left : node->right;
            if (!child) {    // No children, leaf
                delete node;
                return nullptr;
            } else {        // One child, replace node by child
                AVLNode* old = node;
                node = child;     // promote child
                delete old;       // remove old node
            }
        } else {            // Node has two children
            AVLNode* succ = min_node(node->right);
            node->key = succ->key;
            node->meaning = succ->meaning;
            node->right = remove_rec(node->right, succ->key);
        }
    }

    // Update node height
    node->height = 1 + maxint(height(node->left), height(node->right));
    // Calculate balance factor
    int bf = balance_factor(node);

    // Perform rotations based on different imbalance situations
    if (bf > 1 && balance_factor(node->left) >= 0)
        return rotate_right(node);
    if (bf > 1 && balance_factor(node->left) < 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (bf < -1 && balance_factor(node->right) <= 0)
        return rotate_left(node);
    if (bf < -1 && balance_factor(node->right) > 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;            // new root of the subtree
}

// ---------- Search ----------
// Search for node with specified key value in AVL tree
AVLNode* AVLTree::search(const std::string &key) const {
    AVLNode* cur = root;
    while (cur) {
        if (key == cur->key) return cur;
        cur = (key < cur->key) ? cur->left : cur->right;
    }
    return nullptr;
}

// ---------- Range Search ----------
// Find all nodes within specified range
void AVLTree::range_search(const std::string &low, const std::string &high, std::vector<AVLNode*> &out) const {
    // Use lambda expression to implement depth-first search
    std::function<void(AVLNode*)> dfs = [&](AVLNode* node) {
        if (!node) return;
        if (node->key > low) dfs(node->left);
        if (node->key >= low && node->key <= high) out.push_back(node);
        if (node->key < high) dfs(node->right);
    };
    dfs(root);
}

// ---------- Preorder Print ----------
// Preorder traversal print AVL tree structure to file
void AVLTree::preorder_print(const std::string &outpath) const {
    std::ofstream out(outpath);
    if (!out) {
        std::cerr << "Cannot open " << outpath << "\n";
        return;
    }
    preorder_rec(root, 0, 0, out);
}

// Recursive preorder traversal print node information
void AVLTree::preorder_rec(AVLNode *node, int level, int childIndex, std::ofstream &out) const {
    if (!node) {
        out << "level=" << level << " child=" << childIndex << " null\n";
        return;
    }
    out << "level=" << level << " child=" << childIndex << " "
        << node->key << "(h=" << node->height << ")\n";
    preorder_rec(node->left, level + 1, 0, out);
    preorder_rec(node->right, level + 1, 1, out);
}