/**
 * Usage: SegTree<M> st(n) / st(vector); M = monoid {T, e(), op(a, b)}
 *        st.set(i, v); st.get(i); st.query(l, r) = op(a[l..r]) (inclusive,
 *          l > r gives e()); st.total()
 *        st.max_right(l, f) -> largest r with f(op(a[l..r-1])) true
 *        st.min_left(r, f)  -> smallest l with f(op(a[l..r-1])) true
 *          (f monotone, f(e()) true) -> "first index where prefix >= x"
 * Complexity: O(log n) per operation, O(n) build
 * Source: UMSS General Iterative Segment Tree (monoid + max_right; n not a
 *         power of 2 there) + AtCoder Library (max_right / min_left);
 *         old segment_tree (size ctor bug, -1 sentinel)
 * Note: op need NOT be commutative (left and right parts kept apart):
 *       matrices, (sum, best prefix, best suffix, best) for max subarray
 *       sum on ranges, string hashes. Point add: set(i, op(get(i), v)).
 * Status: untested
 */
struct SumM {
    typedef ll T;
    static T e() { return 0; }
    static T op(T a, T b) { return a + b; }
};
struct MaxM {
    typedef ll T;
    static T e() { return LLONG_MIN; }
    static T op(T a, T b) { return max(a, b); }
};

template <class M>
struct SegTree {
    typedef typename M::T T;
    int n, size;
    vector<T> t;
    SegTree(int n_) : n(n_), size(1) {
        while (size < n) size *= 2;
        t.assign(2 * size, M::e());
    }
    SegTree(const vector<T> &a) : SegTree(sz(a)) {
        for (int i = 0; i < n; i++) t[size + i] = a[i];
        for (int i = size - 1; i > 0; i--) pull(i);
    }
    void pull(int i) { t[i] = M::op(t[2 * i], t[2 * i + 1]); }
    void set(int p, T v) { for (t[p += size] = v; p >>= 1;) pull(p); }
    T get(int p) const { return t[p + size]; }
    T total() const { return t[1]; }
    T query(int l, int r) const {
        T sl = M::e(), sr = M::e();
        for (l += size, r += size + 1; l < r; l >>= 1, r >>= 1) {
            if (l & 1) sl = M::op(sl, t[l++]);
            if (r & 1) sr = M::op(t[--r], sr);
        }
        return M::op(sl, sr);
    }
    template <class F>
    int max_right(int l, F f) const {
        if (l == n) return n;
        l += size;
        T s = M::e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!f(M::op(s, t[l]))) {
                while (l < size)
                    if (l *= 2; f(M::op(s, t[l]))) s = M::op(s, t[l++]);
                return l - size;
            }
            s = M::op(s, t[l++]);
        } while ((l & -l) != l);
        return n;
    }
    template <class F>
    int min_left(int r, F f) const {
        if (r == 0) return 0;
        r += size;
        T s = M::e();
        do {
            r--;
            while (r > 1 && r % 2) r >>= 1;
            if (!f(M::op(t[r], s))) {
                while (r < size)
                    if (r = 2 * r + 1; f(M::op(t[r], s))) s = M::op(t[r--], s);
                return r + 1 - size;
            }
            s = M::op(t[r], s);
        } while ((r & -r) != r);
        return 0;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_segtree() {
    SegTree<SumM> s(vl{5, 1, 4, 3, 2});
    s.set(1, 6);  // 5 6 4 3 2
    cout << s.query(1, 3) << ' ' << s.total() << '\n';  // 13 20
    // longest prefix from 0 with sum <= 15 -> 5 + 6 + 4 (r = 3)
    cout << s.max_right(0, [](ll x) { return x <= 15; }) << '\n';  // 3
    SegTree<MaxM> m(vl{1, 7, 3, 9, 2});
    // first index i >= 2 with a[i] >= 8: max_right of "max < 8"
    int i = m.max_right(2, [](ll x) { return x < 8; });
    cout << i << ' ' << m.query(0, 2) << '\n';  // 3 7
}
