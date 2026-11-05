// Stress test for src/trees/mo_tree.cpp
// Oracles: at every get() the set of present vertices must be exactly the
// u - v path (naive parent walking); distinct colors and value sums on
// the path vs direct computation; u == v and ancestor / descendant pairs.
#include "tests/stress/template_lib.h"
#include "src/ds/mo.cpp"
#include "src/trees/euler_tour.cpp"
#include "src/trees/mo_tree.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_mo_tree, "src/trees/mo_tree.cpp", "example_mo_tree");
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        int n = stress::rnd(1, 50), root = stress::rnd(0, n - 1), C = stress::rnd(1, 5), q = stress::rnd(1, 60);
        vector<vi> g(n);
        vi perm(n);
        iota(all(perm), 0), shuffle(all(perm), stress::rng);
        for (int i = 1; i < n; i++) {
            int p = stress::rnd(0, i - 1);
            g[perm[i]].pb(perm[p]), g[perm[p]].pb(perm[i]);
        }
        vi col(n);
        for (int &c : col) c = stress::rnd(0, C - 1);
        vi par(n, -1), dep(n, 0), bq = {root};
        vector<char> seen(n, 0);
        seen[root] = 1;
        for (int i = 0; i < sz(bq); i++)
            for (int v : g[bq[i]]) if (!seen[v]) seen[v] = 1, par[v] = bq[i], dep[v] = dep[bq[i]] + 1, bq.pb(v);
        vector<pii> qs(q);
        for (auto &[u, v] : qs) u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
        vector<char> in(n, 0);
        vi cnt(C, 0);
        int distinct = 0, qi = 0;
        auto toggle = [&](int x) {
            if (in[x] ^= 1) distinct += cnt[col[x]]++ == 0;
            else distinct -= --cnt[col[x]] == 0;
        };
        vector<set<int>> seen_sets;
        auto get = [&] {
            set<int> s;
            for (int x = 0; x < n; x++) if (in[x]) s.insert(x);
            seen_sets.pb(s);
            qi++;
            return (ll)distinct;
        };
        vl res = mo_tree(g, root, qs, toggle, get);
        // every recorded set must be some query's path; check per query
        for (int i = 0; i < q; i++) {
            auto [u, v] = qs[i];
            set<int> path;
            int a = u, b = v;
            while (dep[a] > dep[b]) path.insert(a), a = par[a];
            while (dep[b] > dep[a]) path.insert(b), b = par[b];
            while (a != b) path.insert(a), path.insert(b), a = par[a], b = par[b];
            path.insert(a);
            set<int> cs;
            for (int x : path) cs.insert(col[x]);
            CHECK(res[i] == sz(cs), "distinct on path " + to_string(u) + " " + to_string(v));
            CHECK(count(all(seen_sets), path), "structure == path at get()");
        }
        CHECK(qi == q, "one get per query");
    }
    puts("OK");
}
