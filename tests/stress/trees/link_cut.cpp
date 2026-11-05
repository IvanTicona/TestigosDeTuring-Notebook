// Stress test for src/trees/link_cut.cpp
// Oracle: explicit forest (adjacency sets) with BFS paths: random links of
// different components, cuts of existing (and non-existing) edges, value
// updates, path sums, connectivity and LCA for a random root (path from
// the root to u and to v, last common vertex).
#include "tests/stress/template_lib.h"
#include "src/trees/link_cut.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_link_cut, "src/trees/link_cut.cpp", "example_link_cut");
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        int n = stress::rnd(1, 25);
        LinkCut t(n);
        vector<set<int>> adj(n);
        vl val(n, 0);
        auto bfs_path = [&](int s, int e) {  // vertices from s to e, {} if none
            vi par(n, -2), q = {s};
            par[s] = -1;
            for (int i = 0; i < sz(q); i++)
                for (int v : adj[q[i]]) if (par[v] == -2) par[v] = q[i], q.pb(v);
            if (par[e] == -2) return vi();
            vi p;
            for (int v = e; v != -1; v = par[v]) p.pb(v);
            reverse(all(p));
            return p;
        };
        for (int op = 0; op < 300; op++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1), ty = stress::rnd(0, 5);
            bool conn = !bfs_path(u, v).empty();
            if (ty == 0) {
                bool ok = t.link(u, v);
                CHECK(ok == !conn, "link result");
                if (ok) adj[u].insert(v), adj[v].insert(u);
            } else if (ty == 1) {
                if (stress::rnd(0, 1) && !adj[u].empty()) v = *adj[u].begin();
                bool has = adj[u].count(v);
                CHECK(t.cut(u, v) == has, "cut result");
                if (has) adj[u].erase(v), adj[v].erase(u);
            } else if (ty == 2) {
                ll x = stress::rnd(-(ll)1e9, (ll)1e9);
                t.set(u, x), val[u] = x;
            } else if (ty == 3) {
                CHECK(t.connected(u, v) == conn, "connected");
            } else if (conn) {
                vi p = bfs_path(u, v);
                ll s = 0;
                for (int x : p) s += val[x];
                CHECK(t.path_sum(u, v) == s, "path sum");
                int r = p[stress::rnd(0, sz(p) - 1)];
                for (int x = 0; x < n; x++) if (!bfs_path(r, x).empty() && stress::rnd(0, 3) == 0) { r = x; break; }
                vi pu = bfs_path(r, u), pv = bfs_path(r, v);
                int k = 0;
                while (k < min(sz(pu), sz(pv)) && pu[k] == pv[k]) k++;
                CHECK(t.lca(r, u, v) == pu[k - 1], "lca");
            }
        }
    }
    int n = 200000;  // long path: amortized splay depth
    LinkCut t(n);
    for (int i = 0; i + 1 < n; i++) t.link(i, i + 1);
    for (int v = 0; v < n; v++) t.set(v, 1);
    CHECK(t.path_sum(0, n - 1) == n && t.cut(n / 2, n / 2 + 1) && !t.connected(0, n - 1), "big path");
    puts("OK");
}
