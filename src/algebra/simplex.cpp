/**
 * Usage: maximize c^T x  subject to  A x <= b, x >= 0
 *        LPSolver lp(A, b, c); vector<lpd> x; lpd v = lp.solve(x);
 *        v = -inf: infeasible, v = inf: unbounded, else optimum (x filled)
 * Complexity: O(n m * #pivots), exponential worst case, fast in practice
 * Source: UPB Simplex (Stanford two-phase; pivots with a double inverse
 *         while the table is long double) + UPB isc-notebook math_simplex;
 *         structure KACTL (Bland's rule on ties, no cycling)
 * Note: min c^T x -> max -c^T x. a x >= b -> -a x <= -b. Equality ->
 *       both inequalities. Free variable x -> x' - x''. b may be negative
 *       (phase 1 finds a feasible start). Integer LP is NOT solved by this.
 * Status: ok
 */
typedef double lpd;
typedef vector<lpd> lpv;
const lpd LP_EPS = 1e-9, LP_INF = numeric_limits<lpd>::infinity();

struct LPSolver {
    int m, n;
    vi N, B;
    vector<lpv> D;
    LPSolver(const vector<lpv> &A, const lpv &b, const lpv &c)
        : m(sz(b)), n(sz(c)), N(n + 1), B(m), D(m + 2, lpv(n + 2)) {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) D[i][j] = A[i][j];
            B[i] = n + i, D[i][n] = -1, D[i][n + 1] = b[i];
        }
        for (int j = 0; j < n; j++) N[j] = j, D[m][j] = -c[j];
        N[n] = -1, D[m + 1][n] = 1;
    }
    void pivot(int r, int s) {
        lpd *a = D[r].data(), inv = 1 / a[s];
        for (int i = 0; i < m + 2; i++)
            if (i != r && fabs(D[i][s]) > LP_EPS) {
                lpd *b = D[i].data(), inv2 = b[s] * inv;
                for (int j = 0; j < n + 2; j++) b[j] -= a[j] * inv2;
                b[s] = a[s] * inv2;
            }
        for (int j = 0; j < n + 2; j++) if (j != s) D[r][j] *= inv;
        for (int i = 0; i < m + 2; i++) if (i != r) D[i][s] *= -inv;
        D[r][s] = inv;
        swap(B[r], N[s]);
    }
    bool better(const lpv &row, int j, int s) {  // Bland tie-break
        return make_pair(row[j], N[j]) < make_pair(row[s], N[s]);
    }
    bool simplex(int phase) {
        int x = m + phase - 1;
        for (;;) {
            int s = -1;
            for (int j = 0; j <= n; j++)
                if (N[j] != -phase && (s == -1 || better(D[x], j, s))) s = j;
            if (D[x][s] >= -LP_EPS) return true;
            int r = -1;
            for (int i = 0; i < m; i++) {
                if (D[i][s] <= LP_EPS) continue;
                if (r == -1 || make_pair(D[i][n + 1] / D[i][s], B[i]) <
                                   make_pair(D[r][n + 1] / D[r][s], B[r]))
                    r = i;
            }
            if (r == -1) return false;
            pivot(r, s);
        }
    }
    lpd solve(lpv &x) {
        int r = 0;
        for (int i = 1; i < m; i++) if (D[i][n + 1] < D[r][n + 1]) r = i;
        if (D[r][n + 1] < -LP_EPS) {
            pivot(r, n);
            if (!simplex(2) || D[m + 1][n + 1] < -LP_EPS) return -LP_INF;
            for (int i = 0; i < m; i++)
                if (B[i] == -1) {
                    int s = 0;
                    for (int j = 1; j <= n; j++)
                        if (better(D[i], j, s)) s = j;
                    pivot(i, s);
                }
        }
        bool ok = simplex(1);
        x = lpv(n);
        for (int i = 0; i < m; i++) if (B[i] < n) x[B[i]] = D[i][n + 1];
        return ok ? D[m][n + 1] : LP_INF;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_simplex() {
    // max 3x + 2y : x + y <= 4, x + 3y <= 6, x <= 3  -> x = 3, y = 1
    LPSolver lp({{1, 1}, {1, 3}, {1, 0}}, {4, 6, 3}, {3, 2});
    lpv x;
    cout << lp.solve(x) << ' ' << x[0] << ' ' << x[1] << '\n';  // 11 3 1
    LPSolver bad({{1}, {-1}}, {1, -2}, {1});  // x <= 1 and x >= 2
    cout << bad.solve(x) << '\n';                               // -inf
    LPSolver unb({{-1}}, {1}, {1});           // max x, -x <= 1
    cout << unb.solve(x) << '\n';                               // inf
}
