// Stress test for src/algebra/simplex.cpp
// Oracles: bounded random LPs (box constraints added) solved by vertex
// enumeration: every choice of n tight constraints among A x <= b and
// x >= 0, solved by Cramer-free Gaussian elimination, kept if feasible;
// optimum = best feasible vertex, infeasible iff none. Unbounded cases
// built with a column of A <= 0 and positive objective, b >= 0.
#include "tests/stress/template_lib.h"
#include "src/algebra/simplex.cpp"
#include "tests/stress/common.h"

bool solve_sq(vector<vector<double>> a, vector<double> &x) {  // n x (n+1)
    int n = sz(a);
    for (int c = 0; c < n; c++) {
        int p = c;
        for (int i = c; i < n; i++) if (fabs(a[i][c]) > fabs(a[p][c])) p = i;
        if (fabs(a[p][c]) < 1e-9) return false;
        swap(a[p], a[c]);
        for (int i = 0; i < n; i++)
            if (i != c) {
                double f = a[i][c] / a[c][c];
                for (int j = c; j <= n; j++) a[i][j] -= f * a[c][j];
            }
    }
    x.assign(n, 0);
    for (int i = 0; i < n; i++) x[i] = a[i][n] / a[i][i];
    return true;
}

int main() {
    stress::check_example(example_simplex, "src/algebra/simplex.cpp", "example_simplex");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 3), m0 = stress::rnd(0, 4);
        vector<lpv> A;
        lpv b, c(n);
        for (int i = 0; i < m0; i++) {
            lpv r(n);
            for (auto &v : r) v = stress::rnd(-5, 5);
            A.pb(r), b.pb(stress::rnd(-6, 10));
        }
        for (int j = 0; j < n; j++) {  // box x_j <= U keeps it bounded
            lpv r(n, 0);
            r[j] = 1;
            A.pb(r), b.pb(stress::rnd(0, 8));
        }
        for (auto &v : c) v = stress::rnd(-5, 5);
        int m = sz(A);
        // all constraints as rows g . x <= h (x >= 0 -> -x_j <= 0)
        vector<lpv> G = A;
        lpv H = b;
        for (int j = 0; j < n; j++) {
            lpv r(n, 0);
            r[j] = -1;
            G.pb(r), H.pb(0);
        }
        int tot = m + n;
        double best = -LP_INF;
        for (int mask = 0; mask < (1 << tot); mask++) {
            if (__builtin_popcount(mask) != n) continue;
            vector<vector<double>> S;
            for (int i = 0; i < tot; i++)
                if (mask >> i & 1) {
                    vector<double> row(all(G[i]));
                    row.pb(H[i]);
                    S.pb(row);
                }
            vector<double> x;
            if (!solve_sq(S, x)) continue;
            bool feas = true;
            for (int i = 0; i < tot; i++) {
                double s = 0;
                for (int j = 0; j < n; j++) s += G[i][j] * x[j];
                feas &= s <= H[i] + 1e-7;
            }
            if (!feas) continue;
            double v = 0;
            for (int j = 0; j < n; j++) v += c[j] * x[j];
            best = max(best, v);
        }
        LPSolver lp(A, b, c);
        lpv x;
        lpd got = lp.solve(x);
        if (best == -LP_INF) {
            CHECK(got == -LP_INF, "infeasible expected, got " + to_string(got));
            continue;
        }
        CHECK(fabs(got - best) < 1e-6, "optimum " + to_string(got) + " vs " + to_string(best));
        double v = 0;
        for (int j = 0; j < n; j++) {
            CHECK(x[j] >= -1e-7, "x >= 0");
            v += c[j] * x[j];
        }
        CHECK(fabs(v - got) < 1e-6, "x attains the optimum");
        for (int i = 0; i < m; i++) {
            double s = 0;
            for (int j = 0; j < n; j++) s += A[i][j] * x[j];
            CHECK(s <= b[i] + 1e-6, "x feasible");
        }
    }
    // unbounded: column 0 of A is <= 0, c0 > 0, b >= 0 (x = 0 feasible)
    for (stress::iter = 0; stress::iter < stress::scaled(500); stress::iter++) {
        int n = stress::rnd(1, 4), m = stress::rnd(0, 5);
        vector<lpv> A(m, lpv(n));
        lpv b(m), c(n);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) A[i][j] = j == 0 ? -stress::rnd(0, 5) : stress::rnd(-5, 5);
            b[i] = stress::rnd(0, 10);
        }
        for (auto &v : c) v = stress::rnd(-5, 5);
        c[0] = stress::rnd(1, 5);
        LPSolver lp(A, b, c);
        lpv x;
        CHECK(lp.solve(x) == LP_INF, "unbounded n=" + to_string(n) + " m=" + to_string(m));
    }
    puts("OK");
}
