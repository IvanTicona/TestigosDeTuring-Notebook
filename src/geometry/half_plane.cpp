/**
 * Usage: HP(a, b) = half-plane to the LEFT of the directed line a -> b
 *        hp_intersect(hs) -> CCW vertices of the intersection (bounded by
 *        a big box of half-side BOX), {} if empty or degenerate
 * Complexity: O(n log n)
 * Source: UPB HalfPlaneIntersection (ax + by <= c form; parallel half-planes
 *         decided by testing the ORIGIN: wrong) + cp-algorithms (deque)
 * Note: strict inequality semantics up to EPS: an intersection of area 0
 *       may come back empty. Polygon kernel (visibility region): the
 *       half-planes of its CCW edges. Feasibility of linear 2D constraints:
 *       non-empty result.
 * Depends: src/geometry/point.cpp
 * Status: untested
 */
struct HP {
    Pt<ld> p, d;  // point and UNIT direction: EPS is a distance, whatever
    ld ang;       // the length of b - a (short directions broke it)
    HP(Pt<ld> a, Pt<ld> b) : p(a), d((b - a).unit()), ang(atan2l(d.y, d.x)) {}
    bool out(Pt<ld> r) const { return d.cross(r - p) < -EPS; }
};
Pt<ld> hp_inter(const HP &s, const HP &t) {
    ld al = (t.p - s.p).cross(t.d) / s.d.cross(t.d);
    return s.p + s.d * al;
}

vector<Pt<ld>> hp_intersect(vector<HP> hs, ld BOX = 1e9) {
    Pt<ld> c[4] = {{-BOX, -BOX}, {BOX, -BOX}, {BOX, BOX}, {-BOX, BOX}};
    for (int i = 0; i < 4; i++) hs.pb(HP(c[i], c[(i + 1) % 4]));
    sort(all(hs), [](const HP &a, const HP &b) { return a.ang < b.ang; });
    deque<HP> dq;
    auto last = [&] { return hp_inter(dq.back(), dq[sz(dq) - 2]); };
    for (auto &h : hs) {
        while (sz(dq) > 1 && h.out(last())) dq.pop_back();
        while (sz(dq) > 1 && h.out(hp_inter(dq[0], dq[1]))) dq.pop_front();
        if (!dq.empty() && fabsl(h.d.cross(dq.back().d)) < EPS) {  // parallel
            if (h.d.dot(dq.back().d) < 0) return {};  // opposite: empty band
            if (h.out(dq.back().p)) dq.pop_back();    // h is stricter
            else continue;
        }
        dq.pb(h);
    }
    while (sz(dq) > 2 && dq[0].out(last())) dq.pop_back();
    while (sz(dq) > 2 && dq.back().out(hp_inter(dq[0], dq[1]))) dq.pop_front();
    if (sz(dq) < 3) return {};
    vector<Pt<ld>> r;
    ld a2 = 0;
    for (int i = 0; i < sz(dq); i++)
        r.pb(hp_inter(dq[i], dq[(i + 1) % sz(dq)]));
    for (int i = 0; i < sz(r); i++) a2 += r[i].cross(r[(i + 1) % sz(r)]);
    if (a2 < EPS) return {};  // empty or degenerate (point / segment)
    return r;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_half_plane() {
    // x >= 0, y >= 0, x + y <= 4: triangle of area 8
    vector<HP> hs = {HP({0, 1}, {0, 0}), HP({0, 0}, {1, 0}),
                     HP({4, 0}, {0, 4})};
    auto poly = hp_intersect(hs);
    ld a = 0;
    for (int i = 0; i < sz(poly); i++)
        a += poly[i].cross(poly[(i + 1) % sz(poly)]);
    cout << sz(poly) << ' ' << llround(a / 2) << '\n';  // 3 8
    hs.pb(HP({5, 5}, {6, 5}));  // y >= 5 too: empty
    cout << sz(hp_intersect(hs)) << '\n';  // 0
}
