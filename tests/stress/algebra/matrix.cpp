// Stress test for src/algebra/matrix.cpp
// Oracles: repeated naive multiplication (__int128) for powers, direct
// iteration of the recurrence for lin_rec (n <= 3000), Bellman-Ford style
// layered DP (exactly e edges) for the (min, +) power.
#include "tests/stress/template_lib.h"
#include "src/algebra/matrix.cpp"
#include "tests/stress/common.h"

Mat naive_mul(const Mat &A, const Mat &B, ll m) {
    int n = sz(A), p = sz(B), q = sz(B[0]);
    Mat C(n, vl(q, 0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < q; j++) {
            i128 s = 0;
            for (int k = 0; k < p; k++) s += (i128)A[i][k] * B[k][j];
            C[i][j] = (ll)(s % m);
        }
    return C;
}

int main() {
    stress::check_example(example_matrix, "src/algebra/matrix.cpp", "example_matrix");
    vl mods = {1, 2, 10, MOD, MOD2, 2147483647};
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        ll m = mods[stress::rnd(0, sz(mods) - 1)];
        int n = stress::rnd(1, 8), p = stress::rnd(1, 8), q = stress::rnd(1, 8);
        bool worst = stress::rnd(0, 3) == 0;
        Mat A(n, vl(p)), B(p, vl(q)), S(n, vl(n));
        for (auto &r : A) for (ll &x : r) x = worst ? m - 1 : stress::rnd(0, m - 1);
        for (auto &r : B) for (ll &x : r) x = worst ? m - 1 : stress::rnd(0, m - 1);
        for (auto &r : S) for (ll &x : r) x = worst ? m - 1 : stress::rnd(0, m - 1);
        CHECK(mat_mul(A, B, m) == naive_mul(A, B, m), "mul m=" + to_string(m));
        int e = stress::rnd(0, 40);
        Mat R = naive_mul(mat_id(n), mat_id(n), m);
        for (int i = 0; i < e; i++) R = naive_mul(R, S, m);
        CHECK(mat_pow(S, e, m) == R, "pow e=" + to_string(e) + " m=" + to_string(m));
    }
    // lin_rec vs direct iteration
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        ll m = mods[stress::rnd(0, sz(mods) - 1)];
        int k = stress::rnd(1, 6);
        vl c(k), f(k);
        for (ll &x : c) x = stress::rnd(0, m - 1);
        for (ll &x : f) x = stress::rnd(0, m - 1);
        vl seq = f;
        int N = 3000;
        while (sz(seq) <= N) {
            i128 s = 0;
            for (int j = 0; j < k; j++) s += (i128)c[j] * seq[sz(seq) - 1 - j];
            seq.pb((ll)(s % m));
        }
        for (int t = 0; t < 10; t++) {
            int n = t < 3 ? stress::rnd(0, k) : stress::rnd(0, N);
            CHECK(lin_rec(c, f, n, m) == seq[n] % m, "lin_rec k=" + to_string(k) + " n=" + to_string(n));
        }
    }
    // (min, +) power vs layered DP
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        int n = stress::rnd(1, 7), e = stress::rnd(0, 30);
        Mat W(n, vl(n, INFL));
        for (auto &r : W) for (ll &x : r) if (stress::rnd(0, 2)) x = stress::rnd(-5, 1000000000000LL);
        Mat P = mp_pow(W, e);
        for (int s = 0; s < n; s++) {
            vl d(n, INFL);
            d[s] = 0;
            for (int l = 0; l < e; l++) {
                vl nd(n, INFL);
                for (int u = 0; u < n; u++)
                    if (d[u] < INFL)
                        for (int v = 0; v < n; v++)
                            if (W[u][v] < INFL) nd[v] = min(nd[v], d[u] + W[u][v]);
                d = nd;
            }
            CHECK(P[s] == d, "mp_pow n=" + to_string(n) + " e=" + to_string(e));
        }
    }
    puts("OK");
}
