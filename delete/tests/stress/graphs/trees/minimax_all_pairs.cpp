// Stress test for src/graphs/trees/minimax_all_pairs.cpp
// Oracle: Floyd-Warshall in the (min, max) semiring.
// Graphs: connected, undirected, weights in [0, 1e9], parallel edges, self-loops, C = 1.
//
// KNOWN BUG: nodes that are unreachable from the source report cap = 0 (the
//   initial value), indistinguishable from cap[s][s]; the test uses connected graphs.
// KNOWN BUG: negative weights are clamped to 0 (the search starts with bestW = 0);
//   the test uses weights >= 0.
#include "template/template.cpp"
#include "src/graphs/trees/minimax_all_pairs.cpp"
#include "tests/stress/common.h"

struct TEdge { int u, v, w; };

static string t_desc(int C, const vector<TEdge> &es) {
    string s = "C=" + to_string(C) + " edges(u,v,w):";
    for (auto &e : es) s += " (" + to_string(e.u) + "," + to_string(e.v) + "," + to_string(e.w) + ")";
    return s;
}

static vvpii t_graph(int C, const vector<TEdge> &es) {
    vvpii g(C);
    for (auto &e : es) {
        g[e.u].push_back({e.v, e.w});
        if (e.u != e.v) g[e.v].push_back({e.u, e.w}); // a self-loop is listed once
    }
    return g;
}

static vector<vll> t_floyd(int C, const vector<TEdge> &es) {
    vector<vll> d(C, vll(C, INF));
    for (int i = 0; i < C; i++) d[i][i] = 0;
    for (auto &e : es) if (e.u != e.v) { d[e.u][e.v] = min<ll>(d[e.u][e.v], e.w); d[e.v][e.u] = d[e.u][e.v]; }
    for (int k = 0; k < C; k++)
        for (int i = 0; i < C; i++)
            for (int j = 0; j < C; j++)
                d[i][j] = min(d[i][j], max(d[i][k], d[k][j]));
    return d;
}

// Random connected multigraph: random spanning tree + extra edges (parallel, self-loops).
static vector<TEdge> t_random(int C, int extra, int maxw) {
    vector<TEdge> es;
    for (int i = 1; i < C; i++) es.push_back({(int)stress::rnd(0, i - 1), i, (int)stress::rnd(0, maxw)});
    for (int k = 0; k < extra; k++) es.push_back({(int)stress::rnd(0, C - 1), (int)stress::rnd(0, C - 1), (int)stress::rnd(0, maxw)});
    shuffle(all(es), stress::rng);
    return es;
}

static void t_check(int C, const vector<TEdge> &es) {
    vvi cap = minimaxAllPairs(C, t_graph(C, es));
    vector<vll> d = t_floyd(C, es);
    CHECK(sz(cap) == C, t_desc(C, es));
    for (int i = 0; i < C; i++) {
        CHECK(sz(cap[i]) == C, t_desc(C, es));
        for (int j = 0; j < C; j++)
            CHECK(cap[i][j] == d[i][j], t_desc(C, es) + " at (" + to_string(i) + "," + to_string(j) + ") got " + to_string(cap[i][j]) + " want " + to_string(d[i][j]));
    }
}

int main() {
    // Deterministic edge cases
    t_check(1, {});
    t_check(1, {{0, 0, 7}});
    t_check(2, {{0, 1, 5}});
    t_check(2, {{0, 1, 5}, {1, 0, 3}, {0, 1, 9}});          // parallel edges
    t_check(2, {{0, 1, 0}});                                // zero weight
    t_check(3, {{0, 1, 1000000000}, {1, 2, 1000000000}});   // max weight
    t_check(3, {{0, 1, 4}, {1, 2, 2}, {0, 2, 9}, {2, 2, 1}}); // path 0-1-2 beats direct, self-loop ignored
    t_check(4, {{0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {3, 0, 4}, {0, 2, 10}});

    for (int it = 0; it < 1500; it++) {
        stress::iter = it;
        int C = (int)stress::rnd(1, it % 10 == 0 ? 30 : 9);
        int extra = (int)stress::rnd(0, 2 * C);
        int maxw = stress::rnd(0, 3) == 0 ? 3 : 1000000000; // small weights force ties
        t_check(C, t_random(C, extra, maxw));
    }
    puts("OK");
}
