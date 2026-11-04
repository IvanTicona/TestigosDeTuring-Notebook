// Stress test for src/ds/dsu.cpp
// Oracles: explicit component labels relabeled on every union (O(n) per
// op) for DSU; a stack of full snapshots of those labels for rollback;
// absolute potentials kept explicitly (shifting a whole component on
// union) for WeightedDSU, including contradicting constraints.
#include "tests/stress/template_lib.h"
#include "src/ds/dsu.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_dsu, "src/ds/dsu.cpp", "example_dsu");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 40);
        DSU d(n);
        DSURollback r(n);
        vi lab(n), rl(n);
        iota(all(lab), 0), iota(all(rl), 0);
        vector<pair<int, vi>> snaps;  // (time, labels)
        auto comps = [&](const vi &l) { return sz(set<int>(all(l))); };
        for (int op = 0; op < 200; op++) {
            int a = stress::rnd(0, n - 1), b = stress::rnd(0, n - 1), ty = stress::rnd(0, 9);
            if (ty < 5) {
                bool want = lab[a] != lab[b];
                CHECK(d.unite(a, b) == want, "unite result");
                int la = lab[a], lb = lab[b];
                for (int &x : lab) if (x == lb) x = la;
                bool w2 = rl[a] != rl[b];
                CHECK(r.unite(a, b) == w2, "rollback unite result");
                int ra = rl[a], rb = rl[b];
                for (int &x : rl) if (x == rb) x = ra;
            } else if (ty < 7) {
                snaps.pb({r.time(), rl});
            } else if (ty < 8 && !snaps.empty()) {
                int k = stress::rnd(0, sz(snaps) - 1);
                r.rollback(snaps[k].F), rl = snaps[k].S;
                snaps.resize(k);
            }
            CHECK(d.same(a, b) == (lab[a] == lab[b]), "same");
            CHECK(d.size(a) == count(all(lab), lab[a]), "size");
            CHECK(d.comps == comps(lab), "comps");
            CHECK((r.find(a) == r.find(b)) == (rl[a] == rl[b]) && r.comps == comps(rl), "rollback state");
        }
    }
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 30);
        WeightedDSU w(n);
        vi lab(n);
        vl val(n, 0);  // explicit potentials, consistent within a component
        iota(all(lab), 0);
        for (int op = 0; op < 150; op++) {
            int a = stress::rnd(0, n - 1), b = stress::rnd(0, n - 1);
            if (stress::rnd(0, 1)) {
                ll dlt = lab[a] == lab[b] && stress::rnd(0, 1) ? val[b] - val[a] : stress::rnd(-50, 50);
                bool ok = lab[a] != lab[b] || val[b] - val[a] == dlt;
                CHECK(w.unite(a, b, dlt) == ok, "weighted unite result");
                if (lab[a] != lab[b]) {  // shift b's component so val[b] - val[a] = dlt
                    ll sh = val[a] + dlt - val[b];
                    int lb = lab[b];
                    for (int i = 0; i < n; i++) if (lab[i] == lb) lab[i] = lab[a], val[i] += sh;
                }
            } else if (lab[a] == lab[b]) {
                CHECK(w.diff(a, b) == val[b] - val[a], "diff");
            }
        }
    }
    // deep chains: no recursion issues for DSU / rollback at 2e5
    int n = 200000;
    DSU d(n);
    DSURollback r(n);
    for (int i = 1; i < n; i++) d.unite(i - 1, i), r.unite(i - 1, i);
    CHECK(d.comps == 1 && d.size(0) == n && r.comps == 1, "big chain");
    r.rollback(0);
    CHECK(r.comps == n && r.find(n - 1) == n - 1, "big rollback");
    puts("OK");
}
