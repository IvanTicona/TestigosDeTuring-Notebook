// Stress test for src/geometry/minkowski.cpp
// Oracle: convex hull (monotone chain, convex_hull.cpp) of all pairwise
// sums a + b; the result must be the same vertex set (as a cyclic CCW
// sequence) for random convex polygons, including segments-like thin
// polygons and parallel edges.
#include "tests/stress/template_lib.h"
#include "src/geometry/point.cpp"
#include "src/geometry/convex_hull.cpp"
#include "src/geometry/minkowski.cpp"
#include "tests/stress/common.h"

typedef Pt<ll> P;

int main() {
    stress::check_example(example_minkowski, "src/geometry/minkowski.cpp", "example_minkowski");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        auto rnd_convex = [&]() {
            vector<P> pts;
            int k = stress::rnd(3, 10);
            ll C = stress::rnd(1, 10);
            for (int i = 0; i < k; i++) pts.pb({stress::rnd(-C, C), stress::rnd(-C, C)});
            if (stress::rnd(0, 3) == 0) pts = {{0, 0}, {C, 0}, {C, C}, {0, C}};  // parallel edges
            return hull(pts);
        };
        vector<P> A = rnd_convex(), B = rnd_convex();
        if (sz(A) < 3 || sz(B) < 3) continue;
        vector<P> sums;
        for (auto a : A) for (auto b : B) sums.pb(a + b);
        vector<P> want = hull(sums), got = minkowski(A, B);
        CHECK(sz(got) == sz(want), "size");
        int k = find(all(got), want[0]) - got.begin();
        CHECK(k < sz(got), "start vertex present");
        for (int i = 0; i < sz(want); i++) CHECK(got[(k + i) % sz(got)] == want[i], "same CCW cycle");
    }
    puts("OK");
}
