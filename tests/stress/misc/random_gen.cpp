// Stress test for src/misc/random_gen.cpp
// Oracles: structural properties checked directly: permutations contain
// each value once; distinct values are distinct, sorted and in range;
// trees have n - 1 edges and are connected (DSU, no cycle); graphs are
// simple, connected, with exactly m edges; strings use only the first k
// letters.
#include "tests/stress/template_lib.h"
#include "src/misc/random_gen.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_random_gen, "src/misc/random_gen.cpp", "example_random_gen");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        int n = stress::rnd(1, 30);
        vi p = rand_perm(n), q = p;
        sort(all(q));
        for (int i = 0; i < n; i++) CHECK(q[i] == i, "permutation");
        ll lo = stress::rnd(-50, 50), hi = lo + stress::rnd(0, 40);
        int k = stress::rnd(0, (int)(hi - lo + 1));
        vl d = rand_distinct(k, lo, hi);
        CHECK(sz(d) == k && is_sorted(all(d)) && (d.empty() || (d[0] >= lo && d.back() <= hi)), "distinct");
        CHECK(sz(set<ll>(all(d))) == k, "distinct values");
        auto check_conn = [&](const vector<pii> &e, bool tree) {
            vi par(n);
            iota(all(par), 0);
            function<int(int)> f = [&](int x) { return par[x] == x ? x : par[x] = f(par[x]); };
            int comps = n;
            for (auto [u, v] : e) {
                CHECK(0 <= u && u < n && 0 <= v && v < n && u != v, "edge range");
                int a = f(u), b = f(v);
                if (a == b) CHECK(!tree, "tree has no cycle");
                else par[a] = b, comps--;
            }
            CHECK(comps == 1, "connected");
        };
        auto t = rand_tree(n);
        CHECK(sz(t) == n - 1, "tree edges");
        check_conn(t, true);
        int m = stress::rnd(n - 1, n * (n - 1) / 2);
        auto g = rand_graph(n, m);
        CHECK(sz(g) == m, "graph edges");
        set<pii> s;
        for (auto [u, v] : g) CHECK(s.insert({min(u, v), max(u, v)}).second, "simple");
        check_conn(g, false);
        int kk = stress::rnd(1, 26);
        string str = rand_str(stress::rnd(0, 30), kk);
        for (char c : str) CHECK(c >= 'a' && c < 'a' + kk, "alphabet");
    }
    puts("OK");
}
