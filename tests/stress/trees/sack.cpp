// Stress test for src/trees/sack.cpp
// Oracles: at every ans(v) the set of present vertices must be exactly the
// subtree of v (computed by parent walking); distinct-color counts vs
// direct subtree scans; total add calls <= n (log2 n + 1); vertex 0 as a
// non-root child; a 2e5-node path / star for the iterative traversal.
#include "tests/stress/template_lib.h"
#include "src/trees/sack.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_sack, "src/trees/sack.cpp", "example_sack");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 60), root = stress::rnd(0, n - 1), C = stress::rnd(1, 6);
        vector<vi> g(n);
        vi perm(n);
        iota(all(perm), 0), shuffle(all(perm), stress::rng);
        bool forest = stress::rnd(0, 2) == 0;  // the old version crashed
        for (int i = 1; i < n; i++) {
            if (forest && stress::rnd(0, 3) == 0) continue;
            int p = stress::rnd(0, i - 1);
            g[perm[i]].pb(perm[p]), g[perm[p]].pb(perm[i]);
        }
        vi col(n);
        for (int &c : col) c = stress::rnd(0, C - 1);
        vi par(n, -1), q;
        vector<char> seen(n, 0);
        for (int r0 = -1; r0 < n; r0++) {
            int r = r0 < 0 ? root : r0;
            if (seen[r]) continue;
            seen[r] = 1, q.pb(r);
            for (int i = sz(q) - 1; i < sz(q); i++)
                for (int v : g[q[i]]) if (!seen[v]) seen[v] = 1, par[v] = q[i], q.pb(v);
        }
        auto in_sub = [&](int x, int v) {
            for (int z = x; z >= 0; z = par[z]) if (z == v) return true;
            return false;
        };
        vector<char> present(n, 0);
        vi cnt(C, 0);
        int distinct = 0;
        ll adds = 0;
        vi got(n, -1);
        sack(g, root,
             [&](int x) { CHECK(!present[x], "double add"); present[x] = 1, adds++, distinct += cnt[col[x]]++ == 0; },
             [&](int x) { CHECK(present[x], "remove absent"); present[x] = 0, distinct -= --cnt[col[x]] == 0; },
             [&](int v) {
                 set<int> cs;
                 for (int x = 0; x < n; x++) {
                     CHECK(present[x] == in_sub(x, v), "structure == subtree");
                     if (in_sub(x, v)) cs.insert(col[x]);
                 }
                 got[v] = distinct;
                 CHECK(distinct == sz(cs), "distinct colors");
             });
        for (int v = 0; v < n; v++) CHECK(got[v] >= 0, "ans called once per vertex");
        int lg = 0;
        while ((1 << lg) < n) lg++;
        CHECK(adds <= (ll)n * (lg + 1), "O(n log n) adds");
    }
    for (int shape = 0; shape < 2; shape++) {
        int n = stress::SLOW ? 20000 : 200000;
        vector<vi> g(n);
        for (int i = 1; i < n; i++) {
            int p = shape ? 0 : i - 1;
            g[i].pb(p), g[p].pb(i);
        }
        ll adds = 0, calls = 0;
        sack(g, 0, [&](int) { adds++; }, [&](int) {}, [&](int) { calls++; });
        CHECK(calls == n && adds <= 2LL * n, "big shapes");
    }
    puts("OK");
}
