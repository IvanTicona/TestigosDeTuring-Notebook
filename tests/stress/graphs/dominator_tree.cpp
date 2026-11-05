// Stress test for src/graphs/dominator_tree.cpp
// Oracle: by definition, d strictly dominates v iff v is unreachable from
// the root after deleting d (BFS per d); idom(v) = the strict dominator
// that is dominated by all the others (the one with the most dominators);
// a 2e5-node path checks the iterative implementation.
#include "tests/stress/template_lib.h"
#include "src/graphs/dominator_tree.cpp"
#include "tests/stress/common.h"

vector<char> reach(const vector<vi> &g, int root, int banned) {
    vector<char> vis(sz(g), 0);
    if (root == banned) return vis;
    vi st = {root};
    vis[root] = 1;
    while (!st.empty()) {
        int u = st.back();
        st.pop_back();
        for (int v : g[u]) if (v != banned && !vis[v]) vis[v] = 1, st.pb(v);
    }
    return vis;
}

int main() {
    stress::check_example(example_dominator_tree, "src/graphs/dominator_tree.cpp", "example_dominator_tree");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 12), m = stress::rnd(0, 30), root = stress::rnd(0, n - 1);
        vector<vi> g(n);
        for (int i = 0; i < m; i++) g[stress::rnd(0, n - 1)].pb(stress::rnd(0, n - 1));
        auto base = reach(g, root, -1);
        vector<vector<char>> dom(n, vector<char>(n, 0));  // dom[d][v]
        for (int d = 0; d < n; d++) {
            auto r = reach(g, root, d);
            for (int v = 0; v < n; v++) dom[d][v] = base[v] && v != d && !r[v];
        }
        vi idom = dominators(g, root);
        for (int v = 0; v < n; v++) {
            if (!base[v]) { CHECK(idom[v] == -1, "unreachable"); continue; }
            if (v == root) { CHECK(idom[v] == root, "root"); continue; }
            int best = -1, bc = -1;
            for (int d = 0; d < n; d++) {
                if (!dom[d][v]) continue;
                int c = 0;
                for (int e = 0; e < n; e++) c += dom[e][d];
                if (c > bc) bc = c, best = d;
            }
            CHECK(idom[v] == best, "idom v=" + to_string(v));
        }
    }
    int n = 200000;
    vector<vi> g(n);
    for (int i = 0; i + 1 < n; i++) g[i].pb(i + 1);
    for (int i = 2; i < n; i += 2) g[0].pb(i);  // shortcut from root to evens
    vi d = dominators(g, 0);
    for (int v = 1; v < n; v++) CHECK(d[v] == (v % 2 ? v - 1 : 0), "big idom");
    puts("OK");
}
