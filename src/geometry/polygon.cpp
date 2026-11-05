/**
 * Usage: polygon = vector<Pt<T>> in order (CW or CCW), no repeated last
 *        area2(poly)            -> 2 * signed area (> 0 if CCW), exact for ll
 *                                  while n * 2 max|coord|^2 < ~9e18
 *        in_polygon(poly, p)    -> 1 inside, 0 on the boundary, -1 outside
 *                                  (any simple polygon, O(n))
 *        in_convex(poly, p)     -> same for a CCW convex polygon (no 3
 *                                  collinear vertices), O(log n)
 *        lattice(poly)          -> {interior, boundary} lattice points (ll
 *                                  coordinates, Pick: A = I + B/2 - 1)
 *        cut_convex(poly, a, b) -> part of a convex polygon left of a -> b
 *        centroid(poly)         -> center of mass of the area (area != 0)
 * Complexity: O(n) unless noted
 * Source: UMSS Point In General Polygon (int cross, ints with EPS) + UMSS
 *         Point in Convex Polygon (CW only) + UPB Geometry.cpp + UMSS Some
 *         Formulas; Pick / cut / centroid: own
 * Note: winding-number style crossing test with half-open edges (handles
 *       vertices on the ray). Area of a union of polygons: sweep instead.
 * Depends: src/geometry/point.cpp
 * Depends: src/geometry/segments.cpp
 * Status: untested
 */
template <class T>
T area2(const vector<Pt<T>> &p) {
    T a = 0;
    for (int i = 0, n = sz(p); i < n; i++) a += p[i].cross(p[(i + 1) % n]);
    return a;
}

template <class T>
int in_polygon(const vector<Pt<T>> &p, Pt<T> q) {
    int n = sz(p), cnt = 0;
    for (int i = 0; i < n; i++) {
        Pt<T> a = p[i], b = p[(i + 1) % n];
        if (on_segment(a, b, q)) return 0;
        if ((a.y > q.y) != (b.y > q.y)) {  // edge crosses the horizontal line
            int s = sgn(a.cross(b, q));      // q left of a -> b ?
            if ((s > 0) == (b.y > a.y)) cnt ^= 1;
        }
    }
    return cnt ? 1 : -1;
}

template <class T>
int in_convex(const vector<Pt<T>> &p, Pt<T> q) {
    int n = sz(p);
    if (n < 3) return n && on_segment(p[0], p[n - 1], q) ? 0 : -1;
    if (sgn(p[0].cross(p[1], q)) < 0 || sgn(p[0].cross(p[n - 1], q)) > 0)
        return -1;  // outside the wedge at p[0]
    int lo = 1, hi = n - 1;  // find the triangle p0, p[lo], p[lo + 1]
    while (hi - lo > 1) {
        int md = (lo + hi) / 2;
        if (sgn(p[0].cross(p[md], q)) >= 0) lo = md;
        else hi = md;
    }
    int s = sgn(p[lo].cross(p[lo + 1], q));
    if (s < 0) return -1;
    if (s == 0 || on_segment(p[0], p[1], q) || on_segment(p[0], p[n - 1], q))
        return 0;
    return 1;
}

pll lattice(const vector<Pt<ll>> &p) {
    ll b = 0, n = sz(p);
    for (int i = 0; i < n; i++) {
        Pt<ll> d = p[(i + 1) % n] - p[i];
        b += __gcd(llabs(d.x), llabs(d.y));
    }
    ll a2 = llabs(area2(p));
    return {(a2 - b + 2) / 2, b};  // I = A - B/2 + 1
}

template <class T>
vector<Pt<ld>> cut_convex(const vector<Pt<T>> &p, Pt<T> a, Pt<T> b) {
    vector<Pt<ld>> r;
    int n = sz(p);
    for (int i = 0; i < n; i++) {
        Pt<T> c = p[i], d = p[(i + 1) % n];
        int sc = sgn(a.cross(b, c)), sd = sgn(a.cross(b, d));
        if (sc >= 0) r.pb(to_ld(c));
        if (sc * sd < 0) r.pb(line_inter(a, b, c, d).S);
    }
    return r;
}

template <class T>
Pt<ld> centroid(const vector<Pt<T>> &p) {
    Pt<ld> c{0, 0};
    ld a = 0;
    for (int i = 0, n = sz(p); i < n; i++) {
        Pt<ld> u = to_ld(p[i]), v = to_ld(p[(i + 1) % n]);
        ld w = u.cross(v);
        c = c + (u + v) * w, a += w;
    }
    return c / (3 * a);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_polygon() {
    vector<Pt<ll>> sq = {{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    int a = in_polygon(sq, Pt<ll>{2, 2}), b = in_polygon(sq, Pt<ll>{4, 1});
    int c = in_polygon(sq, Pt<ll>{5, 1});  // inside, on the border, outside
    cout << area2(sq) << ' ' << a << b << c << '\n';  // 32 10-1
    auto [inside, border] = lattice(sq);
    int k = in_convex(sq, Pt<ll>{1, 3});
    cout << inside << ' ' << border << ' ' << k << '\n';  // 9 16 1
    auto half = cut_convex(sq, Pt<ll>{0, 2}, Pt<ll>{4, 2});  // keep y >= 2
    cout << sz(half) << ' ' << llround(area2(half) / 2) << '\n';  // 4 8
}
