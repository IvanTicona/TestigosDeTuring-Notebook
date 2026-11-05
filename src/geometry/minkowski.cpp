/**
 * Usage: minkowski(A, B) -> convex polygon A + B = {a + b}, CCW, for convex
 *        CCW polygons A, B (no collinear triples); collinear output
 *        vertices removed
 * Complexity: O(|A| + |B|)
 * Source: UPB MinkowskiSum (rotates both to the lowest point and merges
 *         edges by angle) + UPB ConvexPolygonIntersection (not included:
 *         use half_plane.cpp with the edges of both polygons)
 * Note: distance between convex polygons A, B = distance from the origin
 *       to A + (-B) (0 if it contains the origin). Collision of a moving
 *       convex shape: Minkowski sum with the reflected obstacle.
 * Depends: src/geometry/point.cpp
 * Status: untested
 */
template <class T>
vector<Pt<T>> minkowski(vector<Pt<T>> A, vector<Pt<T>> B) {
    auto low = [](vector<Pt<T>> &P) {  // start at the lowest, then leftmost
        int k = 0;
        for (int i = 1; i < sz(P); i++)
            if (tie(P[i].y, P[i].x) < tie(P[k].y, P[k].x)) k = i;
        rotate(P.begin(), P.begin() + k, P.end());
    };
    low(A), low(B);
    int n = sz(A), m = sz(B), i = 0, j = 0;
    vector<Pt<T>> r;
    while (i < n || j < m) {
        r.pb(A[i % n] + B[j % m]);
        Pt<T> ea = A[(i + 1) % n] - A[i % n], eb = B[(j + 1) % m] - B[j % m];
        T c = ea.cross(eb);
        if (j == m || (i < n && c > 0)) i++;
        else if (i == n || c < 0) j++;
        else i++, j++;  // parallel edges: advance both
    }
    vector<Pt<T>> h;  // drop collinear vertices
    auto flat = [&](Pt<T> p) {
        return sgn(h[sz(h) - 2].cross(h.back(), p)) == 0;
    };
    for (auto p : r) {
        while (sz(h) >= 2 && flat(p)) h.pop_back();
        h.pb(p);
    }
    while (sz(h) >= 3 && flat(h[0])) h.pop_back();
    return h;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_minkowski() {
    vector<Pt<ll>> sq = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    vector<Pt<ll>> tri = {{0, 0}, {2, 0}, {0, 2}};
    auto s = minkowski(sq, tri);  // pentagon (0,0) (3,0) (3,1) (1,3) (0,3)
    ll a2 = 0;  // twice the area
    for (int i = 0; i < sz(s); i++) a2 += s[i].cross(s[(i + 1) % sz(s)]);
    cout << sz(s) << ' ' << a2 << '\n';  // 5 14
}
