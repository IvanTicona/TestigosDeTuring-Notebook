/**
 * Usage: all 0-indexed, ranges inclusive [l, r]
 *        BIT b(n); b.add(i, v); b.sum(r) (prefix [0, r]); b.sum(l, r);
 *          b.lower_bound(s) -> smallest i with prefix sum(i) >= s (n if
 *          none; needs non-negative values) -> k-th element of a multiset
 *        RangeBIT rb(n); rb.add(l, r, v); rb.sum(l, r) (range add + sum)
 *        BIT2D t(n, m); t.add(x, y, v); t.sum(x1, y1, x2, y2)
 * Complexity: O(log n) per op (2D: O(log n log m)); build O(n)
 * Source: UMSS Fenwick Tree (int) + UMSS Fenwick Tree 2D + UPB BIT
 *         (getind = lower_bound; global arrays, set/get clash with std) +
 *         UPB Fenwick2D + old fenwick_tree (RURQ int overflow); RangeBIT:
 *         two BITs (cp-algorithms)
 * Note: point update + range query; for range add + point query use a
 *       BIT on differences (rb.sum(i, i) also works). Inversions: iterate,
 *       sum of values greater than a[i] seen so far. Coordinates up to 1e9:
 *       compress first.
 * Status: untested
 */
struct BIT {
    int n;
    vl t;  // t[i] covers (i - lowbit(i), i] in 1-indexed terms
    BIT(int n_) : n(n_), t(n_ + 1, 0) {}
    BIT(const vl &a) : BIT(sz(a)) {  // O(n) build
        for (int i = 1; i <= n; i++) {
            t[i] += a[i - 1];
            if (int j = i + (i & -i); j <= n) t[j] += t[i];
        }
    }
    void add(int i, ll v) { for (i++; i <= n; i += i & -i) t[i] += v; }
    ll sum(int r) const {  // [0, r]; r >= n means everything
        ll s = 0;
        for (r = min(r + 1, n); r > 0; r -= r & -r) s += t[r];
        return s;
    }
    ll sum(int l, int r) const { return l > r ? 0 : sum(r) - sum(l - 1); }
    int lower_bound(ll s) const {
        int pos = 0;
        for (int pw = 1 << __lg(max(n, 1)); pw; pw >>= 1)
            if (pos + pw <= n && t[pos + pw] < s) pos += pw, s -= t[pos];
        return pos;  // 0-indexed answer
    }
};

struct RangeBIT {  // sum[0, r] = B1(r) * (r + 1) - B2(r)
    BIT b1, b2;
    RangeBIT(int n) : b1(n + 1), b2(n + 1) {}
    void add(int l, int r, ll v) {
        b1.add(l, v), b1.add(r + 1, -v);
        b2.add(l, v * l), b2.add(r + 1, -v * (r + 1));
    }
    ll pre(int r) const { return b1.sum(r) * (r + 1) - b2.sum(r); }
    ll sum(int l, int r) const {
        return l > r ? 0 : pre(r) - (l ? pre(l - 1) : 0);
    }
};

struct BIT2D {
    int n, m;
    vector<vl> t;
    BIT2D(int n_, int m_) : n(n_), m(m_), t(n_ + 1, vl(m_ + 1, 0)) {}
    void add(int x, int y, ll v) {
        for (int i = x + 1; i <= n; i += i & -i)
            for (int j = y + 1; j <= m; j += j & -j) t[i][j] += v;
    }
    ll pre(int x, int y) const {  // [0, x] x [0, y]
        ll s = 0;
        for (int i = x + 1; i > 0; i -= i & -i)
            for (int j = y + 1; j > 0; j -= j & -j) s += t[i][j];
        return s;
    }
    ll sum(int x1, int y1, int x2, int y2) const {
        return pre(x2, y2) - pre(x1 - 1, y2) - pre(x2, y1 - 1) +
               pre(x1 - 1, y1 - 1);
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_fenwick() {
    BIT b(vl{5, 1, 0, 3});
    b.add(2, 4);  // 5 1 4 3
    cout << b.sum(3) << ' ' << b.sum(1, 2) << ' '  // 13 5 2
         << b.lower_bound(7) << '\n';
    RangeBIT rb(5);
    rb.add(1, 3, 2), rb.add(0, 4, 1);  // 1 3 3 3 1
    cout << rb.sum(0, 4) << ' ' << rb.sum(2, 2) << '\n';  // 11 3
    BIT2D t(3, 3);
    t.add(0, 0, 1), t.add(1, 2, 5), t.add(2, 2, 7);
    cout << t.sum(1, 1, 2, 2) << ' ' << t.sum(0, 0, 0, 2) << '\n';  // 12 1
}
