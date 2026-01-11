#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct AVLNode {
    std::string key;
    std::string meaning;
    int height;
    AVLNode *left, *right;
    AVLNode(const std::string &k="", const std::string &m="") : key(k), meaning(m), height(1), left(nullptr), right(nullptr) {}
};

class AVLTree {
public:
    AVLTree();
    ~AVLTree();
    void insert(const std::string &key, const std::string &meaning);
    void remove(const std::string &key);
    AVLNode* search(const std::string &key) const;
    void range_search(const std::string &low, const std::string &high, std::vector<AVLNode*> &out) const;
    void preorder_print(const std::string &outpath) const;

private:
    AVLNode *root;
    void destroy(AVLNode *x);
    AVLNode* insert_rec(AVLNode* node, const std::string &k, const std::string &m);
    AVLNode* remove_rec(AVLNode* node, const std::string &k);
    AVLNode* rotate_right(AVLNode* y);
    AVLNode* rotate_left(AVLNode* x);
    int height(AVLNode* n) const;
    int balance_factor(AVLNode* n) const;
    void preorder_rec(AVLNode *node, int level, int childIndex, std::ofstream &out) const;
    AVLNode* min_node(AVLNode* node) const;
};
