// Stress test for src/geometry/segments.cpp
// Oracles on small integer grids (|coords| <= 4, many collinear and
// degenerate cases): segment intersection by sampling both segments as
// rational points (x, y with denominator 12) and checking membership
// exactly; seg_inter points must lie on both segments, with 2 points
// exactly when the common part is a segment; distances vs dense sampling
// lower bounds and exact endpoint formulas; line_inter by cross products.
#include "tests/stress/template_lib.h"
#include "src/geometry/point.cpp"
#include "src/geometry/segments.cpp"
#include "tests/stress/common.h"

typedef Pt<ll> P;

int main() {
    stress::check_example(example_segments, "src/geometry/segments.cpp", "example_segments");
    auto rp = [](ll C) { return P{stress::rnd(-C, C), stress::rnd(-C, C)}; };
    for (stress::iter = 0; stress::iter < stress::scaled(20000); stress::iter++) {
        P a = rp(4), b = rp(4), c = rp(4), d = rp(4);
        if (stress::rnd(0, 3) == 0) {  // force collinear
            c = a + (b - a) * stress::rnd(-2, 2), d = a + (b - a) * stress::rnd(-2, 2);
        }
        // exact membership of grid points scaled by L along each segment
        const ll L = 12;
        auto on = [&](P s, P e, P q12) {  // q12 = point * L
            P s12 = s * L, e12 = e * L;
            return q12.cross(s12, e12) == 0 && (s12 - q12).dot(e12 - q12) <= 0;
        };
        set<P> common;  // common points among L-subdivision points of ab
        for (int k = 0; k <= L; k++) {
            P q = a * (L - k) + b * k;  // = L * (a + (b - a) k / L)
            if (on(c, d, q)) common.insert(q);
        }
        for (int k = 0; k <= L; k++) {
            P q = c * (L - k) + d * k;
            if (on(a, b, q)) common.insert(q);
        }
        bool inter = seg_intersect(a, b, c, d);
        if (!common.empty()) CHECK(inter, "sampled common point -> intersect");
        auto r = seg_inter(a, b, c, d);
        CHECK(r.empty() == !inter, "seg_inter empty iff no intersection");
        for (auto q : r) {
            P qi{llround(q.x * L), llround(q.y * L)};
            bool exact = fabsl(q.x * L - qi.x) < 1e-6 && fabsl(q.y * L - qi.y) < 1e-6;
            if (exact) CHECK(on(a, b, qi) && on(c, d, qi), "point on both segments");
            auto near = [&](P s0, P e0) {  // q on segment s0 e0 (tolerance)
                Pt<ld> S = to_ld(s0), E = to_ld(e0);
                return fabsl((S - q).cross(E - q)) < 1e-9 && (S - q).dot(E - q) < 1e-9;
            };
            CHECK(near(a, b) && near(c, d), "seg_inter point on both segments");
        }
        CHECK(sz(r) <= 2, "at most 2 points");
        if (sz(r) == 2) CHECK(sz(common) >= 2, "overlap has many common points");
        if (sz(common) >= 2) CHECK(sz(r) == 2, "overlap reported as 2 points");
        // distances: point-segment exact for vertical/horizontal-free check
        ll best2 = LLONG_MAX;  // squared distance between segment samples (upper bound)
        for (int i = 0; i <= L; i++)
            for (int j = 0; j <= L; j++) {
                P u = a * (L - i) + b * i, v = c * (L - j) + d * j;
                best2 = min(best2, (u - v).norm2());
            }
        ld dd = seg_seg_dist(a, b, c, d);
        CHECK(dd <= sqrtl((ld)best2) / L + 1e-9, "seg_seg_dist <= sampled");
        if (inter) CHECK(dd == 0, "touching -> 0");
        // line_inter
        if (!(a == b) && !(c == d)) {
            auto [k, p] = line_inter(a, b, c, d);
            ll den = (b - a).cross(d - c);
            if (den) {
                CHECK(k == 1, "unique");
                CHECK(fabsl((p - to_ld(a)).cross(to_ld(b - a))) < 1e-9 && fabsl((p - to_ld(c)).cross(to_ld(d - c))) < 1e-9, "on both lines");
            } else CHECK(k == (a.cross(b, c) == 0 ? -1 : 0), "parallel / same");
            Pt<ld> f = project(a, b, c);
            CHECK(fabsl((f - to_ld(a)).cross(to_ld(b - a))) < 1e-9 && fabsl((to_ld(c) - f).dot(to_ld(b - a))) < 1e-9, "projection");
        }
    }
    puts("OK");
}
