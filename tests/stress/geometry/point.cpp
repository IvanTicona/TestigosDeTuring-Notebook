// Stress test for src/geometry/point.cpp
// Oracles: component formulas recomputed by hand (__int128 for |coords| up
// to 1e9), rotation checked through length and angle preservation,
// angle_less against atan2 ordering on integer vectors (exact ties = same
// direction, compared with cross == 0 && dot > 0).
#include "tests/stress/template_lib.h"
#include "src/geometry/point.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_point, "src/geometry/point.cpp", "example_point");
    for (stress::iter = 0; stress::iter < stress::scaled(20000); stress::iter++) {
        ll C = stress::rnd(0, 1) ? 5 : (ll)1e9;
        Pt<ll> a{stress::rnd(-C, C), stress::rnd(-C, C)}, b{stress::rnd(-C, C), stress::rnd(-C, C)};
        Pt<ll> c{stress::rnd(-C, C), stress::rnd(-C, C)};
        if (C > 5) {  // keep differences within 1e9 for cross(a, b, c)
            a = a / 2, b = b / 2, c = c / 2;
        }
        CHECK(a.cross(b) == (ll)((i128)a.x * b.y - (i128)a.y * b.x), "cross");
        CHECK(a.dot(b) == (ll)((i128)a.x * b.x + (i128)a.y * b.y), "dot");
        i128 o = (i128)(b.x - a.x) * (c.y - a.y) - (i128)(b.y - a.y) * (c.x - a.x);
        CHECK((i128)a.cross(b, c) == o, "orientation");
        CHECK(a.perp().dot(a) == 0 && a.perp().cross(a) == -a.norm2(), "perp");
        Pt<ld> d{(ld)a.x, (ld)a.y};
        ld t = stress::rnd(-1000, 1000) / 100.0L;
        Pt<ld> r = d.rot(t);
        CHECK(fabsl(r.len() - d.len()) <= 1e-9L * max((ld)1, d.len()), "rotation keeps length");
        if (a.norm2() > 0) {
            ld da = remainderl(r.ang() - d.ang() - t, 2 * PI);
            CHECK(fabsl(da) < 1e-9L, "rotation angle");
            CHECK(fabsl(d.unit().len() - 1) < 1e-12L, "unit");
        }
        if (a.norm2() && b.norm2()) {
            bool same = a.cross(b) == 0 && a.dot(b) > 0;
            ld aa = atan2l((ld)a.y, (ld)a.x), ab = atan2l((ld)b.y, (ld)b.x);
            if (aa >= PI) aa -= 2 * PI;
            if (ab >= PI) ab -= 2 * PI;
            // angle_less orders by angle in [0, 2pi) from the positive x axis
            auto key = [](ld x) { return x < 0 ? x + 2 * PI : x; };
            if (same) CHECK(!angle_less(a, b) && !angle_less(b, a), "same direction tie");
            else CHECK(angle_less(a, b) == (key(aa) < key(ab)), "angle order");
        }
    }
    puts("OK");
}
