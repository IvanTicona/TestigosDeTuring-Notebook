// Stress test for src/graphs/two_sat.cpp
// Oracle: try all 2^n assignments (n <= 12) against random clauses
// (either / imply / force / at_most_one groups); the returned assignment
// must satisfy every constraint.
#include "tests/stress/template_lib.h"
#include "src/graphs/scc.cpp"
#include "src/graphs/two_sat.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_two_sat, "src/graphs/two_sat.cpp", "example_two_sat");
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int n = stress::rnd(1, 12), m = stress::rnd(0, 3 * n);
        TwoSat ts(n);
        vector<array<int, 3>> cl;  // {type, a, b}: 0 either, 1 imply, 2 force
        vector<vi> amo;
        auto rlit = [&] { return TwoSat::lit(stress::rnd(0, n - 1), stress::rnd(0, 1)); };
        for (int i = 0; i < m; i++) {
            int ty = stress::rnd(0, 5);
            if (ty == 3) {
                vi ls;
                for (int k = stress::rnd(0, 4); k--;) ls.pb(rlit());
                ts.at_most_one(ls), amo.pb(ls);
                continue;
            }
            int a = rlit(), b = rlit();
            ty = min(ty, 2);
            if (ty == 0) ts.either(a, b);
            else if (ty == 1) ts.imply(a, b);
            else ts.force(a);
            cl.pb({ty, a, b});
        }
        auto holds = [](int l, int mask) { return (mask >> (l / 2) & 1) == !(l & 1); };
        auto good = [&](int mask) {
            for (auto [ty, a, b] : cl) {
                if (ty == 0 && !holds(a, mask) && !holds(b, mask)) return false;
                if (ty == 1 && holds(a, mask) && !holds(b, mask)) return false;
                if (ty == 2 && !holds(a, mask)) return false;
            }
            for (auto &ls : amo) {
                int c = 0;
                for (int l : ls) c += holds(l, mask);
                if (c > 1) return false;
            }
            return true;
        };
        bool any = false;
        for (int mask = 0; mask < (1 << n) && !any; mask++) any = good(mask);
        vb val;
        bool ok = ts.solve(val);
        CHECK(ok == any, "satisfiable n=" + to_string(n));
        if (ok) {
            int mask = 0;
            for (int x = 0; x < n; x++) mask |= val[x] << x;
            CHECK(good(mask), "assignment satisfies");
        }
    }
    puts("OK");
}
