// Stress test for src/dp/lcs.cpp
// Oracles: LCS length by enumerating all subsequences of the shorter
// string (2^n, n <= 12) and checking subsequence membership; the returned
// LCS must be a common subsequence of that length; lcs3 by memoized
// recursion over (i, j, k) suffixes; edit distance by Dijkstra over
// (i, j) with insert/delete/replace edges and random costs.
#include "tests/stress/template_lib.h"
#include "src/dp/lcs.cpp"
#include "tests/stress/common.h"

bool is_sub(const string &s, const string &t) {  // s subsequence of t
    int j = 0;
    for (char c : t) if (j < sz(s) && s[j] == c) j++;
    return j == sz(s);
}

int main() {
    stress::check_example(example_lcs, "src/dp/lcs.cpp", "example_lcs");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        char hi = stress::rnd(0, 1) ? 'b' : 'e';
        string a = stress::rnd_str(stress::rnd(0, 11), 'a', hi), b = stress::rnd_str(stress::rnd(0, 14), 'a', hi);
        int best = 0;
        for (int m = 0; m < (1 << sz(a)); m++) {
            string s;
            for (int i = 0; i < sz(a); i++) if (m >> i & 1) s += a[i];
            if (sz(s) > best && is_sub(s, b)) best = sz(s);
        }
        string r = lcs(a, b);
        CHECK(sz(r) == best && is_sub(r, a) && is_sub(r, b), "lcs a=" + a + " b=" + b);
        string c = stress::rnd_str(stress::rnd(0, 8), 'a', hi);
        map<tuple<int, int, int>, int> memo;
        auto rec = [&](auto &&self, int i, int j, int k) -> int {
            if (i == sz(a) || j == sz(b) || k == sz(c)) return 0;
            auto key = make_tuple(i, j, k);
            if (memo.count(key)) return memo[key];
            int v = max({self(self, i + 1, j, k), self(self, i, j + 1, k), self(self, i, j, k + 1)});
            if (a[i] == b[j] && b[j] == c[k]) v = max(v, 1 + self(self, i + 1, j + 1, k + 1));
            return memo[key] = v;
        };
        CHECK(lcs3(a, b, c) == rec(rec, 0, 0, 0), "lcs3");
        ll ins = stress::rnd(0, 5), del = stress::rnd(0, 5), rep = stress::rnd(0, 12);
        // Dijkstra on (i, j): consumed i chars of a, produced j chars of b
        int n = sz(a), m = sz(b);
        vector<vl> d(n + 1, vl(m + 1, INFL));
        priority_queue<pair<ll, pii>, vector<pair<ll, pii>>, greater<>> pq;
        d[0][0] = 0, pq.push({0, {0, 0}});
        while (!pq.empty()) {
            auto [du, p] = pq.top();
            pq.pop();
            auto [i, j] = p;
            if (du > d[i][j]) continue;
            auto relax = [&](int x, int y, ll w) {
                if (du + w < d[x][y]) d[x][y] = du + w, pq.push({d[x][y], {x, y}});
            };
            if (i < n) relax(i + 1, j, del);
            if (j < m) relax(i, j + 1, ins);
            if (i < n && j < m) relax(i + 1, j + 1, a[i] == b[j] ? 0 : rep);
        }
        CHECK(edit_distance(a, b, ins, del, rep) == d[n][m], "edit a=" + a + " b=" + b);
        CHECK(edit_distance(a, b) == edit_distance(b, a), "edit symmetric (unit costs)");
    }
    // vectors of ints
    vl x = {1, 2, 3, 2, 1}, y = {3, 2, 1, 2};
    CHECK(sz(lcs(x, y)) == 3, "vector lcs");
    puts("OK");
}
