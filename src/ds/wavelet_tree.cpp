/**
 * Usage: Wavelet w(a);  static array of ll (any range, no compression)
 *        w.kth(l, r, k)      -> k-th smallest (0-indexed) of a[l..r]
 *        w.count_le(l, r, x) -> #i in [l, r] with a[i] <= x
 *        count in [x, y]: count_le(l, r, y) - count_le(l, r, x - 1)
 * Complexity: build O(n log V), queries O(log V), V = max - min + 1
 * Source: UTP wavelet tree; vector nodes + stable_partition: own
 * Note: also gives the median (k = len / 2), #distinct needs offline /
 *       PST instead. With updates: BIT of sorted vectors or PST.
 *       Same queries online with a persistent segment tree. Memory
 *       O(n log V) ints + nodes: with a huge value range compress to
 *       0..n-1 first (2e5 raw ll values: ~260 MB; compressed: ~20 MB).
 * Status: untested
 */
struct Wavelet {
    struct Nd { ll lo, hi; int l = -1, r = -1; vi b; };  // b[i]: #left in
    vector<Nd> t;                                      // the first i
    Wavelet(vl a) {
        ll lo = a.empty() ? 0 : *min_element(all(a));
        ll hi = a.empty() ? 0 : *max_element(all(a));
        build(a.begin(), a.end(), lo, hi);
    }
    int build(vl::iterator f, vl::iterator e, ll lo, ll hi) {
        int id = sz(t);
        t.pb({lo, hi, -1, -1, {}});
        if (f >= e || lo == hi) return id;
        ll mid = lo + (hi - lo) / 2;
        auto le = [mid](ll x) { return x <= mid; };
        vi b(1, 0);
        for (auto it = f; it != e; ++it) b.pb(b.back() + le(*it));
        auto p = stable_partition(f, e, le);
        int L = build(f, p, lo, mid), R = build(p, e, mid + 1, hi);
        t[id].b = move(b), t[id].l = L, t[id].r = R;
        return id;
    }
    ll kth(int l, int r, int k) const {
        int v = 0;
        r++;
        while (t[v].lo != t[v].hi) {
            int lb = t[v].b[l], rb = t[v].b[r];
            if (k < rb - lb) l = lb, r = rb, v = t[v].l;
            else k -= rb - lb, l -= lb, r -= rb, v = t[v].r;
        }
        return t[v].lo;
    }
    int cle(int v, int l, int r, ll x) const {
        if (l >= r || x < t[v].lo) return 0;
        if (t[v].hi <= x) return r - l;
        int lb = t[v].b[l], rb = t[v].b[r];
        return cle(t[v].l, lb, rb, x) + cle(t[v].r, l - lb, r - rb, x);
    }
    int count_le(int l, int r, ll x) const { return cle(0, l, r + 1, x); }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_wavelet_tree() {
    Wavelet w(vl{5, 1, 9, -3, 7, 1});
    // a[1..4] = {1, 9, -3, 7}: sorted -3 1 7 9
    cout << w.kth(1, 4, 0) << ' ' << w.kth(1, 4, 2) << '\n';  // -3 7
    cout << w.count_le(0, 5, 1) << ' ' << w.count_le(2, 4, 6) << '\n';  // 3 1
}
