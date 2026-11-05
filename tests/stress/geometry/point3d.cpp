// Stress test for src/geometry/point3d.cpp
// Oracles: 3x3 determinant expanded by hand (__int128) for vol6; distance
// to plane vs projecting with the normalized normal; the line-plane point
// lies on both; volumes of random boxes (6 outward faces, translated) and
// tetrahedra (4 faces) vs closed forms; haversine vs chord formula
// 2 R asin(|u - v| / 2) on unit vectors.
#include "tests/stress/template_lib.h"
#include "src/geometry/point3d.cpp"
#include "tests/stress/common.h"

typedef P3<ll> Q;

int main() {
    stress::check_example(example_point3d, "src/geometry/point3d.cpp", "example_point3d");
    auto rq = [](ll C) { return Q{stress::rnd(-C, C), stress::rnd(-C, C), stress::rnd(-C, C)}; };
    for (stress::iter = 0; stress::iter < stress::scaled(5000); stress::iter++) {
        Q a = rq(1000), b = rq(1000), c = rq(1000), d = rq(1000);
        Q u = b - a, v = c - a, w = d - a;
        i128 det = (i128)u.x * (v.y * w.z - v.z * w.y) - (i128)u.y * (v.x * w.z - v.z * w.x) +
                   (i128)u.z * (v.x * w.y - v.y * w.x);
        CHECK((i128)vol6(a, b, c, d) == det, "vol6");
        Q n = u.cross(v);
        if (n.norm2() == 0) continue;
        ld want = ((ld)n.x * w.x + (ld)n.y * w.y + (ld)n.z * w.z) / sqrtl((ld)n.norm2());
        CHECK(fabsl(plane_dist(a, b, c, d) - want) < 1e-9 * max((ld)1, fabsl(want)), "plane_dist");
        Q p = rq(1000), q = rq(1000);
        auto [k, X] = line_plane(a, b, c, p, q);
        if (n.dot(q - p) == 0) CHECK(k == 0, "parallel");
        else {
            CHECK(k == 1, "hit");
            P3<ld> A{(ld)a.x, (ld)a.y, (ld)a.z}, N{(ld)n.x, (ld)n.y, (ld)n.z};
            CHECK(fabsl(N.dot(X - A)) < 1e-6 * N.len() * max((ld)1, (X - A).len()), "on plane");
            P3<ld> Pp{(ld)p.x, (ld)p.y, (ld)p.z}, D{(ld)(q - p).x, (ld)(q - p).y, (ld)(q - p).z};
            CHECK((X - Pp).cross(D).len() < 1e-6 * D.len() * max((ld)1, (X - Pp).len()), "on line");
        }
    }
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        ll L = stress::rnd(1, 50), W = stress::rnd(1, 50), H = stress::rnd(1, 50);
        Q s = rq(100);
        auto at = [&](int i, int j, int k) { return s + Q{i * L, j * W, k * H}; };
        vector<vector<Q>> box = {
            {at(0, 0, 0), at(0, 1, 0), at(1, 1, 0), at(1, 0, 0)},  // bottom (z = 0, normal -z)
            {at(0, 0, 1), at(1, 0, 1), at(1, 1, 1), at(0, 1, 1)},  // top
            {at(0, 0, 0), at(1, 0, 0), at(1, 0, 1), at(0, 0, 1)},  // y = 0 (normal -y)
            {at(0, 1, 0), at(0, 1, 1), at(1, 1, 1), at(1, 1, 0)},  // y = 1
            {at(0, 0, 0), at(0, 0, 1), at(0, 1, 1), at(0, 1, 0)},  // x = 0 (normal -x)
            {at(1, 0, 0), at(1, 1, 0), at(1, 1, 1), at(1, 0, 1)},  // x = 1
        };
        CHECK(fabsl(poly_volume(box) - (ld)L * W * H) < 1e-6, "box volume");
        ld la1 = stress::rnd(-1500, 1500) / 1000.0L, lo1 = stress::rnd(-3000, 3000) / 1000.0L;
        ld la2 = stress::rnd(-1500, 1500) / 1000.0L, lo2 = stress::rnd(-3000, 3000) / 1000.0L;
        auto vec = [](ld la, ld lo) { return P3<ld>{cosl(la) * cosl(lo), cosl(la) * sinl(lo), sinl(la)}; };
        ld chord = (vec(la1, lo1) - vec(la2, lo2)).len();
        CHECK(fabsl(sphere_dist(la1, lo1, la2, lo2, 3) - 6 * asinl(min((ld)1, chord / 2))) < 1e-9, "haversine");
    }
    puts("OK");
}
