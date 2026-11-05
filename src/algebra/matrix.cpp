/**
 * Usage: typedef vector<vl> Mat;  all mod m < 2^31 (entries in [0, m))
 *        mat_mul(A, B, m), mat_pow(A, e, m), mat_id(n)
 *        lin_rec(c, f, n, m) -> f_n for f_i = sum_j c[j] f_{i-1-j},
 *                               f = first k terms (k = sz(c)), n ~ 1e18
 *        mp_mul(A, B), mp_pow(A, e) -> (min, +) product: mp_pow(W, e)[i][j]
 *                               = cheapest walk i -> j with EXACTLY e edges
 * Complexity: mul O(n^3), pow O(n^3 log e); lin_rec O(k^3 log n)
 * Source: UPB MatrixExponentiation (% per product) + UPB FastExpo + UMSS
 *         Linear Recurrence Matrix; (min, +) semiring: own
 * Note: f_n = sum c_j f_{n-1-j} + d (constant) -> add a state that stays 1.
 *       #walks of length e from i to j = A^e[i][j]. Walks of length <= e:
 *       add a row/col "sink" or use [[A, I], [0, I]]. For k > ~60 use
 *       Kitamasa / Berlekamp-Massey (linear_recurrence.cpp).
 *       mp: INFL = no edge; mp_pow(W, 0) = identity (0 diagonal).
 * Status: ok
 */
typedef vector<vl> Mat;

Mat mat_id(int n) {
    Mat I(n, vl(n, 0));
    for (int i = 0; i < n; i++) I[i][i] = 1;
    return I;
}

Mat mat_mul(const Mat &A, const Mat &B, ll m = MOD) {
    int n = sz(A), p = sz(B), q = sz(B[0]);
    ll M2 = m * m;
    Mat C(n, vl(q, 0));
    for (int i = 0; i < n; i++)
        for (int k = 0; k < p; k++)
            if (A[i][k])
                for (int j = 0; j < q; j++) {  // sums kept < 2 m^2
                    C[i][j] += A[i][k] * B[k][j];
                    if (C[i][j] >= M2) C[i][j] -= M2;
                }
    for (auto &r : C) for (ll &x : r) x %= m;
    return C;
}

Mat mat_pow(Mat A, ll e, ll m = MOD) {
    Mat R = mat_id(sz(A));
    for (auto &r : R) for (ll &x : r) x %= m;
    for (; e; e >>= 1, A = mat_mul(A, A, m))
        if (e & 1) R = mat_mul(R, A, m);
    return R;
}

ll lin_rec(const vl &c, const vl &f, ll n, ll m = MOD) {
    int k = sz(c);
    if (n < k) return f[n] % m;
    Mat T(k, vl(k, 0));  // state (f_i, f_{i-1}, ..., f_{i-k+1})
    for (int j = 0; j < k; j++) T[0][j] = c[j] % m;
    for (int i = 1; i < k; i++) T[i][i - 1] = 1 % m;
    T = mat_pow(T, n - k + 1, m);
    ll r = 0;
    for (int j = 0; j < k; j++) r = (r + T[0][j] * (f[k - 1 - j] % m)) % m;
    return r;
}

Mat mp_mul(const Mat &A, const Mat &B) {
    int n = sz(A), p = sz(B), q = sz(B[0]);
    Mat C(n, vl(q, INFL));
    for (int i = 0; i < n; i++)
        for (int k = 0; k < p; k++)
            if (A[i][k] < INFL)
                for (int j = 0; j < q; j++)
                    if (B[k][j] < INFL)
                        C[i][j] = min(C[i][j], A[i][k] + B[k][j]);
    return C;
}

Mat mp_pow(Mat A, ll e) {
    int n = sz(A);
    Mat R(n, vl(n, INFL));
    for (int i = 0; i < n; i++) R[i][i] = 0;
    for (; e; e >>= 1, A = mp_mul(A, A))
        if (e & 1) R = mp_mul(R, A);
    return R;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_matrix() {
    Mat F = {{1, 1}, {1, 0}};
    cout << mat_pow(F, 10)[0][1] << '\n';                  // 55
    cout << lin_rec({1, 1}, {0, 1}, 90, (ll)2e9) << '\n';  // 370816120
    // tribonacci 0 0 1 1 2 4 7 13 ...
    cout << lin_rec({1, 1, 1}, {0, 0, 1}, 7) << '\n';      // 13
    Mat W = {{INFL, 1, 5}, {INFL, INFL, 1}, {1, INFL, INFL}};
    cout << mp_pow(W, 3)[0][0] << ' ' << mp_pow(W, 2)[0][2] << '\n';  // 3 2
}
