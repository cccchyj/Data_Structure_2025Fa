#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>
#include <iomanip>
#include "avl_tree.h"
#include "rb_tree.h"

using namespace std;
using namespace std::chrono;

static inline string trim(const string &s) {
    size_t start = s.find_first_not_of(" \r\n\t");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \r\n\t");
    return s.substr(start, end - start + 1);
}

struct Entry { string key; string meaning; };

vector<Entry> load_file(const string &path, string &operation) {
    vector<Entry> data;
    ifstream fin(path);
    if (!fin.is_open()) {
        cerr << "Cannot open " << path << endl;
        return data;
    }
    string line;
    if (!getline(fin, line)) return data;
    operation = trim(line);             // first line is DELETE / INSERT

    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;
        stringstream ss(line);
        string key, meaning;
        ss >> key;
        getline(ss, meaning);
        meaning = trim(meaning);
        data.push_back({key, meaning});
    }
    return data;
}

template <typename TreeType>
void perform_batch(TreeType &tree, const vector<Entry> &data, const string &operation,
                   const string &tree_name, const string &step_name,
                   const string &csv_path, const string &print_prefix) {
    ofstream csv(csv_path, ios::app);
    if (!csv.is_open()) {
        cerr << "Cannot open " << csv_path << endl;
        return;
    }

    auto start_all = high_resolution_clock::now();

    for (size_t i = 0; i < data.size(); ++i) {
        if (operation == "INSERT")
            tree.insert(data[i].key, data[i].meaning);
        else if (operation == "DELETE")
            tree.remove(data[i].key);

        if ((i + 1) % 100 == 0) {
            auto now = high_resolution_clock::now();
            double elapsed = duration<double, milli>(now - start_all).count();
            csv << step_name << "," << tree_name << "," << (i + 1)
                << "," << fixed << setprecision(3) << elapsed << "\n";
        }
    }

    string outfile = print_prefix + "_" + step_name + ".txt";
    tree.preorder_print(outfile);
    cout << tree_name << " tree after " << step_name
         << " saved to " << outfile << endl;
}

template <typename TreeType, typename NodeType>
void query(TreeType &tree) {
    cout << "\n=== Query Mode ===\n";
    cout << "Enter 1 to search single word, 2 for range, 0 to exit.\n";

    while (true) {
        int choice;
        cout << "Choice: ";
        if (!(cin >> choice)) break;
        if (choice == 0) break;
        if (choice == 1) {
            string word;
            cout << "Enter word: ";
            cin >> word;
            NodeType* res = tree.search(word);
            if (!res) cout << "Not found.\n";
            else cout << res->key << " : " << res->meaning << endl;
        } else if (choice == 2) {
            string low, high;
            cout << "Enter range low high: ";
            cin >> low >> high;
            vector<NodeType*> results;
            tree.range_search(low, high, results);
            cout << "Results (" << results.size() << "):\n";
            for (auto *n : results)
                cout << n->key << " : " << n->meaning << "\n";
        }
    }
}

int main() {
    AVLTree avl;
    RBTree rb;

    // 初始化时间记录文件
    ofstream csv("time_log.csv");
    csv << "step,tree,operations,time_ms\n";
    csv.close();

    // 1️ init
    string op1;
    auto init_data = load_file("init.txt", op1);
    perform_batch(avl, init_data, op1, "AVL", "init", "time_log.csv", "avlt");
    perform_batch(rb, init_data, op1, "RB", "init", "time_log.csv", "rbt");

    // 2️ delete
    string op2;
    auto del_data = load_file("delete.txt", op2);
    perform_batch(avl, del_data, op2, "AVL", "delete", "time_log.csv", "avlt");
    perform_batch(rb, del_data, op2, "RB", "delete", "time_log.csv", "rbt");

    // 3️ insert
    string op3;
    auto ins_data = load_file("insert.txt", op3);
    perform_batch(avl, ins_data, op3, "AVL", "insert", "time_log.csv", "avlt");
    perform_batch(rb, ins_data, op3, "RB", "insert", "time_log.csv", "rbt");

    // 4️/5️ query
    cout << "\nNow you can query either tree.\n";
    cout << "Enter which tree to query (avl/rb): ";
    string which;
    cin >> which;
    if (which == "avl")
        query<AVLTree, AVLNode>(avl);
    else
        query<RBTree, RBNode>(rb);

    cout << "\nAll operations finished.\n";
    cout << "Timing log saved to time_log.csv\n";
    cout << "Tree states saved to avlt_init.txt, avlt_delete.txt, avlt_insert.txt, etc.\n";
    return 0;
}
