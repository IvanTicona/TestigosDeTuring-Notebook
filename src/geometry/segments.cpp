/**
 * Usage: points Pt<T> (ll exact or ld); results with coordinates are Pt<ld>
 *        on_segment(a, b, p)      -> p on the closed segment ab (exact)
 *        seg_intersect(a, b, c, d)-> closed segments touch / cross (exact)
 *        seg_inter(a, b, c, d)    -> {} / {point} / {two ends of the
 *                                    common part} (sorted, as Pt<ld>)
 *        line_inter(a, b, c, d)   -> {1, P} unique, {0, _} parallel,
 *                                    {-1, _} same line (lines ab and cd)
 *        project(a, b, p) / seg_dist(a, b, p) / seg_seg_dist(a, b, c, d)
 * Complexity: O(1)
 * Source: UPB Geometry.cpp (Stanford: ComputeLineIntersection,
 *         SegmentsIntersect) + UMSS Segment Intersection (slope-based,
 *         printed results, wrong for lines vs segments) + KACTL segInter
 * Note: a != b and c != d where a direction is needed (project, line_inter).
 *       Sign tests use the template's sgn (EPS for ld, exact for ll).
 * Depends: src/geometry/point.cpp
 * Status: ok
 */
template <class T>
Pt<ld> to_ld(Pt<T> p) { return {(ld)p.x, (ld)p.y}; }

template <class T>
bool on_segment(Pt<T> a, Pt<T> b, Pt<T> p) {
    return sgn(p.cross(a, b)) == 0 && sgn((a - p).dot(b - p)) <= 0;
}

template <class T>
bool seg_intersect(Pt<T> a, Pt<T> b, Pt<T> c, Pt<T> d) {
    int o1 = sgn(a.cross(b, c)), o2 = sgn(a.cross(b, d));
    int o3 = sgn(c.cross(d, a)), o4 = sgn(c.cross(d, b));
    if (o1 * o2 < 0 && o3 * o4 < 0) return true;
    return on_segment(a, b, c) || on_segment(a, b, d) || on_segment(c, d, a) ||
           on_segment(c, d, b);
}

template <class T>
vector<Pt<ld>> seg_inter(Pt<T> a, Pt<T> b, Pt<T> c, Pt<T> d) {
    T oa = c.cross(d, a), ob = c.cross(d, b);
    T oc = a.cross(b, c), od = a.cross(b, d);
    if (sgn(oa) * sgn(ob) < 0 && sgn(oc) * sgn(od) < 0)  // proper crossing
        return {(to_ld(a) * (ld)ob - to_ld(b) * (ld)oa) / (ld)(ob - oa)};
    set<Pt<T>> s;
    if (on_segment(c, d, a)) s.insert(a);
    if (on_segment(c, d, b)) s.insert(b);
    if (on_segment(a, b, c)) s.insert(c);
    if (on_segment(a, b, d)) s.insert(d);
    vector<Pt<ld>> r;
    for (auto p : s) r.pb(to_ld(p));
    return r;
}

template <class T>
pair<int, Pt<ld>> line_inter(Pt<T> a, Pt<T> b, Pt<T> c, Pt<T> d) {
    T den = (b - a).cross(d - c);
    if (sgn(den) == 0) return {sgn(a.cross(b, c)) == 0 ? -1 : 0, {}};
    ld t = (ld)(c - a).cross(d - c) / (ld)den;
    return {1, to_ld(a) + to_ld(b - a) * t};
}

template <class T>
Pt<ld> project(Pt<T> a, Pt<T> b, Pt<T> p) {  // foot on the LINE ab
    ld t = (ld)(p - a).dot(b - a) / (ld)(b - a).norm2();
    return to_ld(a) + to_ld(b - a) * t;
}

template <class T>
ld seg_dist(Pt<T> a, Pt<T> b, Pt<T> p) {
    if (a == b) return (p - a).len();
    ld t = min((ld)1, max((ld)0, (ld)(p - a).dot(b - a) / (ld)(b - a).norm2()));
    return (to_ld(a) + to_ld(b - a) * t - to_ld(p)).len();
}

template <class T>
ld seg_seg_dist(Pt<T> a, Pt<T> b, Pt<T> c, Pt<T> d) {
    if (seg_intersect(a, b, c, d)) return 0;
    return min({seg_dist(a, b, c), seg_dist(a, b, d), seg_dist(c, d, a),
                seg_dist(c, d, b)});
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_segments() {
    Pt<ll> a{0, 0}, b{4, 4}, c{0, 4}, d{4, 0}, e{2, 2}, f{6, 6};
    auto x = seg_inter(a, b, c, d);  // X shape: crossing at (2, 2)
    cout << sz(x) << ' ' << llround(x[0].x) << llround(x[0].y) << '\n';  // 1 22
    auto o = seg_inter(a, b, e, f);  // collinear overlap [(2,2), (4,4)]
    int same = line_inter(a, b, e, f).F;  // -1: same line
    cout << sz(o) << ' ' << llround(o[1].x) << ' ' << same << '\n';  // 2 4 -1
    cout << to_string(seg_dist(a, d, Pt<ll>{2, 3})) << '\n';  // 3.000000
}
