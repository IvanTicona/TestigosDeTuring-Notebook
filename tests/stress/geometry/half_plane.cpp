// Stress test for src/geometry/half_plane.cpp
// Oracle: start from the bounding box and clip it by each half-plane in
// input order (Sutherland-Hodgman, cut_convex from polygon.cpp: a
// different algorithm); compare areas, and every returned vertex must
// satisfy every half-plane; parallel and opposite half-planes included.
#include "tests/stress/template_lib.h"
#include "src/geometry/point.cpp"
#include "src/geometry/segments.cpp"
#include "src/geometry/polygon.cpp"
#include "src/geometry/half_plane.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_half_plane, "src/geometry/half_plane.cpp", "example_half_plane");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        int n = stress::rnd(1, 8);
        ld B = 100;
        vector<HP> hs;
        vector<pair<Pt<ld>, Pt<ld>>> lines;
        ld sc = vector<ld>{1, 1e-5, 1e-3, 1e4}[stress::rnd(0, 3)];  // |b - a|
        for (int i = 0; i < n; i++) {
            Pt<ld> a{(ld)stress::rnd(-10, 10), (ld)stress::rnd(-10, 10)};
            Pt<ld> b{(ld)stress::rnd(-10, 10), (ld)stress::rnd(-10, 10)};
            if (a == b) continue;
            if (stress::rnd(0, 4) == 0 && !lines.empty()) {  // parallel / opposite copy
                auto [p, q] = lines[0];
                ld sh = stress::rnd(-3, 3);
                a = p + Pt<ld>{sh, -sh}, b = a + (q - p) * (stress::rnd(0, 1) ? 1 : -1);
            }
            hs.pb(HP(a, a + (b - a) * sc)), lines.pb({a, b});
        }
        vector<Pt<ld>> poly = {{-B, -B}, {B, -B}, {B, B}, {-B, B}};
        for (auto [a, b] : lines) {
            if (sz(poly) < 3) break;
            poly = cut_convex(poly, a, b);
        }
        ld want = sz(poly) >= 3 ? fabsl(area2(poly)) / 2 : 0;
        auto got = hp_intersect(hs, B);
        ld ga = sz(got) >= 3 ? fabsl(area2(got)) / 2 : 0;
        CHECK(fabsl(ga - want) < 1e-6 * max((ld)1, want), "area " + to_string((double)ga) + " vs " + to_string((double)want));
        for (auto q : got)
            for (auto &h : hs) CHECK(h.d.cross(q - h.p) > -1e-6, "vertex satisfies all");
    }
    puts("OK");
}
