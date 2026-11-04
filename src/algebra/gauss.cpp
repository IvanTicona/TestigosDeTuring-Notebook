/**
 * Usage: a = n rows of m coefficients + 1 extra column (right-hand side)
 *        gauss(a, x)          -> 0 no solution, 1 unique, 2 infinite (x = one
 *                                solution, free variables = 0), doubles
 *        gauss_mod(a, x, p)   -> same over Z_p (p prime)
 *        rank_mod(a, p), det_mod(a, p) (p prime), det(a) (doubles)
 *        det_any(a, m)        -> determinant mod ANY m (Euclid row steps)
 *        inverse_mod(a, p)    -> A^-1 mod p, empty if singular
 * Complexity: O(n^2 m); det_any O(n^3 log m)
 * Source: UPB GaussJordan (full pivoting, exits the program on singular
 *         matrices) + cp-algorithms style partial pivoting; mod variants
 *         and det_any: own
 * Note: p, m < 2^31, entries in [0, p). Kirchhoff: #spanning trees =
 *       det of the Laplacian (deg - adj) without one row and column.
 *       XOR systems: xor_basis.cpp (bitset elimination).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: untested
 */
typedef vector<double> vd;  // same as fft.cpp (repeating a typedef is fine)

int gauss(vector<vd> a, vd &x) {
    int n = sz(a), m = sz(a[0]) - 1;
    vi where(m, -1);
    for (int col = 0, row = 0; col < m && row < n; col++) {
        int sel = row;
        for (int i = row; i < n; i++) if (fabs(a[i][col]) > fabs(a[sel][col])) sel = i;
        if (fabs(a[sel][col]) < EPS) continue;
        swap(a[sel], a[row]);
        where[col] = row;
        for (int i = 0; i < n; i++)
            if (i != row) {
                double c = a[i][col] / a[row][col];
                for (int j = col; j <= m; j++) a[i][j] -= a[row][j] * c;
            }
        row++;
    }
    x.assign(m, 0);
    for (int i = 0; i < m; i++) if (where[i] != -1) x[i] = a[where[i]][m] / a[where[i]][i];
    for (int i = 0; i < n; i++) {
        double s = 0;
        for (int j = 0; j < m; j++) s += x[j] * a[i][j];
        if (fabs(s - a[i][m]) > EPS) return 0;
    }
    for (int i = 0; i < m; i++) if (where[i] == -1) return 2;
    return 1;
}

double det(vector<vd> a) {
    int n = sz(a);
    double d = 1;
    for (int i = 0; i < n; i++) {
        int b = i;
        for (int j = i + 1; j < n; j++) if (fabs(a[j][i]) > fabs(a[b][i])) b = j;
        if (i != b) swap(a[i], a[b]), d = -d;
        d *= a[i][i];
        if (d == 0) return 0;
        for (int j = i + 1; j < n; j++) {
            double v = a[j][i] / a[i][i];
            if (v != 0) for (int k = i + 1; k < n; k++) a[j][k] -= v * a[i][k];
        }
    }
    return d;
}

// Row-reduces a over Z_p; returns the rank, where[col] = pivot row or -1.
int rref_mod(vector<vl> &a, int cols, vi &where, ll p) {
    int n = sz(a), row = 0;
    where.assign(cols, -1);
    for (int col = 0; col < cols && row < n; col++) {
        int sel = row;
        while (sel < n && a[sel][col] == 0) sel++;
        if (sel == n) continue;
        swap(a[sel], a[row]);
        ll iv = invmod_prime(a[row][col], p);
        for (ll &v : a[row]) v = v * iv % p;
        for (int i = 0; i < n; i++)
            if (i != row && a[i][col]) {
                ll c = a[i][col];
                for (int j = col; j < sz(a[i]); j++) a[i][j] = (a[i][j] - c * a[row][j] % p + p) % p;
            }
        where[col] = row++;
    }
    return row;
}

int gauss_mod(vector<vl> a, vl &x, ll p = MOD) {
    int n = sz(a), m = sz(a[0]) - 1;
    vi where;
    int r = rref_mod(a, m, where, p);
    for (int i = r; i < n; i++) if (a[i][m]) return 0;  // 0 = nonzero
    x.assign(m, 0);
    for (int j = 0; j < m; j++) if (where[j] != -1) x[j] = a[where[j]][m];
    return r < m ? 2 : 1;
}

int rank_mod(vector<vl> a, ll p = MOD) {
    vi where;
    return rref_mod(a, sz(a[0]), where, p);
}

ll det_mod(vector<vl> a, ll p = MOD) {
    int n = sz(a);
    ll d = 1;
    for (int i = 0; i < n; i++) {
        int b = i;
        while (b < n && a[b][i] == 0) b++;
        if (b == n) return 0;
        if (b != i) swap(a[i], a[b]), d = (p - d) % p;
        d = d * a[i][i] % p;
        ll iv = invmod_prime(a[i][i], p);
        for (int j = i + 1; j < n; j++) {
            ll c = a[j][i] * iv % p;
            if (c) for (int k = i; k < n; k++) a[j][k] = (a[j][k] - c * a[i][k] % p + p) % p;
        }
    }
    return d;
}

ll det_any(vector<vl> a, ll m) {
    int n = sz(a);
    ll d = 1 % m;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++)
            while (a[j][i]) {  // Euclid on rows i, j (keeps the determinant)
                ll t = a[i][i] / a[j][i];
                for (int k = i; k < n; k++) a[i][k] = ((a[i][k] - t * a[j][k]) % m + m) % m;
                swap(a[i], a[j]), d = (m - d) % m;
            }
        d = d * a[i][i] % m;
        if (!d) return 0;
    }
    return d;
}

vector<vl> inverse_mod(const vector<vl> &A, ll p = MOD) {
    int n = sz(A);
    vector<vl> a(n, vl(2 * n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) a[i][j] = A[i][j];
        a[i][n + i] = 1;
    }
    vi where;
    if (rref_mod(a, n, where, p) < n) return {};
    vector<vl> r(n);
    for (int i = 0; i < n; i++) r[i] = vl(a[i].begin() + n, a[i].end());
    return r;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_gauss() {
    vd x;  // x + y = 3, x - y = 1
    int k = gauss({{1, 1, 3}, {1, -1, 1}}, x);
    cout << k << ' ' << x[0] << ' ' << x[1] << '\n';      // 1 2 1
    cout << gauss({{1, 1, 1}, {2, 2, 3}}, x) << ' ' << gauss({{1, 1, 1}, {2, 2, 2}}, x) << '\n';  // 0 2
    vl y;  // 2x = 1 (mod 7) -> x = 4
    cout << gauss_mod({{2, 1}}, y, 7) << ' ' << y[0] << '\n';  // 1 4
    cout << det_mod({{1, 2}, {3, 4}}) << ' ' << det_any({{1, 2}, {3, 4}}, 6) << '\n';  // 1000000005 4
    // spanning trees of K4 = 4^(4-2) = 16: Laplacian minus one row/col
    cout << det_mod({{3, MOD - 1, MOD - 1}, {MOD - 1, 3, MOD - 1}, {MOD - 1, MOD - 1, 3}}) << '\n';  // 16
    vector<vl> inv = inverse_mod({{1, 2}, {3, 4}}, 7);  // [[5, 1], [5, 3]]
    cout << inv[0][0] << ' ' << inv[0][1] << ' ' << inv[1][0] << ' ' << inv[1][1] << '\n';  // 5 1 5 3
}
