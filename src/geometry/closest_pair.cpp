/**
 * Usage: auto [d2, ij] = closest_pair(pts);  n >= 2, integer points
 *        d2 = min squared distance, ij = indices (into pts) of a closest pair
 * Complexity: O(n log n) (sweep by x, active set ordered by y)
 * Source: UMSS Closest Pair Of Points (O(n^2) when many points share x:
 *         TLE on CSES 2194) + UTP closest pair; sweep with a set: KACTL
 * Note: duplicates give d2 = 0. Farthest pair: convex_hull.cpp diameter2.
 *       For doubles use the same code with Pt<ld> and sqrt for the window.
 * Depends: src/geometry/point.cpp
 * Status: ok
 */
pair<ll, pii> closest_pair(const vector<Pt<ll>> &p) {
    int n = sz(p);
    vi id(n);
    iota(all(id), 0);
    sort(all(id), [&](int a, int b) { return p[a] < p[b]; });  // by x
    set<pair<ll, int>> act;  // (y, index) of points within the window
    pair<ll, pii> best = {LLONG_MAX, {-1, -1}};
    for (int i = 0, l = 0; i < n; i++) {
        Pt<ll> q = p[id[i]];
        ll d = (ll)ceill(sqrtl((ld)best.F));  // window half-width
        while (l < i && q.x - p[id[l]].x > d)
            act.erase({p[id[l]].y, id[l]}), l++;
        auto lo = act.lower_bound({q.y - d, -1});
        auto hi = act.upper_bound({q.y + d, n});
        for (auto it = lo; it != hi; ++it) {
            ll c = (p[it->S] - q).norm2();
            if (c < best.F)
                best = {c, {it->S, id[i]}}, d = (ll)ceill(sqrtl((ld)c));
        }
        act.insert({q.y, id[i]});
    }
    return best;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_closest_pair() {
    vector<Pt<ll>> pts = {{0, 0}, {10, 10}, {3, 4}, {11, 9}, {-5, 2}};
    auto [d2, ij] = closest_pair(pts);  // (10, 10) and (11, 9)
    cout << d2 << ' ' << min(ij.F, ij.S) << max(ij.F, ij.S) << '\n';  // 2 13
}
