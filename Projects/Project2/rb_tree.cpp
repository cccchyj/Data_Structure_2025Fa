#include "rb_tree.h"
#include <fstream>
#include <iostream>
#include <functional>

RBTree::RBTree() {
    NIL = new RBNode();
    NIL->color = BLACK;
    NIL->left = NIL->right = NIL->parent = NIL;
    root = NIL;
}
RBTree::~RBTree() {
    destroy(root);
    delete NIL;
}

void RBTree::destroy(RBNode *x) {
    if (x == NIL) return;
    destroy(x->left);
    destroy(x->right);
    delete x;
}

// ---------- Rotations ----------
void RBTree::left_rotate(RBNode *x) {
    RBNode *y = x->right;      // y is x's right child
    x->right = y->left;        // turn y’s left subtree into x’s right subtree

    // If y->left is a real node (not NIL), update its parent
    if (y->left != NIL)
        y->left->parent = x;

    // Link y's parent to x's parent
    y->parent = x->parent;

    // If x was the root, now y becomes new root
    if (x->parent == NIL)
        root = y;
    // Otherwise reconnect x's parent to y
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;    // make x become y’s left child
    x->parent = y;
}

void RBTree::right_rotate(RBNode *y) {
    RBNode *x = y->left;
    y->left = x->right;
    if (x->right != NIL)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NIL)
        root = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left = x;
    x->right = y;
    y->parent = x;
}

// ---------- Insert ----------
void RBTree::insert(const std::string &key, const std::string &meaning) {
    // Create new red node
    RBNode *z = new RBNode(key, meaning, RED);
    z->left = z->right = z->parent = NIL;

    RBNode *x = root;   // node being compared with z
    RBNode *y = NIL;    // y will be parent of z

    // Standard BST insertion
    while (x != NIL) {
        y = x;
        if (z->key < x->key)
            x = x->left;
        else if (z->key > x->key)
            x = x->right;
        else {  // Key already exists — update meaning
            x->meaning = meaning;
            delete z;
            return;
        }
    }

    z->parent = y;      // found the location, insert z with parent y
    if (y == NIL)
        root = z;       // tree T was empty, first insertion
    else if (z->key < y->key)
        y->left = z;
    else
        y->right = z;

    insert_fixup(z);
}

void RBTree::insert_fixup(RBNode *z) {
    while (z->parent->color == RED) {
        // Parent is left child of grandparent
        if (z->parent == z->parent->parent->left) {
            RBNode *y = z->parent->parent->right;   // y is z's uncle

            if (y->color == RED) {
                // Case 1: parent & uncle red, recolor and move up
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;              // move up the tree
            } else { // Uncle black
                if (z == z->parent->right) {
                    // Case 2: triangle, rotate to turn into line
                    z = z->parent;
                    left_rotate(z);
                }
                // Case 3: line, rotate and recolor
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(z->parent->parent);
            }
        }

        // Parent is right child (mirror of above)
        else {
            RBNode *y = z->parent->parent->left;

            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(z->parent->parent);
            }
        }
    }

    root->color = BLACK;  // root must always be black
}

// ---------- Delete ----------
void RBTree::remove(const std::string &key) {
    RBNode *z = search(key);
    if (z == NIL) return;   // not found

    RBNode *y = z;      // node actually removed
    RBNode *x;          // subtree moved into y's old place
    Color y_orig_color = y->color;

    // Case 1: One or zero children
    if (z->left == NIL) {
        x = z->right;
        transplant(z, z->right);    // replace z by its right child
    }
    else if (z->right == NIL) {
        x = z->left;
        transplant(z, z->left);     // replace z by its left child
    }

    // Case 2: Two children
    else {
        y = tree_minimum(z->right);     // y is z's successor
        y_orig_color = y->color;
        x = y->right;

        if (y->parent == z) {           // y is z's child
            if (x != NIL) x->parent = y;
        } else {                        // y is farther down the tree
            transplant(y, y->right);    // replace y by its right child
            y->right = z->right;        // z’s right child becomes y’s right child
            if (y->right != NIL) y->right->parent = y;
        }

        transplant(z, y);   // replace z by its successor y
        y->left = z->left;  // give z’s left child to y, which had no left child
        if (y->left != NIL) y->left->parent = y;
        y->color = z->color;  // preserve black-height
    }

    // If we removed a black node, we must rebalance
    if (y_orig_color == BLACK)
        delete_fixup(x);
    
    // z has been detached from the tree
    delete z;
}

void RBTree::transplant(RBNode *u, RBNode *v) {
    if (u->parent == NIL)
        root = v;                       // u was root
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    if (v != NIL) v->parent = u->parent;
}

RBNode* RBTree::tree_minimum(RBNode *x) const {
    while (x->left != NIL)
        x = x->left;
    return x;
}

void RBTree::delete_fixup(RBNode *x) {
    while (x != root && x->color == BLACK) {

        // x is left child
        if (x == x->parent->left) {
            RBNode *w = x->parent->right;   // sibling

            // Case 1: sibling red
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(x->parent);
                w = x->parent->right;
            }

            // Case 2: sibling black, both children black
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }

            // Case 3: sibling black, right child black, left child red
            else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(w);
                    w = x->parent->right;
                }

                // Case 4: sibling black, right child red
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate(x->parent);
                x = root;
            }
        }

        // Mirror: x is right child
        else {
            RBNode *w = x->parent->left;

            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(x->parent);
                w = x->parent->left;
            }

            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate(x->parent);
                x = root;
            }
        }
    }

    x->color = BLACK;   // ensure x is black
}

// ---------- Search ----------
RBNode* RBTree::search(const std::string &key) const {
    RBNode *cur = root;
    while (cur != NIL) {
        if (key == cur->key) return cur;
        cur = (key < cur->key) ? cur->left : cur->right;
    }
    return nullptr;
}

// ---------- Range Search ----------
void RBTree::range_search(const std::string &low, const std::string &high, std::vector<RBNode*> &out) const {
    std::function<void(RBNode*)> dfs = [&](RBNode* node) {
        if (node == NIL) return;
        if (node->key > low) dfs(node->left);
        if (node->key >= low && node->key <= high) out.push_back(node);
        if (node->key < high) dfs(node->right);
    };
    dfs(root);
}

// ---------- Preorder Print ----------
void RBTree::preorder_print(const std::string &outpath) const {
    std::ofstream out(outpath);
    if (!out) {
        std::cerr << "Cannot open " << outpath << "\n";
        return;
    }
    preorder_rec(root, 0, 0, out);
}

void RBTree::preorder_rec(RBNode *node, int level, int childIndex, std::ofstream &out) const {
    if (node == NIL) {
        out << "level=" << level << " child=" << childIndex << " null\n";
        return;
    }
    out << "level=" << level << " child=" << childIndex << " "
        << node->key << "(" << (node->color == RED ? "RED" : "BLACK") << ")\n";
    preorder_rec(node->left, level + 1, 0, out);
    preorder_rec(node->right, level + 1, 1, out);
}
