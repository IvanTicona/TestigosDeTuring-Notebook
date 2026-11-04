// Stress test for src/graphs/trees/minimax_mst_path.cpp
// solve(V, E) reads E edges "u v w" (1-indexed) from stdin with scanf and returns the
// Kruskal MST as (w, u, v) 0-indexed; dfs(u, v, -1) fills `path` with the MST path.
// Oracles: Prim O(V^2) for the MST weight; Floyd-Warshall in the (min, max) semiring
// for the minimax value; structural checks of the returned edges and of the path.
//
// KNOWN BUG: `mst_cost` is a 32-bit `int` that is computed but never used, and it
//   overflows (signed UB) once the MST weight exceeds 2^31 - 1. The test keeps
//   V <= 30 and weights <= 1e6 so the sum stays far below that.
// KNOWN BUG: `mst_cost` is unused (compiler warning); the snippet is not modified.
// Graphs are connected (a disconnected input yields a spanning forest, not tested).
#include "template/template.cpp"
#include "src/graphs/trees/minimax_mst_path.cpp"
#include "tests/stress/common.h"

struct TEdge { int u, v, w; }; // 0-indexed

static string t_desc(int V, const vector<TEdge> &es) {
    string s = "V=" + to_string(V) + " edges(u,v,w 0-idx):";
    for (auto &e : es) s += " (" + to_string(e.u) + "," + to_string(e.v) + "," + to_string(e.w) + ")";
    return s;
}

static string t_stdin(const vector<TEdge> &es) {
    string s;
    for (auto &e : es) s += to_string(e.u + 1) + " " + to_string(e.v + 1) + " " + to_string(e.w) + "\n";
    return s;
}

static vector<vll> t_floyd(int V, const vector<TEdge> &es) {
    vector<vll> d(V, vll(V, INF));
    for (int i = 0; i < V; i++) d[i][i] = 0;
    for (auto &e : es) if (e.u != e.v) { d[e.u][e.v] = min<ll>(d[e.u][e.v], e.w); d[e.v][e.u] = d[e.u][e.v]; }
    for (int k = 0; k < V; k++)
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                d[i][j] = min(d[i][j], max(d[i][k], d[k][j]));
    return d;
}

static ll t_prim(int V, const vector<TEdge> &es) {
    vector<vll> g(V, vll(V, INF));
    for (auto &e : es) if (e.u != e.v) { g[e.u][e.v] = min<ll>(g[e.u][e.v], e.w); g[e.v][e.u] = g[e.u][e.v]; }
    vll key(V, INF);
    vector<char> in(V, 0);
    key[0] = 0;
    ll total = 0;
    for (int it = 0; it < V; it++) {
        int u = -1;
        for (int i = 0; i < V; i++) if (!in[i] && (u == -1 || key[i] < key[u])) u = i;
        in[u] = 1;
        total += key[u];
        for (int v = 0; v < V; v++) if (!in[v]) key[v] = min(key[v], g[u][v]);
    }
    return total;
}

static vector<TEdge> t_random(int V, int extra, int maxw) {
    vector<TEdge> es;
    for (int i = 1; i < V; i++) es.push_back({(int)stress::rnd(0, i - 1), i, (int)stress::rnd(0, maxw)});
    for (int k = 0; k < extra; k++) es.push_back({(int)stress::rnd(0, V - 1), (int)stress::rnd(0, V - 1), (int)stress::rnd(0, maxw)});
    shuffle(all(es), stress::rng);
    return es;
}

static void t_check(int V, const vector<TEdge> &es, int queries) {
    string in = t_desc(V, es);
    stress::feed_stdin(t_stdin(es));
    vector<iii> mst = solve(V, (int)es.size());

    // MST: V-1 edges, acyclic (so spanning, graph is connected), weight == Prim
    CHECK(sz(mst) == V - 1, in + " mst edges " + to_string(sz(mst)));
    vi lab(V);
    iota(all(lab), 0);
    ll total = 0;
    for (auto &[w, u, v] : mst) {
        CHECK(0 <= u && u < V && 0 <= v && v < V && u != v, in + " bad mst edge");
        CHECK(lab[u] != lab[v], in + " mst has a cycle");
        int from = lab[v], to = lab[u];
        for (auto &l : lab) if (l == from) l = to;
        total += w;
        bool found = false; // it must be an input edge
        for (auto &e : es) if (e.w == w && ((e.u == u && e.v == v) || (e.u == v && e.v == u))) found = true;
        CHECK(found, in + " mst edge not in input");
    }
    CHECK(total == t_prim(V, es), in + " mst weight " + to_string(total) + " want " + to_string(t_prim(V, es)));

    // Path queries on the MST: dfs fills `path` from v back to u
    grafo.assign(V, {});
    for (auto &[w, u, v] : mst) { grafo[u].push_back({v, w}); grafo[v].push_back({u, w}); }
    vector<vll> d = t_floyd(V, es);
    for (int q = 0; q < queries; q++) {
        int u = (int)stress::rnd(0, V - 1), v = (int)stress::rnd(0, V - 1);
        string qin = in + " query(" + to_string(u) + "," + to_string(v) + ")";
        visited.assign(V, false);
        path.clear();
        CHECK(dfs(u, v, -1), qin + " no path");
        CHECK(!path.empty() && path.front().first == v && path.back().first == u && path.back().second == -1, qin + " path ends");
        set<int> seen;
        ll mx = 0;
        for (int k = 0; k < sz(path); k++) {
            CHECK(seen.insert(path[k].first).second, qin + " path repeats a node");
            if (k + 1 < sz(path)) {
                // path[k] is entered from path[k+1] through an MST edge of weight path[k].second
                int a = path[k + 1].first, b = path[k].first, w = path[k].second;
                bool ok = false;
                for (auto &[nb, nw] : grafo[a]) if (nb == b && nw == w) ok = true;
                CHECK(ok, qin + " path step is not an MST edge");
                mx = max<ll>(mx, w);
            }
        }
        CHECK(mx == d[u][v], qin + " minimax " + to_string(mx) + " want " + to_string(d[u][v]));
    }
}

int main() {
    // Deterministic edge cases
    t_check(1, {}, 3);                                   // V = 1, E = 0
    t_check(1, {{0, 0, 5}}, 3);                          // only a self-loop
    t_check(2, {{0, 1, 0}}, 4);                          // zero weight
    t_check(2, {{0, 1, 9}, {1, 0, 2}, {0, 1, 5}}, 4);    // parallel edges
    t_check(3, {{0, 1, 4}, {1, 2, 2}, {0, 2, 9}, {2, 2, 1}}, 9);
    t_check(4, {{0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {3, 0, 4}, {0, 2, 10}}, 16);
    t_check(3, {{0, 1, 1000000}, {1, 2, 1000000}, {0, 2, 1000000}}, 9);

    for (int it = 0; it < 400; it++) {
        stress::iter = it;
        int V = (int)stress::rnd(1, it % 10 == 0 ? 30 : 9);
        int extra = (int)stress::rnd(0, 2 * V);
        int maxw = stress::rnd(0, 3) == 0 ? 3 : 1000000; // small weights force ties
        t_check(V, t_random(V, extra, maxw), 6);
    }
    puts("OK");
}
