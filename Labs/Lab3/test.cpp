#include <iostream>
#include "BST.h"
using namespace std;

// Helper function: insert a new node into the BST
template <class T>
Node<T>* insertNode(BST<T>& tree, T key) {
    Node<T>* z = new Node<T>{key, nullptr, nullptr, nullptr};
    Node<T>* y = nullptr;
    Node<T>* x = tree.root;

    while (x != nullptr) {
        y = x;
        if (key < x->key)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;
    if (y == nullptr)
        tree.root = z;
    else if (key < y->key)
        y->left = z;
    else
        y->right = z;

    return z;
}

// Helper to print section headers
void printHeader(const string& title) {
    cout << "\n========== " << title << " ==========\n";
}

int main() {
    BST<int> tree{};
    tree.root = nullptr;

    // Build a tree with various shapes
    //             15
    //           /    \
    //          6      18
    //        /  \    /  \
    //       3    7  17  20
    //      / \    \
    //     2   4    13
    //             /
    //            9
    Node<int>* n15 = insertNode(tree, 15);
    Node<int>* n6  = insertNode(tree, 6);
    Node<int>* n18 = insertNode(tree, 18);
    Node<int>* n3  = insertNode(tree, 3);
    Node<int>* n7  = insertNode(tree, 7);
    Node<int>* n17 = insertNode(tree, 17);
    Node<int>* n20 = insertNode(tree, 20);
    Node<int>* n2  = insertNode(tree, 2);
    Node<int>* n4  = insertNode(tree, 4);
    Node<int>* n13 = insertNode(tree, 13);
    Node<int>* n9  = insertNode(tree, 9);

    // (3) Traversals
    printHeader("Tree Traversals");
    cout << "Inorder:   ";
    tree.inorderTreeWalk(tree.root);
    cout << "\nPreorder:  ";
    tree.preorderTreeWalk(tree.root);
    cout << "\nPostorder: ";
    tree.postorderTreeWalk(tree.root);
    cout << endl;

    // (1) Search tests
    printHeader("Search Tests");
    int keys_to_search[] = {13, 9, 15, 1, 22};
    for (int k : keys_to_search) {
        int r1 = tree.searchRecursive(tree.root, k);
        int r2 = tree.searchIterative(tree.root, k);
        cout << "Key " << k << ": ";
        if (r1 != -1)
            cout << "found (recursive=" << r1 << ", iterative=" << r2 << ")\n";
        else
            cout << "not found\n";
    }

    // (2) Successor / Predecessor tests
    printHeader("Successor / Predecessor Tests");
    auto testSuccPred = [&](Node<int>* node) {
        cout << "Node " << node->key << ": ";
        int succ = tree.successor(node);
        int pred = tree.predecessor(node);
        cout << "successor=" << ((succ == -1) ? string("null") : to_string(succ))
             << ", predecessor=" << ((pred == -1) ? string("null") : to_string(pred)) << "\n";
    };

    testSuccPred(n15); // root
    testSuccPred(n6);  // internal node
    testSuccPred(n2);  // smallest
    testSuccPred(n20); // largest
    testSuccPred(n13); // internal with left child
    testSuccPred(n9);  // leaf

    // Additional trees
    printHeader("Single-node Tree");
    BST<int> single{};
    Node<int>* s1 = insertNode(single, 10);
    single.inorderTreeWalk(single.root);
    cout << "\nSuccessor=" << single.successor(s1)
         << ", Predecessor=" << single.predecessor(s1) << endl;

    printHeader("Left-skewed Tree");
    BST<int> left{};
    Node<int>* a = insertNode(left, 5);
    Node<int>* b = insertNode(left, 4);
    Node<int>* c = insertNode(left, 3);
    Node<int>* d = insertNode(left, 2);
    Node<int>* e = insertNode(left, 1);
    left.inorderTreeWalk(left.root);
    cout << "\nSuccessor(3)=" << left.successor(c)
         << ", Predecessor(3)=" << left.predecessor(c) << endl;

    printHeader("Right-skewed Tree");
    BST<int> right{};
    Node<int>* r1 = insertNode(right, 1);
    Node<int>* r2 = insertNode(right, 2);
    Node<int>* r3 = insertNode(right, 3);
    Node<int>* r4 = insertNode(right, 4);
    Node<int>* r5 = insertNode(right, 5);
    right.inorderTreeWalk(right.root);
    cout << "\nSuccessor(3)=" << right.successor(r3)
         << ", Predecessor(3)=" << right.predecessor(r3) << endl;

    cout << "\nAll tests completed.\n";
    return 0;
}
