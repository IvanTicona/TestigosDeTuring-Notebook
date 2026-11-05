// Stress test for src/geometry/closest_pair.cpp
// Oracle: all pairs O(n^2) (duplicates, all points on one vertical line,
// coordinates up to 1e9 / 2); returned indices must be distinct and
// realize the minimum; 2e5 points on one vertical line (old UMSS TLE case)
// against the exact answer.
#include "tests/stress/template_lib.h"
#include "src/geometry/point.cpp"
#include "src/geometry/closest_pair.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_closest_pair, "src/geometry/closest_pair.cpp", "example_closest_pair");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        int n = stress::rnd(2, 60);
        ll C = stress::rnd(0, 2) == 0 ? 3 : stress::rnd(0, 1) ? 1000 : (ll)5e8;
        bool vertical = stress::rnd(0, 4) == 0;
        vector<Pt<ll>> p(n);
        for (auto &q : p) q = {vertical ? 7 : stress::rnd(-C, C), stress::rnd(-C, C)};
        ll best = LLONG_MAX;
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++) best = min(best, (p[i] - p[j]).norm2());
        auto [d2, ij] = closest_pair(p);
        CHECK(d2 == best, "closest distance");
        CHECK(ij.F != ij.S && (p[ij.F] - p[ij.S]).norm2() == d2, "indices");
    }
    int n = stress::SLOW ? 20000 : 200000;
    vector<Pt<ll>> p(n);
    for (int i = 0; i < n; i++) p[i] = {5, 3LL * i - (i == n / 2)};  // gaps 3, one 2
    auto [d2, ij] = closest_pair(p);
    CHECK(d2 == 4, "vertical line");
    puts("OK");
}
