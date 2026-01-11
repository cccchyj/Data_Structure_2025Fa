#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>

enum Color { RED, BLACK };

struct RBNode {
    std::string key;
    std::string meaning;
    Color color;
    RBNode *left, *right, *parent;
    RBNode(const std::string &k="", const std::string &m="", Color c=RED)
      : key(k), meaning(m), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBTree {
public:
    RBTree();
    ~RBTree();
    void insert(const std::string &key, const std::string &meaning);
    void remove(const std::string &key);
    RBNode* search(const std::string &key) const;
    void range_search(const std::string &low, const std::string &high, std::vector<RBNode*> &out) const;
    void preorder_print(const std::string &outpath) const;

private:
    RBNode *root;
    RBNode *NIL; // sentinel
    // helper methods you must implement:
    void left_rotate(RBNode *x);
    void right_rotate(RBNode *y);
    void insert_fixup(RBNode *z);
    void delete_fixup(RBNode *x);
    RBNode* tree_minimum(RBNode *x) const;
    void transplant(RBNode *u, RBNode *v);
    void destroy(RBNode *x);
    void preorder_rec(RBNode *node, int level, int childIndex, std::ofstream &out) const;
};
