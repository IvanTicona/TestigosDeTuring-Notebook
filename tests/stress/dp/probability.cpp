// Stress test for src/dp/probability.cpp
// Oracles: enumeration of all f^n dice outcomes (f^n <= 5e4); step-by-step
// vector-matrix products for markov; value iteration E <- 1 + T E until
// convergence for absorbing chains (each transient state reaches an
// absorbing one).
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/algebra/gauss.cpp"
#include "src/dp/probability.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_probability, "src/dp/probability.cpp", "example_probability");
    for (int f = 1; f <= 8; f++)
        for (int n = 0; n <= 6; n++) {
            ll tot = 1;
            for (int i = 0; i < n; i++) tot *= f;
            if (tot > 50000) continue;
            vl cnt(n * f + 1, 0);
            for (ll code = 0; code < tot; code++) {
                ll c = code, s = 0;
                for (int i = 0; i < n; i++) s += c % f + 1, c /= f;
                cnt[s]++;
            }
            vd p = dice_pmf(n, f);
            vl q = dice_count(n, f), q7 = dice_count(n, f, 7);
            CHECK(sz(p) == n * f + 1 && sz(q) == n * f + 1, "sizes");
            for (int s = 0; s <= n * f; s++) {
                CHECK(fabs(p[s] - (double)cnt[s] / tot) < 1e-12, "pmf n=" + to_string(n) + " f=" + to_string(f));
                CHECK(q[s] == cnt[s] % MOD && q7[s] == cnt[s] % 7, "count");
            }
        }
    for (stress::iter = 0; stress::iter < stress::scaled(500); stress::iter++) {
        int n = stress::rnd(2, 6);
        vb isabs(n, false);
        isabs[0] = true;
        for (int i = 1; i < n; i++) isabs[i] = stress::rnd(0, 3) == 0;
        vector<vd> T(n, vd(n, 0));
        for (int i = 0; i < n; i++) {
            if (isabs[i]) { T[i][i] = 1; continue; }
            vd w(n);
            double s = 0;
            for (int j = 0; j < n; j++) w[j] = stress::rnd(0, 4), s += w[j];
            w[0] += 1, s += 1;  // always some chance to be absorbed at 0
            for (int j = 0; j < n; j++) T[i][j] = w[j] / s;
        }
        vd e = absorb_steps(T, isabs), it(n, 0);
        for (int r = 0; r < 3000; r++) {  // absorbs w.p. >= 1/25 per step
            vd nx(n, 0);
            for (int i = 0; i < n; i++) {
                if (isabs[i]) continue;
                nx[i] = 1;
                for (int j = 0; j < n; j++) nx[i] += T[i][j] * it[j];
            }
            it = nx;
        }
        for (int i = 0; i < n; i++) CHECK(fabs(e[i] - it[i]) < 1e-6, "absorb i=" + to_string(i));
        vd pi(n, 0);
        pi[stress::rnd(0, n - 1)] = 1;
        ll k = stress::rnd(0, 60);
        vd cur = pi;
        for (ll s = 0; s < k; s++) {
            vd r(n, 0);
            for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) r[j] += cur[i] * T[i][j];
            cur = r;
        }
        vd got = markov(pi, T, k);
        for (int i = 0; i < n; i++) CHECK(fabs(got[i] - cur[i]) < 1e-9, "markov k=" + to_string(k));
    }
    puts("OK");
}
