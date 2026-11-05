/**
 * Usage: Pt<ll> (exact, |coords| <= 1e9) or Pt<ld>; p + q, p - q, p * k,
 *        p / k, p.dot(q), p.cross(q), a.cross(b, c) = (b - a) x (c - a)
 *        (> 0: c is to the left of a -> b, counter-clockwise),
 *        p.norm2(), p.len(), p.ang() (atan2), p.perp() (rotate +90),
 *        p.rot(t) (ld, radians), p.unit(); p < q (x, then y)
 *        angle_less(a, b) -> sort vectors by angle in [-pi, pi) exactly
 * Complexity: O(1) per operation
 * Source: UPB Geometry.cpp (Stanford PT, doubles only) + UMSS Convex Hull
 *         pt (ll) + UPB JavaGeometry; template and angular sort: KACTL-like
 * Note: with ll keep |coords| <= 1e9 so cross products fit (< 9.2e18).
 *       sgn(x) from the template compares with EPS (also fine for ll).
 *       Angle between u, v: atan2(|u x v|, u . v).
 * Status: untested
 */
template <class T>
struct Pt {
    T x = 0, y = 0;
    Pt operator+(Pt o) const { return {x + o.x, y + o.y}; }
    Pt operator-(Pt o) const { return {x - o.x, y - o.y}; }
    Pt operator*(T k) const { return {x * k, y * k}; }
    Pt operator/(T k) const { return {x / k, y / k}; }
    bool operator<(Pt o) const { return tie(x, y) < tie(o.x, o.y); }
    bool operator==(Pt o) const { return x == o.x && y == o.y; }
    T dot(Pt o) const { return x * o.x + y * o.y; }
    T cross(Pt o) const { return x * o.y - y * o.x; }
    T cross(Pt a, Pt b) const { return (a - *this).cross(b - *this); }
    T norm2() const { return x * x + y * y; }
    ld len() const { return sqrtl((ld)norm2()); }
    ld ang() const { return atan2l((ld)y, (ld)x); }
    Pt perp() const { return {-y, x}; }
    Pt<ld> rot(ld t) const {
        return {x * cosl(t) - y * sinl(t), x * sinl(t) + y * cosl(t)};
    }
    Pt<ld> unit() const { return {x / len(), y / len()}; }
};

template <class T>
bool half(Pt<T> p) { return p.y < 0 || (p.y == 0 && p.x < 0); }  // [pi, 2pi)
template <class T>
bool angle_less(Pt<T> a, Pt<T> b) {  // a, b != (0, 0); ties = same direction
    if (half(a) != half(b)) return half(a) < half(b);
    return a.cross(b) > 0;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_point() {
    Pt<ll> a{1, 2}, b{4, 6}, c{0, 5};
    cout << (b - a).norm2() << ' ' << a.cross(b, c) << ' '  // 25 13 9
         << (b - a).dot(c - a) << '\n';
    vector<Pt<ll>> v = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
    sort(all(v), angle_less<ll>);  // counter-clockwise from angle 0
    cout << v[0].x << v[0].y << ' ' << v[3].x << v[3].y << '\n';  // 10 0-1
    Pt<ld> r = Pt<ld>{2, 0}.rot(PI / 2);  // (0, 2) up to rounding
    cout << llround(r.x) << ' ' << llround(r.y) << '\n';  // 0 2
}
