// Stress test for src/algebra/gauss.cpp
// Oracles: tiny systems over Z_p (p = 2, 3, 5, 7) solved by enumerating all
// p^m assignments (count of solutions -> 0 / 1 / many); returned solutions
// substituted back; determinant by Leibniz permutation expansion
// (__int128 exact, then mod); rank via the #solutions of A x = 0 (p^(m-r));
// inverse checked by A * inv == I; doubles: random integer systems with
// planted solution, singular systems built from dependent rows.
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/algebra/gauss.cpp"
#include "tests/stress/common.h"

i128 leibniz(const vector<vl> &a, ll md = 0) {  // md = 0: exact (small entries)
    int n = sz(a);
    vi pm(n);
    iota(all(pm), 0);
    i128 s = 0;
    do {
        i128 t = 1;
        for (int i = 0; i < n; i++) t = md ? t * a[i][pm[i]] % md : t * a[i][pm[i]];
        int inv = 0;
        for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) inv += pm[i] > pm[j];
        s += inv & 1 ? -t : t;
    } while (next_permutation(all(pm)));
    return s;
}

int main() {
    stress::check_example(example_gauss, "src/algebra/gauss.cpp", "example_gauss");
    // mod p systems vs enumeration
    vl small = {2, 3, 5, 7};
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        ll p = small[stress::rnd(0, 3)];
        int n = stress::rnd(1, 5), m = stress::rnd(1, 4);
        vector<vl> a(n, vl(m + 1));
        for (auto &r : a) for (ll &v : r) v = stress::rnd(0, 3) ? stress::rnd(0, p - 1) : 0;
        ll total = 1, cnt = 0, hom = 0;
        for (int i = 0; i < m; i++) total *= p;
        for (ll code = 0; code < total; code++) {
            vl x(m);
            ll c = code;
            for (int i = 0; i < m; i++) x[i] = c % p, c /= p;
            bool ok = true, ok0 = true;
            for (int i = 0; i < n; i++) {
                ll s = 0;
                for (int j = 0; j < m; j++) s += a[i][j] * x[j];
                ok &= s % p == a[i][m];
                ok0 &= s % p == 0;
            }
            cnt += ok, hom += ok0;
        }
        vl x;
        int k = gauss_mod(a, x, p);
        CHECK(k == (cnt == 0 ? 0 : cnt == 1 ? 1 : 2), "gauss_mod kind p=" + to_string(p));
        if (k) {
            for (int i = 0; i < n; i++) {
                ll s = 0;
                for (int j = 0; j < m; j++) s += a[i][j] * x[j];
                CHECK(s % p == a[i][m], "gauss_mod solution satisfies");
            }
        }
        vector<vl> A(n, vl(m));
        for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) A[i][j] = a[i][j];
        int r = rank_mod(A, p);
        ll pw = 1;
        for (int i = 0; i < m - r; i++) pw *= p;
        CHECK(hom == pw, "rank_mod r=" + to_string(r));
    }
    // determinants and inverses
    vl mods = {MOD, MOD2, 7, 2};
    vl anym = {1, 4, 6, 12, 30, 1000000000, 2147483646, 360};
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int n = stress::rnd(1, 6);
        ll p = mods[stress::rnd(0, 3)], m = anym[stress::rnd(0, sz(anym) - 1)];
        vector<vl> a(n, vl(n)), b(n, vl(n));
        for (auto &r : a) for (ll &v : r) v = stress::rnd(0, 2) ? stress::rnd(0, p - 1) : 0;
        for (auto &r : b) for (ll &v : r) v = stress::rnd(0, 2) ? stress::rnd(0, m - 1) : 0;
        if (n >= 2 && stress::rnd(0, 4) == 0) a[1] = a[0], b[1] = b[0];  // singular
        ll want = (ll)(((leibniz(a, p) % p) + p) % p);
        CHECK(det_mod(a, p) == want, "det_mod n=" + to_string(n) + " p=" + to_string(p));
        CHECK(det_any(b, m) == (ll)(((leibniz(b, m) % m) + m) % m), "det_any n=" + to_string(n) + " m=" + to_string(m));
        vector<vl> inv = inverse_mod(a, p);
        CHECK(inv.empty() == (want == 0), "inverse exists iff det != 0");
        if (!inv.empty())
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++) {
                    ll s = 0;
                    for (int k = 0; k < n; k++) s = (s + a[i][k] * inv[k][j]) % p;
                    CHECK(s == (i == j), "A * inv == I");
                }
        vector<vd> fa(n, vd(n));
        vector<vl> ia(n, vl(n));
        for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) ia[i][j] = stress::rnd(-9, 9), fa[i][j] = ia[i][j];
        double dd = det(fa), ex = (double)leibniz(ia);
        CHECK(fabs(dd - ex) < 1e-6 * max(1.0, fabs(ex)), "det double");
    }
    // doubles: planted unique solutions, inconsistent and dependent systems
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int m = stress::rnd(1, 6);
        vector<vd> a(m, vd(m + 1));
        vd sol(m);
        for (double &v : sol) v = stress::rnd(-100, 100);
        vector<vl> ia(m, vl(m));
        for (int i = 0; i < m; i++) {
            double s = 0;
            for (int j = 0; j < m; j++) ia[i][j] = stress::rnd(-20, 20), a[i][j] = ia[i][j], s += a[i][j] * sol[j];
            a[i][m] = s;
        }
        vd x;
        int k = gauss(a, x);
        if (leibniz(ia) != 0) {
            CHECK(k == 1, "gauss unique");
            for (int j = 0; j < m; j++) CHECK(fabs(x[j] - sol[j]) < 1e-6, "gauss value");
        } else CHECK(k == 2, "singular but consistent -> infinite");
        if (m >= 1) {  // duplicate row 0 with different rhs -> no solution
            a.pb(a[0]);
            a.back()[m] += 1;
            CHECK(gauss(a, x) == 0, "inconsistent");
        }
    }
    puts("OK");
}
