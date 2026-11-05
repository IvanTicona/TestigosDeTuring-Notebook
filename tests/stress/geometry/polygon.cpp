// Stress test for src/geometry/polygon.cpp
// Oracles: random simple polygons (sorted by angle around an interior
// point, small integer grid); point location by the winding number (sum
// of signed angles) and boundary check by on_segment; in_convex on
// convex hulls vs in_polygon; Pick counts vs scanning all lattice points;
// cut_convex: both sides add up to the area, every vertex on its side
// and inside the polygon; centroid of rectangles / triangles in closed form.
#include "tests/stress/template_lib.h"
#include "src/geometry/point.cpp"
#include "src/geometry/segments.cpp"
#include "src/geometry/polygon.cpp"
#include "tests/stress/common.h"

typedef Pt<ll> P;

int main() {
    stress::check_example(example_polygon, "src/geometry/polygon.cpp", "example_polygon");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        // star-shaped polygon around (0, 0): distinct angles, radius random
        int n = stress::rnd(3, 9);
        vector<P> poly;
        set<pair<ll, ll>> dirs;
        while (sz(poly) < n) {
            ll x = stress::rnd(-6, 6), y = stress::rnd(-6, 6);
            if (!x && !y) continue;
            ll g = __gcd(llabs(x), llabs(y));
            if (!dirs.insert({x / g, y / g}).second) continue;
            poly.pb({x, y});
        }
        sort(all(poly), angle_less<ll>);
        bool simple = true;  // every angular gap < pi -> simple polygon
        for (int i = 0; i < n; i++)
            if (poly[i].cross(poly[(i + 1) % n]) <= 0) simple = false;  // angle gap >= pi
        if (!simple) continue;
        if (stress::rnd(0, 1)) reverse(all(poly));  // CW too
        ll a2 = area2(poly);
        ll I = 0, B = 0;
        for (ll x = -7; x <= 7; x++)
            for (ll y = -7; y <= 7; y++) {
                P q{x, y};
                bool on = false;
                for (int i = 0; i < n; i++) on |= on_segment(poly[i], poly[(i + 1) % n], q);
                // winding number by summing signed angles
                ld w = 0;
                for (int i = 0; i < n; i++) {
                    Pt<ld> u = to_ld(poly[i] - q), v = to_ld(poly[(i + 1) % n] - q);
                    w += atan2l(u.cross(v), u.dot(v));
                }
                int want = on ? 0 : (fabsl(w) > PI ? 1 : -1);
                CHECK(in_polygon(poly, q) == want, "in_polygon");
                I += want == 1, B += want == 0;
            }
        auto [li, lb] = lattice(poly);
        CHECK(li == I && lb == B, "Pick");
        CHECK(llabs(a2) == 2 * I + B - 2, "area via Pick");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        // convex polygon: lattice points on a circle-ish set, hull by angle
        int k = stress::rnd(3, 10);
        vector<P> pts;
        for (int i = 0; i < k; i++) pts.pb({stress::rnd(-6, 6), stress::rnd(-6, 6)});
        // monotone chain hull (independent of the snippet)
        sort(all(pts)), pts.erase(unique(all(pts)), pts.end());
        if (sz(pts) < 3) continue;
        vector<P> h(2 * sz(pts));
        int m = 0;
        for (int it = 0; it < 2; it++, reverse(all(pts))) {
            int st = m;
            for (auto p : pts) {
                while (m >= st + 2 && h[m - 2].cross(h[m - 1], p) <= 0) m--;
                h[m++] = p;
            }
            m--;
        }
        h.resize(m);
        if (m < 3) continue;
        for (int t = 0; t < 40; t++) {
            P q{stress::rnd(-7, 7), stress::rnd(-7, 7)};
            CHECK(in_convex(h, q) == in_polygon(h, q), "in_convex");
        }
        P a{stress::rnd(-7, 7), stress::rnd(-7, 7)}, b{stress::rnd(-7, 7), stress::rnd(-7, 7)};
        if (a == b) continue;
        auto L = cut_convex(h, a, b), R = cut_convex(h, b, a);
        auto ar = [](const vector<Pt<ld>> &v) { return sz(v) >= 3 ? fabsl(area2(v)) / 2 : 0.0L; };
        CHECK(fabsl(ar(L) + ar(R) - fabsl((ld)area2(h)) / 2) < 1e-9, "pieces add up");
        for (auto &[piece, sign] : vector<pair<vector<Pt<ld>>, int>>{{L, 1}, {R, -1}})
            for (auto q : piece) {
                ld side = (to_ld(b) - to_ld(a)).cross(q - to_ld(a)) * sign;
                CHECK(side > -1e-9, "vertex on the kept side");
                for (int i = 0; i < m; i++)
                    CHECK((to_ld(h[(i + 1) % m]) - to_ld(h[i])).cross(q - to_ld(h[i])) > -1e-9, "vertex inside hull");
            }
    }
    // centroids in closed form
    vector<P> rect = {{1, 2}, {7, 2}, {7, 5}, {1, 5}};
    Pt<ld> c = centroid(rect);
    CHECK(fabsl(c.x - 4) < 1e-12 && fabsl(c.y - 3.5L) < 1e-12, "rectangle centroid");
    vector<P> tri = {{0, 0}, {6, 0}, {0, 3}};
    c = centroid(tri);
    CHECK(fabsl(c.x - 2) < 1e-12 && fabsl(c.y - 1) < 1e-12, "triangle centroid");
    puts("OK");
}
