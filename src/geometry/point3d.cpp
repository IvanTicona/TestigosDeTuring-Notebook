/**
 * Usage: P3<ll> / P3<ld>: + - * /, dot, cross, norm2, len
 *        vol6(a, b, c, d)        -> 6 * signed volume of tetrahedron (exact
 *                                   for ll; > 0 if d is on the side of
 *                                   (b - a) x (c - a))
 *        plane_dist(a, b, c, p)  -> signed distance from p to plane abc
 *        line_plane(a, b, c, p, q) -> {1, X} point where line pq meets the
 *                                   plane, {0, _} if parallel
 *        poly_volume(faces)      -> volume of a closed polyhedron, faces as
 *                                   vertex lists oriented outwards (CCW
 *                                   seen from outside)
 *        sphere_dist(lat1, lon1, lat2, lon2, R) -> great-circle distance
 *                                   (radians)
 * Complexity: O(1); poly_volume O(total vertices)
 * Source: UPB Geom3D (Java) + UMSS Some Formulas (glass volume); own C++
 * Note: 3D convex hull (O(n^2) incremental) is not included. Point in
 *       tetrahedron: four vol6 with the same sign. Coplanar check: vol6 = 0.
 * Status: untested
 */
template <class T>
struct P3 {
    T x = 0, y = 0, z = 0;
    P3 operator+(P3 o) const { return {x + o.x, y + o.y, z + o.z}; }
    P3 operator-(P3 o) const { return {x - o.x, y - o.y, z - o.z}; }
    P3 operator*(T k) const { return {x * k, y * k, z * k}; }
    P3 operator/(T k) const { return {x / k, y / k, z / k}; }
    T dot(P3 o) const { return x * o.x + y * o.y + z * o.z; }
    P3 cross(P3 o) const {
        return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x};
    }
    T norm2() const { return dot(*this); }
    ld len() const { return sqrtl((ld)norm2()); }
};

template <class T>
T vol6(P3<T> a, P3<T> b, P3<T> c, P3<T> d) {
    return (b - a).cross(c - a).dot(d - a);
}

template <class T>
ld plane_dist(P3<T> a, P3<T> b, P3<T> c, P3<T> p) {
    P3<T> n = (b - a).cross(c - a);
    return (ld)n.dot(p - a) / n.len();
}

template <class T>
pair<int, P3<ld>> line_plane(P3<T> a, P3<T> b, P3<T> c, P3<T> p, P3<T> q) {
    P3<T> n = (b - a).cross(c - a);
    T den = n.dot(q - p);
    if (sgn(den) == 0) return {0, {}};
    ld t = (ld)n.dot(a - p) / (ld)den;
    auto L = [](P3<T> v) { return P3<ld>{(ld)v.x, (ld)v.y, (ld)v.z}; };
    return {1, L(p) + L(q - p) * t};
}

template <class T>
ld poly_volume(const vector<vector<P3<T>>> &faces) {
    ld v = 0;  // sum of signed tetrahedra (origin, triangle fan)
    for (auto &f : faces)
        for (int i = 1; i + 1 < sz(f); i++)
            v += (ld)f[0].cross(f[i]).dot(f[i + 1]);
    return v / 6;
}

ld sphere_dist(ld la1, ld lo1, ld la2, ld lo2, ld R) {  // haversine
    ld a = sinl((la2 - la1) / 2), b = sinl((lo2 - lo1) / 2);
    ld h = a * a + cosl(la1) * cosl(la2) * b * b;
    return 2 * R * asinl(min((ld)1, sqrtl(h)));
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_point3d() {
    P3<ll> o{0, 0, 0}, x{1, 0, 0}, y{0, 1, 0}, z{0, 0, 1};
    ll d = llround(plane_dist(o, x, y, P3<ll>{5, 5, 7}));  // height over z = 0
    cout << vol6(o, x, y, z) << ' ' << d << '\n';  // 1 7
    // line through (1,1,4) and (3,5,2) reaches z = 0 at t = 2
    auto [k, X] = line_plane(o, x, y, P3<ll>{1, 1, 4}, P3<ll>{3, 5, 2});
    cout << k << ' ' << llround(X.x) << llround(X.y) << llround(X.z)  // 1 590
         << '\n';
    cout << to_string(sphere_dist(0, 0, 0, PI / 2, 1)) << '\n';  // 1.570796
}
