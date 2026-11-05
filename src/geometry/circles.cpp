/**
 * Usage: all with Pt<ld>, radii ld
 *        circle_line(c, r, a, b)     -> intersections with the LINE ab
 *        circle_circle(c1, r1, c2, r2) -> 0, 1 or 2 points ({} if equal)
 *        tangents(c, r, p)           -> tangent points from p (outside)
 *        circumcircle(a, b, c)       -> {center, radius} (non-collinear)
 *        mec(pts)                    -> minimum enclosing circle (Welzl)
 *        lens_area(c1, r1, c2, r2)   -> area of the intersection of disks
 * Complexity: O(1); mec expected O(n) (random shuffle)
 * Source: UPB Geometry.cpp (CircleLineIntersection, CircleCircle) + UTP
 *         center from 2 points + radius, triangles + UPB JavaGeometry;
 *         mec, tangents and lens: own
 * Note: inscribed circle of a triangle: center = (a|BC| + b|CA| + c|AB|)
 *       / perimeter, r = 2 area / perimeter. Area of a circle cut by a
 *       polygon: triangle-wise (not here).
 * Depends: src/geometry/point.cpp
 * Status: ok
 */
typedef Pt<ld> PD;

vector<PD> circle_line(PD c, ld r, PD a, PD b) {
    PD d = b - a, h = a + d * ((c - a).dot(d) / d.norm2());  // h: foot from c
    ld s = r * r - (h - c).norm2();
    if (s < -EPS) return {};
    if (s < EPS) return {h};
    PD off = d * (sqrtl(s) / d.len());
    return {h - off, h + off};
}

vector<PD> circle_circle(PD c1, ld r1, PD c2, ld r2) {
    PD d = c2 - c1;
    ld d2 = d.norm2(), dl = sqrtl(d2);
    if (dl < EPS || dl > r1 + r2 + EPS || dl < fabsl(r1 - r2) - EPS) return {};
    ld a = (d2 + r1 * r1 - r2 * r2) / (2 * dl), h2 = r1 * r1 - a * a;
    PD m = c1 + d * (a / dl);
    if (h2 < EPS * max((ld)1, r1)) return {m};
    PD off = d.perp() * (sqrtl(h2) / dl);
    return {m - off, m + off};
}

vector<PD> tangents(PD c, ld r, PD p) {  // points of tangency
    ld d2 = (p - c).norm2();
    if (d2 < r * r - EPS) return {};
    return circle_circle(c, r, p, sqrtl(max((ld)0, d2 - r * r)));
}

pair<PD, ld> circumcircle(PD a, PD b, PD c) {
    PD u = b - a, v = c - a;
    ld d = 2 * u.cross(v);
    ld nu = u.norm2(), nv = v.norm2();
    PD o = a + PD{v.y * nu - u.y * nv, u.x * nv - v.x * nu} / d;
    return {o, (o - a).len()};
}

pair<PD, ld> mec(vector<PD> p) {
    shuffle(all(p), rng);
    PD o = p[0];
    ld r = 0, e = 1 + 1e-12L;
    for (int i = 0; i < sz(p); i++)
        if ((p[i] - o).len() > r * e) {
            o = p[i], r = 0;
            for (int j = 0; j < i; j++)
                if ((p[j] - o).len() > r * e) {
                    o = (p[i] + p[j]) / 2, r = (p[i] - o).len();
                    for (int k = 0; k < j; k++)
                        if ((p[k] - o).len() > r * e)
                            tie(o, r) = circumcircle(p[i], p[j], p[k]);
                }
        }
    return {o, r};
}

ld lens_area(PD c1, ld r1, PD c2, ld r2) {
    ld d = (c2 - c1).len();
    if (d >= r1 + r2) return 0;
    if (d <= fabsl(r1 - r2)) return PI * min(r1, r2) * min(r1, r2);
    auto ac = [](ld x) { return acosl(max((ld)-1, min((ld)1, x))); };
    ld a1 = 2 * ac((d * d + r1 * r1 - r2 * r2) / (2 * d * r1));  // clamp:
    ld a2 = 2 * ac((d * d + r2 * r2 - r1 * r1) / (2 * d * r2));  // NaN-safe
    return (r1 * r1 * (a1 - sinl(a1)) + r2 * r2 * (a2 - sinl(a2))) / 2;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_circles() {
    auto pl = circle_line(PD{0, 0}, 5, PD{-10, 3}, PD{10, 3});  // x = +-4
    ll x0 = llround(pl[0].x), x1 = llround(pl[1].x);
    cout << sz(pl) << ' ' << x0 << ' ' << x1 << '\n';  // 2 -4 4
    auto cc = circle_circle(PD{0, 0}, 5, PD{8, 0}, 5);  // meet at (4, +-3)
    ll cx = llround(cc[0].x), cy = llround(fabsl(cc[0].y));
    cout << sz(cc) << ' ' << cx << cy << '\n';  // 2 43
    auto [o, r] = mec({{0, 0}, {4, 0}, {0, 3}, {1, 1}});  // right triangle
    string out = to_string(o.x) + " " + to_string(o.y) + " " + to_string(r);
    cout << out << '\n';  // 2.000000 1.500000 2.500000
}
