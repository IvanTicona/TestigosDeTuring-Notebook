// Stress test for src/backtracking/n_queens.cpp
// Oracles: (1) known solution counts (OEIS A000170) for N = 0..10;
//          (2) permutation enumeration for N <= 8: the set of boards printed by the
//              snippet (captured from cout and parsed) must equal the brute-force set.
#include "template/template.cpp"
#include "src/backtracking/n_queens.cpp"
#include "tests/stress/common.h"

static const int T_A000170[] = {1, 1, 0, 0, 2, 10, 4, 40, 92, 352, 724};

// Runs the snippet the documented way and returns the captured output.
static string t_run(int n) {
    ::N = n;
    col.assign(n, 0);
    solutions = 0;
    stress::Capture cap;
    solve(0);
    return cap.str();
}

static string t_vec(const vi &v) {
    string s = "[";
    for (int x : v) s += to_string(x) + " ";
    return s + "]";
}

// Parses "Solution k:\n<N rows of 'Q ' / '. '>\n\n" blocks into column vectors.
static vector<vi> t_parse(const string &out, int n, const string &in) {
    vector<vi> boards;
    istringstream is(out);
    string line;
    while (getline(is, line)) {
        CHECK(line == "Solution " + to_string(sz(boards) + 1) + ":", in + " header: '" + line + "'");
        vi cols;
        for (int r = 0; r < n; r++) {
            CHECK((bool)getline(is, line), in + " truncated board");
            CHECK(sz(line) == 2 * n, in + " row width: '" + line + "'");
            int c = -1;
            for (int j = 0; j < n; j++) {
                CHECK((line[2 * j] == 'Q' || line[2 * j] == '.') && line[2 * j + 1] == ' ', in + " cell: '" + line + "'");
                if (line[2 * j] == 'Q') { CHECK(c == -1, in + " two queens in a row: '" + line + "'"); c = j; }
            }
            CHECK(c != -1, in + " row without queen: '" + line + "'");
            cols.push_back(c);
        }
        CHECK((bool)getline(is, line) && line.empty(), in + " missing blank separator");
        boards.push_back(cols);
    }
    return boards;
}

static bool t_valid(const vi &p) {
    int n = sz(p);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (p[i] == p[j] || abs(p[i] - p[j]) == j - i) return false;
    return true;
}

int main() {
    for (int n = 0; n <= 10; n++) {
        stress::iter = n;
        string in = "N=" + to_string(n);
        string out = t_run(n);
        CHECK(solutions == T_A000170[n], in + " count " + to_string(solutions));
        vector<vi> boards = t_parse(out, n, in);
        CHECK(sz(boards) == T_A000170[n], in + " printed boards " + to_string(sz(boards)));

        // every printed board is a valid placement, and all are distinct
        set<vi> printed;
        for (auto &b : boards) {
            CHECK(t_valid(b), in + " invalid board " + t_vec(b));
            CHECK(printed.insert(b).second, in + " duplicate board " + t_vec(b));
        }

        if (n <= 8) { // brute force: all permutations (one queen per row and column)
            set<vi> brute;
            vi p(n);
            iota(all(p), 0);
            do { if (t_valid(p)) brute.insert(p); } while (next_permutation(all(p)));
            CHECK(printed == brute, in + " board set differs from permutation enumeration");
        }
    }

    // Re-running with a different N after a larger one (global state is reset by the caller)
    for (int it = 0; it < 30; it++) {
        stress::iter = it;
        int n = (int)stress::rnd(0, 8);
        t_run(n);
        CHECK(solutions == T_A000170[n], "rerun N=" + to_string(n));
    }
    puts("OK");
}
