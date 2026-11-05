/**
 * Usage: hull(pts)      -> convex hull CCW, starting at the lowest-leftmost
 *                          point, no collinear points, no duplicates (1 or 2
 *                          points for degenerate inputs, {} if empty)
 *        diameter2(h)   -> {max squared distance, (i, j)} over hull points
 *                          (rotating calipers on the CCW hull)
 * Complexity: hull O(n log n); diameter O(h)
 * Source: UMSS Convex Hull (empty result when all points are equal) + UPB
 *         ConvexHull + UMSS Polygon Diameter + UPB ConvexPolygonDiameter;
 *         monotone chain: Andrew
 * Note: keep collinear points: change <= 0 to < 0 in the pops (and handle
 *       the all-collinear case). Width / min-area rectangle: calipers with
 *       the edge directions. Integer hull: area2 exact with ll.
 * Depends: src/geometry/point.cpp
 * Status: untested
 */
template <class T>
vector<Pt<T>> hull(vector<Pt<T>> p) {
    auto by_y = [](Pt<T> a, Pt<T> b) { return tie(a.y, a.x) < tie(b.y, b.x); };
    sort(all(p), by_y);
    p.erase(unique(all(p)), p.end());
    if (sz(p) < 3) return p;
    vector<Pt<T>> h(2 * sz(p));
    int k = 0;
    for (int it = 0; it < 2; it++, reverse(all(p))) {  // lower, then upper
        int st = k;
        for (auto q : p) {
            while (k >= st + 2 && sgn(h[k - 2].cross(h[k - 1], q)) <= 0) k--;
            h[k++] = q;
        }
        k--;  // the last point starts the other chain
    }
    h.resize(k);
    if (k == 2 && h[0] == h[1]) h.pop_back();
    return h;
}

template <class T>
pair<T, pii> diameter2(const vector<Pt<T>> &h) {
    int n = sz(h);
    if (n < 2) return {0, {0, 0}};
    pair<T, pii> best = {0, {0, 0}};
    for (int i = 0, j = 1; i < n; i++)
        for (;; j = (j + 1) % n) {  // advance j while the next one is farther
            best = max(best, {(h[i] - h[j]).norm2(), {i, j}});
            Pt<T> e = h[(i + 1) % n] - h[i], f = h[(j + 1) % n] - h[j];
            if (sgn(e.cross(f)) <= 0) break;
        }
    return best;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_convex_hull() {
    vector<Pt<ll>> pts = {{0, 0}, {2, 0}, {4, 0}, {2, 2},
                          {4, 4}, {0, 4}, {1, 3}};
    auto h = hull(pts);  // (2, 0) is collinear and (2, 2), (1, 3) are inside
    cout << sz(h) << ' ' << h[0].x << h[0].y << ' '  // 4 00 40
         << h[1].x << h[1].y << '\n';
    cout << diameter2(h).F << '\n';  // 32
}
