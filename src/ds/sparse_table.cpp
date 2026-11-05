/**
 * Usage: Sparse sp(a, f);          f idempotent (min, max, gcd, &, |)
 *        sp.query(l, r)            -> f over a[l..r] in O(1)
 *        DisjointSparse ds(a, f);  f ANY associative (sum, product mod p,
 *                                  matrix product, ...); ds.query(l, r) O(1)
 *        e.g. Sparse sp(a, [](ll x, ll y) { return min(x, y); });
 * Complexity: build O(n log n) time and memory; query O(1)
 * Source: UMSS Min Sparse Table (log by loop) + UPB SparseTable + UPB
 *         DisjointSparseTable (std::function: slow); templates: own
 * Note: static arrays only (no updates). Also: argmin (compare indices by
 *       value), LCA via Euler tour + RMQ (trees section).
 * Status: ok
 */
template <class T, class F>
struct Sparse {
    vector<vector<T>> t;
    F f;
    Sparse(const vector<T> &a, F f_) : t(1, a), f(f_) {
        for (int k = 1; (1 << k) <= sz(a); k++) {
            t.pb(vector<T>(sz(a) - (1 << k) + 1));
            for (int i = 0; i < sz(t[k]); i++)
                t[k][i] = f(t[k - 1][i], t[k - 1][i + (1 << (k - 1))]);
        }
    }
    T query(int l, int r) const {
        int k = __lg(r - l + 1);
        return f(t[k][l], t[k][r - (1 << k) + 1]);
    }
};

template <class T, class F>
struct DisjointSparse {
    // level k: blocks of size 2^(k+1); L = suffix folds of the left half,
    // R = prefix folds of the right half
    vector<vector<T>> L, R;
    vector<T> a;
    F f;
    DisjointSparse(const vector<T> &a_, F f_) : a(a_), f(f_) {
        int n = sz(a);
        for (int k = 0; (1 << k) < n; k++) {
            int len = 1 << k;
            L.pb(a), R.pb(a);
            for (int i = 0; i < n; i += 2 * len) {
                int mid = min(i + len, n), end = min(i + 2 * len, n);
                for (int j = mid - 2; j >= i; j--)
                    L[k][j] = f(a[j], L[k][j + 1]);
                for (int j = mid + 1; j < end; j++)
                    R[k][j] = f(R[k][j - 1], a[j]);
            }
        }
    }
    T query(int l, int r) const {
        if (l == r) return a[l];
        int k = __lg(l ^ r);  // l, r split by the middle of a 2^(k+1) block
        return f(L[k][l], R[k][r]);
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_sparse_table() {
    vl a = {5, 2, 8, 6, 3, 7};
    Sparse mn(a, [](ll x, ll y) { return min(x, y); });
    Sparse g(a, [](ll x, ll y) { return __gcd(x, y); });
    cout << mn.query(2, 4) << ' ' << g.query(2, 3) << '\n';  // 3 2
    DisjointSparse sum(a, [](ll x, ll y) { return x + y; });
    cout << sum.query(1, 4) << ' ' << sum.query(5, 5) << '\n';  // 19 7
}
