// Stress test for src/ds/dynamic_connectivity.cpp
// Oracle: keep the current multiset of edges and answer each query with a
// fresh BFS (components and connectivity); multi-edges and self-loops,
// edges never removed, removals right after insertion.
#include "tests/stress/template_lib.h"
#include "src/ds/dsu.cpp"
#include "src/ds/dynamic_connectivity.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_dynamic_connectivity, "src/ds/dynamic_connectivity.cpp", "example_dynamic_connectivity");
    CHECK(DynCon(3).solve().empty(), "no events");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 12), T = stress::rnd(1, 120);
        DynCon dc(n);
        multiset<pii> cur;
        vl want;
        for (int t = 0; t < T; t++) {
            int ty = stress::rnd(0, 3);
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            if (ty == 1 && cur.empty()) ty = 0;
            if (ty == 0) dc.add(u, v), cur.insert({min(u, v), max(u, v)});
            else if (ty == 1) {
                auto it = cur.begin();
                advance(it, stress::rnd(0, sz(cur) - 1));
                dc.remove(it->S, it->F), cur.erase(it);  // reversed order on purpose
            } else {
                vector<vi> g(n);
                for (auto [a, b] : cur) g[a].pb(b), g[b].pb(a);
                vi comp(n, -1);
                int c = 0;
                for (int s = 0; s < n; s++) {
                    if (comp[s] >= 0) continue;
                    queue<int> q;
                    q.push(s), comp[s] = c;
                    while (!q.empty()) {
                        int x = q.front();
                        q.pop();
                        for (int y : g[x]) if (comp[y] < 0) comp[y] = c, q.push(y);
                    }
                    c++;
                }
                if (ty == 2) dc.ask_comps(), want.pb(c);
                else dc.ask(u, v), want.pb(comp[u] == comp[v]);
            }
        }
        CHECK(dc.solve() == want, "answers n=" + to_string(n) + " T=" + to_string(T));
    }
    puts("OK");
}
