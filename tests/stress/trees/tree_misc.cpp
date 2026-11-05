// Stress test for src/trees/tree_misc.cpp
// Oracles: all-pairs distances (BFS from every vertex) for the diameter
// (weights >= 0) and the centers (min eccentricity); isomorphism vs trying
// every vertex bijection (n <= 7) on random trees and relabeled copies;
// Prufer: decode(encode(T)) == T as edge sets, every code of length n-2
// decodes to a valid tree with distinct codes for distinct trees (n <= 6,
// n^(n-2) trees).
#include "tests/stress/template_lib.h"
#include "src/trees/tree_misc.cpp"
#include "tests/stress/common.h"

vector<vi> rand_tree(int n) {
    vector<vi> g(n);
    vi perm(n);
    iota(all(perm), 0), shuffle(all(perm), stress::rng);
    for (int i = 1; i < n; i++) {
        int p = stress::rnd(0, i - 1);
        g[perm[i]].pb(perm[p]), g[perm[p]].pb(perm[i]);
    }
    return g;
}

int main() {
    stress::check_example(example_tree_misc, "src/trees/tree_misc.cpp", "example_tree_misc");
    for (stress::iter = 0; stress::iter < stress::scaled(500); stress::iter++) {
        int n = stress::rnd(1, 40);
        vector<vi> t = rand_tree(n);
        vector<vector<pll>> g(n);
        for (int u = 0; u < n; u++)
            for (int v : t[u]) if (u < v) {
                ll w = stress::rnd(0, 20);
                g[u].pb({v, w}), g[v].pb({u, w});
            }
        vector<vl> D(n);
        vector<vl> E(n);  // unweighted
        for (int s = 0; s < n; s++) {
            D[s] = far_from(g, s).F;
            vector<vector<pll>> h(n);
            for (int u = 0; u < n; u++) for (auto [v, w] : g[u]) h[u].pb({v, 1});
            E[s] = far_from(h, s).F;
        }
        ll best = 0;
        for (int u = 0; u < n; u++) for (int v = 0; v < n; v++) best = max(best, D[u][v]);
        auto [len, path] = diameter(g);
        CHECK(len == best && D[path[0]][path.back()] == best, "diameter");
        for (int i = 0; i + 1 < sz(path); i++) CHECK(count(all(t[path[i]]), path[i + 1]), "path edges");
        ll mecc = LLONG_MAX;
        vl ecc(n);
        for (int u = 0; u < n; u++) ecc[u] = *max_element(all(E[u])), mecc = min(mecc, ecc[u]);
        vi want;
        for (int u = 0; u < n; u++) if (ecc[u] == mecc) want.pb(u);
        vi c = centers(g);
        sort(all(c));
        CHECK(c == want, "centers");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(800); stress::iter++) {
        int n = stress::rnd(1, 7);
        vector<vi> a = rand_tree(n), b = stress::rnd(0, 1) ? rand_tree(n) : a;
        if (b == a) {  // relabel
            vi p(n);
            iota(all(p), 0), shuffle(all(p), stress::rng);
            vector<vi> c(n);
            for (int u = 0; u < n; u++) for (int v : a[u]) c[p[u]].pb(p[v]);
            b = c;
        }
        vector<vector<char>> A(n, vector<char>(n, 0)), Bm(n, vector<char>(n, 0));
        for (int u = 0; u < n; u++) for (int v : a[u]) A[u][v] = 1;
        for (int u = 0; u < n; u++) for (int v : b[u]) Bm[u][v] = 1;
        vi p(n);
        iota(all(p), 0);
        bool want = false;
        do {
            bool ok = true;
            for (int u = 0; u < n && ok; u++)
                for (int v = 0; v < n; v++) if (A[u][v] != Bm[p[u]][p[v]]) { ok = false; break; }
            want |= ok;
        } while (!want && next_permutation(all(p)));
        TreeIso ti;
        CHECK(ti.iso(a, b) == want, "isomorphism n=" + to_string(n));
    }
    for (int n = 2; n <= 6; n++) {  // all Prufer codes
        int total = 1;
        for (int i = 0; i < n - 2; i++) total *= n;
        set<set<pii>> trees;
        for (int code_id = 0; code_id < total; code_id++) {
            vi code;
            for (int i = 0, x = code_id; i < n - 2; i++) code.pb(x % n), x /= n;
            auto es = from_prufer(code);
            CHECK(sz(es) == n - 1, "edge count");
            vector<vi> adj(n);
            set<pii> s;
            vi pp(n);
            iota(all(pp), 0);
            function<int(int)> f = [&](int x) { return pp[x] == x ? x : pp[x] = f(pp[x]); };
            for (auto [u, v] : es) {
                adj[u].pb(v), adj[v].pb(u), s.insert({min(u, v), max(u, v)});
                CHECK(f(u) != f(v), "acyclic");
                pp[f(u)] = f(v);
            }
            trees.insert(s);
            CHECK(prufer(adj) == code, "encode(decode(code))");
        }
        CHECK(sz(trees) == total, "n^(n-2) distinct trees");
    }
    puts("OK");
}
