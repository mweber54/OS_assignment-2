// banker.cpp
// Bankers Algorithm
// compile: g++ -std=c++17 -Wall -Wextra -O2 banker.cpp -o banker

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// parse a line of ints (ignores #)
bool parse_int_line(std::istream &in, vector<int> &out, int expected = -1) {
    out.clear();
    string line;
    while (std::getline(in, line)) {
        // strip comments
        auto pos = line.find('#');
        if (pos != string::npos) line = line.substr(0, pos);

        std::stringstream ss(line);
        int x;
        while (ss >> x) {
            out.push_back(x);
        }
        if (!out.empty()) break; // non-empty line
        if (in.eof()) break;
    }
    if (out.empty()) return false;
    if (expected != -1 && (int)out.size() != expected) {
        cout << "Error: expected " << expected
             << " integers on a line, got " << out.size() << endl;
        return false;
    }
    return true;
}

// true if safe and fills safeSequence
bool check_safety(const vector<vector<int>> &maxm,
                  const vector<vector<int>> &alloc,
                  const vector<int> &avail,
                  vector<int> &safeSequence) {
    int n = (int)alloc.size();    // processes
    int m = (int)avail.size();    // resource types

    vector<vector<int>> need(n, vector<int>(m, 0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            need[i][j] = maxm[i][j] - alloc[i][j];

    vector<int> work = avail;
    vector<bool> finish(n, false);
    safeSequence.clear();

    while (true) {
        bool progress = false;
        for (int i = 0; i < n; ++i) {
            if (finish[i]) continue;
            bool canFinish = true;
            for (int j = 0; j < m; ++j) {
                if (need[i][j] > work[j]) {
                    canFinish = false;
                    break;
                }
            }
            if (canFinish) {
                // pretend P_i finishes and releases its resources
                for (int j = 0; j < m; ++j) {
                    work[j] += alloc[i][j];
                }
                finish[i] = true;
                safeSequence.push_back(i);
                progress = true;
            }
        }
        if (!progress) break; // no more processes can finish
    }

    // check if all finished
    for (bool f : finish) {
        if (!f) return false;
    }
    return true;
}

//process a single resource request
bool handle_request(vector<vector<int>> &maxm,
                    vector<vector<int>> &alloc,
                    vector<int> &avail) {
    int n = (int)alloc.size();
    int m = (int)avail.size();

    cout << "\nDo you want to test a resource request? (y/n): ";
    char ans;
    if (!(cin >> ans) || (ans != 'y' && ans != 'Y')) return false;

    cout << "Enter process id (0.." << n - 1 << "): ";
    int p;
    cin >> p;
    if (p < 0 || p >= n) {
        cout << "Invalid process id.\n";
        return false;
    }

    cout << "Enter request vector of size " << m << ":\n";
    vector<int> req(m);
    for (int j = 0; j < m; ++j) cin >> req[j];

    vector<vector<int>> need(n, vector<int>(m, 0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            need[i][j] = maxm[i][j] - alloc[i][j];

    // Check Request <= Need
    for (int j = 0; j < m; ++j) {
        if (req[j] > need[p][j]) {
            cout << "Error: process " << p
                 << " has requested more than its remaining need.\n";
            return false;
        }
    }

    // check request <= available
    for (int j = 0; j < m; ++j) {
        if (req[j] > avail[j]) {
            cout << "Resources not available; request must wait.\n";
            return false;
        }
    }

    // allocate
    for (int j = 0; j < m; ++j) {
        avail[j] -= req[j];
        alloc[p][j] += req[j];
        need[p][j] -= req[j];
    }

    vector<int> safeSeq;
    if (check_safety(maxm, alloc, avail, safeSeq)) {
        cout << "Request can be granted. System remains in a safe state.\nSafe sequence: ";
        for (size_t i = 0; i < safeSeq.size(); ++i) {
            cout << "P" << safeSeq[i];
            if (i + 1 != safeSeq.size()) cout << " -> ";
        }
        cout << endl;
        return true; // allocation kept
    } else {
        cout << "Request CANNOT be granted. System would be unsafe.\n";
        // rollback allocation
        for (int j = 0; j < m; ++j) {
            avail[j] += req[j];
            alloc[p][j] -= req[j];
        }
        return false;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: ./banker <input_file>\n";
        return 1;
    }

    std::ifstream fin(argv[1]);
    if (!fin) {
        cout << "Error: cannot open input file " << argv[1] << endl;
        return 1;
    }

    int n, m;
    {
        vector<int> firstLine;
        if (!parse_int_line(fin, firstLine) || firstLine.size() != 2) {
            cout << "Error: first line must be: <num_processes> <num_resources>\n";
            return 1;
        }
        n = firstLine[0];
        m = firstLine[1];
    }

    vector<int> available;
    if (!parse_int_line(fin, available, m)) {
        cout << "Error: could not read Available vector\n";
        return 1;
    }

    vector<vector<int>> maxm(n, vector<int>(m));
    vector<vector<int>> alloc(n, vector<int>(m));

    vector<int> line;
    // Read Max matrix
    for (int i = 0; i < n; ++i) {
        if (!parse_int_line(fin, line, m)) {
            cout << "Error: could not read Max row " << i << endl;
            return 1;
        }
        for (int j = 0; j < m; ++j) maxm[i][j] = line[j];
    }

    // Read Allocation matrix
    for (int i = 0; i < n; ++i) {
        if (!parse_int_line(fin, line, m)) {
            cout << "Error: could not read Allocation row " << i << endl;
            return 1;
        }
        for (int j = 0; j < m; ++j) alloc[i][j] = line[j];
    }

    // Run safety check
    vector<int> safeSeq;
    bool safe = check_safety(maxm, alloc, available, safeSeq);

    if (safe) {
        cout << "System is in a SAFE state.\nSafe sequence: ";
        for (size_t i = 0; i < safeSeq.size(); ++i) {
            cout << "P" << safeSeq[i];
            if (i + 1 != safeSeq.size()) cout << " -> ";
        }
        cout << endl;
    } else {
        cout << "System is NOT in a safe state.\n";
    }

    handle_request(maxm, alloc, available);

    return 0;
}
