#pragma once
#include <iostream>

template <class T>
struct Node {
    T key;
    Node<T> *parent, *left, *right;
};

template <class T>
struct BST {
    Node<T> * root;

    // (1)

    T searchRecursive(Node<T>* x, T k) {                 // recursion version
        if (x == nullptr) return -1;
        else if (x->key == k) return x->key;
        else if (x->key > k) return searchRecursive(x->left, k);
        else return searchRecursive(x->right, k);
    }

    T searchIterative(Node<T>* x, T k) {       // iterative version
        while (x != nullptr && k != x->key) {
            if (k < x->key) x = x->left;
            else x = x->right;
        }
        return (x == nullptr) ? -1 : x->key;
    }

    // (2)

    T minTree(Node<T>* x) {
        while (x->left != nullptr) x = x->left;
        return x->key;
    }
    T successor(Node<T>* x) {
        if (x->right != nullptr) {
            return minTree(x->right);
        } else {
            Node<T>* y = x->parent;
            while (y != nullptr && x == y->right) {
                x = y;
                y = y->parent;
            }
            return (y == nullptr) ? -1 : y->key;
        }
    }

    T maxTree(Node<T>* x) {
        while (x->right != nullptr) x = x->right;
        return x->key;
    }
    T predecessor(Node<T>* x) {
        if (x->left != nullptr) {
            return maxTree(x->left);
        } else {
            Node<T>* y = x->parent;
            while (y != nullptr && x == y->left) {
                x = y;
                y = y->parent;
            }
            return (y == nullptr) ? -1 : y->key;
        }
    }

    // (3)

    void inorderTreeWalk(Node<T>* x) {
        if (x != nullptr) {
            inorderTreeWalk(x->left);
            std::cout << x->key << " ";
            inorderTreeWalk(x->right);
        }
    }

    void preorderTreeWalk(Node<T>* x) {
        if (x != nullptr) {
            std::cout << x->key << " ";
            preorderTreeWalk(x->left);
            preorderTreeWalk(x->right);
        }
    }

    void postorderTreeWalk(Node<T>* x) {
        if (x != nullptr) {
            postorderTreeWalk(x->left);
            postorderTreeWalk(x->right);
            std::cout << x->key << " ";
        }
    }
};

