// Stress test for src/dp/edit_distance.cpp
// Oracle: BFS over single-character edit operations (a different method than the DP).
#include "template/template.cpp"
#include "src/dp/edit_distance.cpp"
#include "tests/stress/common.h"

static string t_q(const string &s) { return "\"" + s + "\""; }

// BFS distance from `src` to every string of length <= maxLen over `alpha`.
static unordered_map<string, int> t_bfs(const string &src, const string &alpha, int maxLen) {
    unordered_map<string, int> dist;
    queue<string> q;
    dist[src] = 0;
    q.push(src);
    while (!q.empty()) {
        string u = q.front(); q.pop();
        int d = dist[u];
        auto visit = [&](const string &v) {
            if (sz(v) > maxLen) return;
            if (dist.emplace(v, d + 1).second) q.push(v);
        };
        for (int i = 0; i < sz(u); i++) {
            visit(u.substr(0, i) + u.substr(i + 1)); // delete
            for (char c : alpha) if (c != u[i]) { string v = u; v[i] = c; visit(v); } // replace
        }
        for (int i = 0; i <= sz(u); i++)
            for (char c : alpha) visit(u.substr(0, i) + c + u.substr(i)); // insert
    }
    return dist;
}

int main() {
    // Deterministic edge cases
    CHECK(editDistance("", "") == 0, "'' ''");
    CHECK(editDistance("", "abc") == 3, "'' abc");
    CHECK(editDistance("abc", "") == 3, "abc ''");
    CHECK(editDistance("kitten", "sitting") == 3, "kitten sitting");
    CHECK(editDistance("abc", "abc") == 0, "abc abc");
    CHECK(editDistance("a", "b") == 1, "a b");
    CHECK(editDistance("abc", "xyz") == 3, "abc xyz");
    CHECK(editDistance("intention", "execution") == 5, "intention execution");

    // Exhaustive: every pair of strings of length <= 4 over {a,b} vs BFS
    {
        vector<string> all = {""};
        for (int len = 1; len <= 4; len++)
            for (int mask = 0; mask < (1 << len); mask++) {
                string s;
                for (int i = 0; i < len; i++) s += (mask >> i & 1) ? 'b' : 'a';
                all.push_back(s);
            }
        for (auto &a : all) {
            auto dist = t_bfs(a, "ab", 5);
            for (auto &b : all)
                CHECK(editDistance(a, b) == dist[b], t_q(a) + " " + t_q(b));
        }
    }

    // Random: alphabet of 3, length <= 5, BFS oracle
    for (int it = 0; it < 60; it++) {
        stress::iter = it;
        string a = stress::rnd_str((int)stress::rnd(0, 5), 'a', 'c');
        auto dist = t_bfs(a, "abc", 6);
        for (int k = 0; k < 40; k++) {
            string b = stress::rnd_str((int)stress::rnd(0, 5), 'a', 'c');
            CHECK(editDistance(a, b) == dist[b], t_q(a) + " " + t_q(b));
        }
    }

    // Random longer strings: metric properties
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        int alpha = (int)stress::rnd(1, 4);
        string a = stress::rnd_str((int)stress::rnd(0, 30), 'a', 'a' + alpha - 1);
        string b = stress::rnd_str((int)stress::rnd(0, 30), 'a', 'a' + alpha - 1);
        string c = stress::rnd_str((int)stress::rnd(0, 30), 'a', 'a' + alpha - 1);
        string in = t_q(a) + " " + t_q(b) + " " + t_q(c);
        int dab = editDistance(a, b), dba = editDistance(b, a);
        CHECK(editDistance(a, a) == 0, in);
        CHECK(dab == dba, in);
        CHECK(dab <= editDistance(a, c) + editDistance(c, b), in);
        CHECK(abs(sz(a) - sz(b)) <= dab && dab <= max(sz(a), sz(b)), in);
    }
    puts("OK");
}
