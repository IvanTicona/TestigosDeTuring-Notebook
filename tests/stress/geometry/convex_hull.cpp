// Stress test for src/geometry/convex_hull.cpp
// Oracles: a point is a hull vertex iff it is not inside / on the segment
// of any triangle / pair of the other points (O(n^4) brute force on small
// grids with many duplicates and collinear points); the hull must be CCW,
// strictly convex and contain every point; diameter = max over all pairs.
#include "tests/stress/template_lib.h"
#include "src/geometry/point.cpp"
#include "src/geometry/convex_hull.cpp"
#include "tests/stress/common.h"

typedef Pt<ll> P;

int main() {
    stress::check_example(example_convex_hull, "src/geometry/convex_hull.cpp", "example_convex_hull");
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int n = stress::rnd(0, 12);
        ll C = stress::rnd(0, 1) ? 2 : (ll)1e9 / 2;
        vector<P> pts(n);
        for (auto &p : pts) p = {stress::rnd(-C, C), stress::rnd(-C, C)};
        if (stress::rnd(0, 3) == 0 && n) for (auto &p : pts) p = {p.x, p.x * 2};  // collinear
        auto h = hull(pts);
        set<P> uniq(all(pts));
        vector<P> u(all(uniq));
        // brute force extreme points: not a convex combination of others
        auto in_tri = [](P a, P b, P c, P q) {
            ll s1 = a.cross(b, q), s2 = b.cross(c, q), s3 = c.cross(a, q);
            bool neg = s1 < 0 || s2 < 0 || s3 < 0, pos = s1 > 0 || s2 > 0 || s3 > 0;
            return !(neg && pos);
        };
        auto on_seg = [](P a, P b, P q) { return a.cross(b, q) == 0 && (a - q).dot(b - q) <= 0; };
        set<P> ext;
        for (auto q : u) {
            bool inside = false;
            for (auto a : u) for (auto b : u) {
                if (a == q || b == q) continue;
                if (on_seg(a, b, q)) inside = true;
                for (auto c : u) if (!(c == q) && a.cross(b, c) != 0 && in_tri(a, b, c, q)) inside = true;
            }
            if (!inside) ext.insert(q);
        }
        CHECK(set<P>(all(h)) == ext && sz(h) == sz(ext), "hull vertices");
        int m = sz(h);
        if (m >= 3) {
            for (int i = 0; i < m; i++)
                CHECK(h[i].cross(h[(i + 1) % m], h[(i + 2) % m]) > 0, "strictly convex CCW");
            for (auto q : u)
                for (int i = 0; i < m; i++) CHECK(h[i].cross(h[(i + 1) % m], q) >= 0, "contains all");
        }
        ll best = 0;
        for (auto a : u) for (auto b : u) best = max(best, (a - b).norm2());
        CHECK(diameter2(h).F == best, "diameter");
    }
    puts("OK");
}
