/**
 * Usage: PST t(n);  positions [0, n); version 0 = all zeros (root 0)
 *        int r2 = t.add(r1, pos, v)  -> new version: a[pos] += v
 *        t.sum(r, l, r)              -> sum of a[l..r] in version r
 *        t.kth(ra, rb, k)            -> smallest p with sum over [0, p] of
 *                                       (version rb - version ra) > k
 * Complexity: O(log n) per op; O(log n) new nodes per add
 * Source: UMSS PersistantSegmentTree (pointers + new; build over int) +
 *         UTP persistent seg tree / array; k-th: own (classic)
 * Note: k-th smallest in a[l..r]: compress values, root[i] = add(root[i-1],
 *       a[i], 1); answer = kth(root[l], root[r + 1], k) (k 0-indexed).
 *       Count of values <= x in a[l..r]: sum(root[r+1]) - sum(root[l]).
 *       Reserve: ~ (q + 1) * (log2 n + 2) nodes.
 * Status: untested
 */
struct PST {
    int n;
    vi L, R;
    vl S;
    PST(int n_) : n(n_), L(1, 0), R(1, 0), S(1, 0) {}  // node 0 = empty
    int clone(int v) {
        L.pb(L[v]), R.pb(R[v]), S.pb(S[v]);
        return sz(S) - 1;
    }
    int add(int v, int l, int r, int pos, ll x) {
        int u = clone(v);
        S[u] += x;
        if (l < r) {
            int m = (l + r) / 2;
            if (pos <= m) { int c = add(L[v], l, m, pos, x); L[u] = c; }
            else { int c = add(R[v], m + 1, r, pos, x); R[u] = c; }
        }
        return u;
    }
    int add(int root, int pos, ll x) { return add(root, 0, n - 1, pos, x); }
    ll sum(int v, int l, int r, int ql, int qr) const {
        if (!v || qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return S[v];
        int m = (l + r) / 2;
        return sum(L[v], l, m, ql, qr) + sum(R[v], m + 1, r, ql, qr);
    }
    ll sum(int root, int l, int r) const { return sum(root, 0, n - 1, l, r); }
    int kth(int a, int b, ll k) const {
        int l = 0, r = n - 1;
        while (l < r) {
            int m = (l + r) / 2;
            ll left = S[L[b]] - S[L[a]];
            if (k < left) a = L[a], b = L[b], r = m;
            else k -= left, a = R[a], b = R[b], l = m + 1;
        }
        return l;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_persistent_segtree() {
    vi a = {3, 1, 4, 1, 5};  // values already in [0, 6)
    PST t(6);
    vi root = {0};
    for (int x : a) root.pb(t.add(root.back(), x, 1));
    // k-th smallest of a[1..3] = {1, 4, 1}: k = 0 -> 1, k = 2 -> 4
    cout << t.kth(root[1], root[4], 0) << ' '  // 1 4
         << t.kth(root[1], root[4], 2) << '\n';
    // #values <= 3 in a[0..4], and in the version after 2 elements
    cout << t.sum(root[5], 0, 3) << ' ' << t.sum(root[2], 0, 3) << '\n';  // 3 2
}
