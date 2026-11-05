// Stress test for src/geometry/circles.cpp
// Oracles: returned points satisfy the equations (on the circle(s) / line)
// and the count matches the distance classification; tangent points are
// on the circle with radius perpendicular to p - t; circumcircle is
// equidistant; mec contains all points and is not larger than the best
// circle through 2 or 3 points among all candidates (brute force O(n^4));
// lens area vs numeric integration over x of the overlap of two chords.
#include "tests/stress/template_lib.h"
#include "src/geometry/point.cpp"
#include "src/geometry/circles.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_circles, "src/geometry/circles.cpp", "example_circles");
    auto rd = [](ld C) { return (ld)stress::rnd(-1000, 1000) / 1000 * C; };
    for (stress::iter = 0; stress::iter < stress::scaled(4000); stress::iter++) {
        PD c{rd(10), rd(10)}, a{rd(10), rd(10)}, b{rd(10), rd(10)};
        ld r = fabsl(rd(10)) + 0.1L;
        if ((b - a).len() < 1e-3) continue;
        auto pl = circle_line(c, r, a, b);
        ld dist = fabsl((b - a).cross(c - a)) / (b - a).len();
        int want = dist < r - 1e-9 ? 2 : dist > r + 1e-9 ? 0 : -1;
        if (want >= 0) CHECK(sz(pl) == want, "circle_line count");
        for (auto q : pl)
            CHECK(fabsl((q - c).len() - r) < 1e-6 && fabsl((b - a).cross(q - a)) < 1e-6 * (b - a).len(), "circle_line point");
        PD c2{rd(10), rd(10)};
        ld r2 = fabsl(rd(10)) + 0.1L, d = (c2 - c).len();
        auto cc = circle_circle(c, r, c2, r2);
        bool sep = d > r + r2 + 1e-9 || d < fabsl(r - r2) - 1e-9, inter = d < r + r2 - 1e-9 && d > fabsl(r - r2) + 1e-9;
        if (sep) CHECK(cc.empty(), "circle_circle none");
        if (inter) CHECK(sz(cc) == 2, "circle_circle two");
        for (auto q : cc) CHECK(fabsl((q - c).len() - r) < 1e-6 && fabsl((q - c2).len() - r2) < 1e-6, "on both circles");
        PD p{rd(20), rd(20)};
        if ((p - c).len() > r + 1e-3) {
            auto t = tangents(c, r, p);
            CHECK(sz(t) == 2, "two tangents");
            for (auto q : t) CHECK(fabsl((q - c).len() - r) < 1e-6 && fabsl((q - c).dot(p - q)) < 1e-6 * max((ld)1, (p - c).len()), "tangent");
        }
        if (fabsl((b - a).cross(c - a)) > 1e-3) {
            auto [o, rr] = circumcircle(a, b, c);
            CHECK(fabsl((o - a).len() - rr) < 1e-6 && fabsl((o - b).len() - rr) < 1e-6 && fabsl((o - c).len() - rr) < 1e-6, "circumcircle");
        }
        // lens area by integrating the vertical overlap of the two disks
        ld lo = max(c.x - r, c2.x - r2), hi = min(c.x + r, c2.x + r2), area = 0;
        if (lo < hi) {
            int N = 4000;
            for (int i = 0; i < N; i++) {
                ld x = lo + (hi - lo) * (i + 0.5L) / N;
                ld h1 = sqrtl(max((ld)0, r * r - (x - c.x) * (x - c.x)));
                ld h2 = sqrtl(max((ld)0, r2 * r2 - (x - c2.x) * (x - c2.x)));
                ld top = min(c.y + h1, c2.y + h2), bot = max(c.y - h1, c2.y - h2);
                area += max((ld)0, top - bot) * (hi - lo) / N;
            }
        }
        CHECK(fabsl(lens_area(c, r, c2, r2) - area) < 1e-3 * max((ld)1, area), "lens area");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(800); stress::iter++) {
        int n = stress::rnd(1, 9);
        vector<PD> p(n);
        for (auto &q : p) q = {(ld)stress::rnd(-20, 20), (ld)stress::rnd(-20, 20)};
        auto [o, r] = mec(p);
        for (auto q : p) CHECK((q - o).len() <= r + 1e-7, "mec contains");
        ld best = n == 1 ? 0 : 1e18;  // brute force over 2- and 3-point circles
        auto covers = [&](PD cen, ld rad) {
            for (auto q : p) if ((q - cen).len() > rad + 1e-7) return false;
            return true;
        };
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++) {
                PD m = (p[i] + p[j]) / 2;
                if (covers(m, (p[i] - m).len())) best = min(best, (p[i] - m).len());
                for (int k = j + 1; k < n; k++)
                    if (fabsl((p[j] - p[i]).cross(p[k] - p[i])) > 1e-9) {
                        auto [cen, rad] = circumcircle(p[i], p[j], p[k]);
                        if (covers(cen, rad)) best = min(best, rad);
                    }
            }
        CHECK(fabsl(r - best) < 1e-7, "mec radius");
    }
    puts("OK");
}
