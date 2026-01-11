#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;

struct Job {
    int id;
    int processing_time;
    int profit;
    int deadline;
};

pair<int, vector<Job>> findMaxProfitSchedule(vector<Job>& jobs) {
    if (jobs.empty()) return {0, {}};
    
    // Sort jobs by deadline
    sort(jobs.begin(), jobs.end(), 
         [](const Job& a, const Job& b) { return a.deadline < b.deadline; });
    
    int n = jobs.size();
    int max_deadline = 0;
    for (const auto& job : jobs) {
        max_deadline = max(max_deadline, job.deadline);
    }
    
    // DP arrays
    vector<int> dp(max_deadline + 1, 0);
    vector<vector<pair<int, int>>> decision(n, vector<pair<int, int>>(max_deadline + 1, {-1, -1}));
    
    // Dynamic programming
    for (int i = 0; i < n; i++) {
        int t_j = jobs[i].processing_time;
        int p_j = jobs[i].profit;
        int d_j = jobs[i].deadline;
        
        for (int t = d_j; t >= t_j; t--) {
            if (dp[t] < dp[t - t_j] + p_j) {
                dp[t] = dp[t - t_j] + p_j;
                decision[i][t] = {t - t_j, i};
            }
        }
    }
    
    // Find maximum profit
    int max_profit = 0;
    int max_time = 0;
    for (int t = 0; t <= max_deadline; t++) {
        if (dp[t] > max_profit) {
            max_profit = dp[t];
            max_time = t;
        }
    }
    
    // Reconstruct schedule
    vector<Job> schedule;
    int current_time = max_time;
    for (int i = n - 1; i >= 0; i--) {
        if (decision[i][current_time].first != -1) {
            schedule.push_back(jobs[i]);
            current_time = decision[i][current_time].first;
        }
    }
    reverse(schedule.begin(), schedule.end());
    
    return {max_profit, schedule};
}

// Parse input string into jobs
vector<Job> parseInput(const string& input) {
    vector<Job> jobs;
    stringstream ss(input);
    string line;
    int job_id = 1;
    
    while (getline(ss, line)) {
        if (line.empty()) continue;
        
        stringstream line_ss(line);
        int t, p, d;
        if (line_ss >> t >> p >> d) {
            jobs.push_back({job_id++, t, p, d});
        }
    }
    
    return jobs;
}

// Print the schedule
void printSchedule(int profit, const vector<Job>& schedule) {
    cout << "\n=== RESULTS ===" << endl;
    cout << "Maximum Profit: " << profit << endl;
    cout << "Jobs to Execute: " << schedule.size() << endl;
    cout << "Schedule:" << endl;
    
    int current_time = 0;
    int total_profit = 0;
    
    for (const auto& job : schedule) {
        int start_time = current_time;
        int end_time = current_time + job.processing_time;
        bool on_time = (end_time <= job.deadline);
        int job_profit = on_time ? job.profit : 0;
        total_profit += job_profit;
        
        cout << "  Job" << job.id << " (t=" << job.processing_time 
             << ", p=" << job.profit << ", d=" << job.deadline << ")";
        cout << " -> Time " << start_time << "-" << end_time;
        cout << " | Profit: " << job_profit;
        cout << " | " << (on_time ? "ON TIME" : "LATE") << endl;
        
        current_time = end_time;
    }
    cout << "Total Profit: " << total_profit << endl;
    cout << "===============\n" << endl;
}

int main() {
    cout << "=== Job Scheduling Algorithm ===" << endl;
    cout << "Input format: Each line = processing_time profit deadline" << endl;
    cout << "Example: " << endl;
    cout << "2 100 2" << endl;
    cout << "1 50 1" << endl;
    cout << "2 150 3" << endl;
    cout << "Enter 'end' on a new line to process, 'quit' to exit\n" << endl;
    
    while (true) {
        cout << "Enter jobs (one per line):" << endl;
        
        vector<string> input_lines;
        string line;
        int job_count = 0;
        
        // Read input until "end" or EOF
        while (getline(cin, line)) {
            if (line == "quit") {
                cout << "Goodbye!" << endl;
                return 0;
            }
            if (line == "end") {
                break;
            }
            if (!line.empty()) {
                input_lines.push_back(line);
                job_count++;
            }
        }
        
        if (input_lines.empty()) {
            cout << "No jobs entered. Try again.\n" << endl;
            continue;
        }
        
        // Combine all input lines
        string combined_input;
        for (const auto& input_line : input_lines) {
            combined_input += input_line + "\n";
        }
        
        // Parse and process
        vector<Job> jobs = parseInput(combined_input);
        
        cout << "\nProcessing " << jobs.size() << " jobs..." << endl;
        cout << "Jobs entered:" << endl;
        for (const auto& job : jobs) {
            cout << "  Job" << job.id << ": t=" << job.processing_time 
                 << ", p=" << job.profit << ", d=" << job.deadline << endl;
        }
        
        auto [profit, schedule] = findMaxProfitSchedule(jobs);
        printSchedule(profit, schedule);
        
        cout << "Ready for next input...\n" << endl;
    }
}