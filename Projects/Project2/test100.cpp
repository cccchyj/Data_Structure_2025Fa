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

// ----------- Trim helper -----------
static inline string trim(const string &s) {
    size_t start = s.find_first_not_of(" \r\n\t");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \r\n\t");
    return s.substr(start, end - start + 1);
}

// ----------- File loading -----------
struct Entry { string key; string meaning; };

vector<Entry> load_file(const string &path, string &operation) {
    vector<Entry> data;
    ifstream fin(path);
    if (!fin.is_open()) {
        cerr << "Cannot open " << path << endl;
        return data;
    }

    string line;
    getline(fin, line);
    operation = trim(line);

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

// ----------- Batch processing with record-every-10 -----------
template <typename TreeType>
double run_batch(TreeType &tree, const vector<Entry> &data,
                 const string &operation, int run_id,
                 const string &tree_name, const string &step_name,
                 ofstream &csv)
{
    auto start = high_resolution_clock::now();
    int counter = 0;

    for (size_t i = 0; i < data.size(); i++) {
        if (operation == "INSERT")
            tree.insert(data[i].key, data[i].meaning);
        else if (operation == "DELETE")
            tree.remove(data[i].key);

        counter++;

        if (counter % 10 == 0 || i == data.size() - 1) {
            auto now = high_resolution_clock::now();
            double elapsed = duration<double, milli>(now - start).count();

            csv << run_id << ","
                << tree_name << ","
                << step_name << ","
                << counter << ","
                << fixed << setprecision(4) << elapsed << "\n";
        }
    }

    auto end = high_resolution_clock::now();
    return duration<double, milli>(end - start).count();
}

// ============================================================================
//                                  main()
// ============================================================================

int main() {
    // Load files
    string op_init, op_delete, op_insert;
    auto init_data = load_file("init.txt", op_init);
    auto del_data  = load_file("delete.txt", op_delete);
    auto ins_data  = load_file("insert.txt", op_insert);

    // CSV log
    ofstream csv("time_log.csv");
    csv << "run,tree,step,ops_completed,time_ms\n";

    cout << "Running benchmark for 100 runs...\n";

    for (int run = 1; run <= 100; run++) {
        cout << "Run " << run << "...\n";

        // ----------- AVL -----------
        AVLTree avl;
        run_batch(avl, init_data, op_init, run, "AVL", "init",   csv);
        run_batch(avl, del_data,  op_delete, run, "AVL", "delete", csv);
        run_batch(avl, ins_data,  op_insert, run, "AVL", "insert", csv);

        // ----------- RB -----------
        RBTree rb;
        run_batch(rb, init_data, op_init, run, "RB", "init",   csv);
        run_batch(rb, del_data,  op_delete, run, "RB", "delete", csv);
        run_batch(rb, ins_data,  op_insert, run, "RB", "insert", csv);
    }

    csv.close();
    cout << "Benchmark finished. Results saved to time_log.csv.\n";
    return 0;
}
