// Stress test for src/flows/hungarian.cpp
// Oracle: try every injective assignment of rows to columns (n <= m <= 7)
// with negative costs and huge costs (1e12); the returned assignment must
// be injective and achieve the reported cost.
#include "tests/stress/template_lib.h"
#include "src/flows/hungarian.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_hungarian, "src/flows/hungarian.cpp", "example_hungarian");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int m = stress::rnd(1, 7), n = stress::rnd(1, m);
        ll V = stress::rnd(0, 1) ? 10 : (ll)1e12;
        vector<vl> a(n, vl(m));
        for (auto &r : a) for (ll &x : r) x = stress::rnd(-V, V);
        vi cols(m);
        iota(all(cols), 0);
        ll best = INFL;
        do {  // first n columns of each permutation
            ll c = 0;
            for (int i = 0; i < n; i++) c += a[i][cols[i]];
            best = min(best, c);
        } while (next_permutation(all(cols)));
        auto [c, mt] = hungarian(a);
        CHECK(c == best, "cost n=" + to_string(n) + " m=" + to_string(m));
        CHECK(sz(set<int>(all(mt))) == n, "injective");
        ll s = 0;
        for (int i = 0; i < n; i++) s += a[i][mt[i]];
        CHECK(s == c, "assignment cost");
    }
    puts("OK");
}
