/**
 * Usage: XorBasis xb; xb.insert(x) (true if x was independent)
 *        xb.contains(x), xb.max_xor(x = 0) -> max of x ^ (span element),
 *        xb.min_xor(x), xb.r (rank; span has 2^r values),
 *        xb.kth(k) -> k-th smallest span value (k = 0 -> 0), k < 2^r
 *        xb.merge(o) (e.g. segment tree of bases, O(B^2))
 *        gauss_xor<N>(a, m, x) -> 0 / 1 / 2 like gauss.cpp, over GF(2):
 *                                a[i][0..m-1] coefficients, a[i][m] = rhs
 * Complexity: insert/query O(B), B = 64; gauss_xor O(n m^2 / 64)
 * Source: UMSS Xor Basis (2 versions) + UPB XOR_Basis_Bitset; kth, min,
 *         merge: own
 * Note: #subsets with XOR v (n numbers) = 2^(n-r) if contains(v) else 0.
 *       Max XOR of a path in a graph: xor of any path + basis of cycle
 *       xors (DFS, insert d[u] ^ d[v] ^ w for each non-tree edge).
 * Status: ok
 */
struct XorBasis {
    ull b[64] = {};  // b[i] has highest bit i
    int r = 0;
    bool insert(ull x) {
        for (int i = 63; i >= 0; i--)
            if (x >> i & 1) {
                if (!b[i]) { b[i] = x, r++; return true; }
                x ^= b[i];
            }
        return false;
    }
    bool contains(ull x) const {
        for (int i = 63; i >= 0; i--)
            if (x >> i & 1) {
                if (!b[i]) return false;
                x ^= b[i];
            }
        return true;
    }
    ull max_xor(ull x = 0) const {
        for (int i = 63; i >= 0; i--) x = max(x, x ^ b[i]);
        return x;
    }
    ull min_xor(ull x) const {
        for (int i = 63; i >= 0; i--) x = min(x, x ^ b[i]);
        return x;
    }
    ull kth(ull k) const {  // reduce, then bits of k pick basis vectors
        ull c[64];
        int t = 0;
        for (int i = 0; i < 64; i++)
            if (b[i]) {
                ull v = b[i];
                for (int j = i - 1; j >= 0; j--) if (v >> j & 1) v ^= b[j];
                c[t++] = v;
            }
        ull x = 0;
        for (int i = 0; i < t; i++) if (k >> i & 1) x ^= c[i];
        return x;
    }
    void merge(const XorBasis &o) { for (ull v : o.b) if (v) insert(v); }
};

template <int N>
int gauss_xor(vector<bitset<N>> a, int m, bitset<N> &x) {
    int n = sz(a), row = 0;
    vi where(m, -1);
    for (int col = 0; col < m && row < n; col++) {
        for (int i = row; i < n; i++)
            if (a[i][col]) { swap(a[i], a[row]); break; }
        if (!a[row][col]) continue;
        where[col] = row;
        for (int i = 0; i < n; i++) if (i != row && a[i][col]) a[i] ^= a[row];
        row++;
    }
    for (int i = row; i < n; i++) if (a[i][m]) return 0;
    x.reset();
    for (int j = 0; j < m; j++) if (where[j] != -1) x[j] = a[where[j]][m];
    return row < m ? 2 : 1;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_xor_basis() {
    XorBasis xb;
    for (ull v : {5, 6, 3, 9}) xb.insert(v);  // 3 = 5 ^ 6 is dependent
    cout << xb.r << ' ' << xb.max_xor() << ' '  // 3 15 1
         << xb.contains(12) << '\n';
    cout << xb.kth(0) << ' ' << xb.kth(1) << ' '  // 0 3 15
         << xb.kth(7) << '\n';
    cout << xb.min_xor(15) << '\n';                                        // 0
    vector<bitset<8>> a(2);  // x0 ^ x1 = 1, x1 = 1
    a[0][0] = a[0][1] = a[0][2] = 1, a[1][1] = a[1][2] = 1;
    bitset<8> x;
    cout << gauss_xor<8>(a, 2, x) << ' ' << x[0] << x[1] << '\n';  // 1 01
}
