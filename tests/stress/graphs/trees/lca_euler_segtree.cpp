// Stress test for src/graphs/trees/lca_euler_segtree.cpp
// Oracle: BFS parent/depth arrays from the root + naive climb to the common ancestor.
// Trees: random (random parent, relabelled), chain, star, caterpillar; root != 0;
// queries with u == v, ancestor pairs, and random pairs.
#include "template/template.cpp"
#include "src/graphs/trees/lca_euler_segtree.cpp"
#include "tests/stress/common.h"

struct TTree {
    int n = 0, root = 0;
    vvi adj;
    vector<pii> edges;
};

// kind: 0 random, 1 chain, 2 star, 3 caterpillar, 4 binary heap shape
static TTree t_make(int n, int kind) {
    vector<int> label(n);
    iota(all(label), 0);
    shuffle(all(label), stress::rng);
    TTree t;
    t.n = n;
    t.adj.assign(n, {});
    for (int i = 1; i < n; i++) {
        int p = 0;
        switch (kind) {
            case 0: p = (int)stress::rnd(0, i - 1); break;
            case 1: p = i - 1; break;
            case 2: p = 0; break;
            case 3: p = (i % 2 == 1) ? max(0, i - 2) : i - 1; break;
            default: p = (i - 1) / 2; break;
        }
        int a = label[i], b = label[p];
        t.adj[a].push_back(b);
        t.adj[b].push_back(a);
        t.edges.push_back({a, b});
    }
    for (auto &v : t.adj) shuffle(all(v), stress::rng);
    t.root = (int)stress::rnd(0, n - 1);
    return t;
}

struct TNaive {
    vi par, dep;
    TNaive(const TTree &t) : par(t.n, -1), dep(t.n, -1) {
        queue<int> q;
        q.push(t.root);
        dep[t.root] = 0;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : t.adj[u]) if (dep[v] == -1) { dep[v] = dep[u] + 1; par[v] = u; q.push(v); }
        }
    }
    int lca(int u, int v) const {
        while (dep[u] > dep[v]) u = par[u];
        while (dep[v] > dep[u]) v = par[v];
        while (u != v) { u = par[u]; v = par[v]; }
        return u;
    }
};

static string t_desc(const TTree &t, int kind, int u, int v) {
    string s = "n=" + to_string(t.n) + " kind=" + to_string(kind) + " root=" + to_string(t.root) +
               " query=(" + to_string(u) + "," + to_string(v) + ") edges:";
    if (t.n <= 40) for (auto &e : t.edges) s += " " + to_string(e.first) + "-" + to_string(e.second);
    return s;
}

int main() {
    // Deterministic edge cases
    {
        vvi adj(1);
        LCA l(adj, 0);
        CHECK(l.lca(0, 0) == 0, "n=1");
    }
    {
        vvi adj = {{1}, {0}};
        LCA l0(adj, 0), l1(adj, 1);
        CHECK(l0.lca(0, 1) == 0 && l0.lca(1, 0) == 0 && l1.lca(0, 1) == 1 && l1.lca(1, 1) == 1, "n=2");
    }
    {
        // 0-1, 0-2, 1-3, 1-4, 2-5
        vvi adj = {{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
        LCA l(adj, 0);
        CHECK(l.lca(3, 4) == 1 && l.lca(3, 5) == 0 && l.lca(4, 1) == 1 && l.lca(5, 2) == 2 && l.lca(0, 5) == 0, "sample tree");
        LCA l3(adj, 3);
        CHECK(l3.lca(4, 5) == 1 && l3.lca(0, 5) == 0 && l3.lca(2, 5) == 2, "sample tree rooted at 3");
    }

    // Small trees: every pair (u, v), all shapes, random roots
    for (int it = 0; it < 600; it++) {
        stress::iter = it;
        int n = (int)stress::rnd(1, 25), kind = it % 5;
        TTree t = t_make(n, kind);
        TNaive nv(t);
        LCA l(t.adj, t.root);
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++)
                CHECK(l.lca(u, v) == nv.lca(u, v), t_desc(t, kind, u, v));
    }

    // Large trees (n <= 3000): sampled queries incl. u == v and ancestor pairs
    for (int it = 0; it < 30; it++) {
        stress::iter = it;
        int n = (int)stress::rnd(500, 3000), kind = it % 5;
        TTree t = t_make(n, kind);
        TNaive nv(t);
        LCA l(t.adj, t.root);
        for (int q = 0; q < 1500; q++) {
            int u = (int)stress::rnd(0, n - 1), v = (int)stress::rnd(0, n - 1);
            int mode = q % 3;
            if (mode == 1) v = u;
            if (mode == 2) { // v becomes a random ancestor of u
                int steps = (int)stress::rnd(0, nv.dep[u]);
                v = u;
                while (steps--) v = nv.par[v];
            }
            CHECK(l.lca(u, v) == nv.lca(u, v), t_desc(t, kind, u, v));
            CHECK(l.lca(v, u) == nv.lca(u, v), t_desc(t, kind, v, u));
        }
    }
    puts("OK");
}
