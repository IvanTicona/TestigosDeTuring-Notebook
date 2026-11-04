// Stress test for src/dp/dp_opt.cpp
// Oracles: plain O(k n^2) partition DP with costs satisfying the quadrangle
// inequality (sum^2, sum * length, (max index span)^2); plain O(n^3)
// interval DP for Knuth (merge cost = interval sum, positive weights);
// aliens vs the plain DP for EVERY k (sum^2 costs are convex in k).
#include "tests/stress/template_lib.h"
#include "src/dp/dp_opt.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_dp_opt, "src/dp/dp_opt.cpp", "example_dp_opt");
    for (stress::iter = 0; stress::iter < stress::scaled(1200); stress::iter++) {
        int n = stress::rnd(1, 40), type = stress::rnd(0, 2);
        vl a(n), pre(n + 1, 0);
        for (ll &x : a) x = stress::rnd(0, stress::rnd(0, 1) ? 3 : 1000);
        for (int i = 0; i < n; i++) pre[i + 1] = pre[i] + a[i];
        auto C = [&](int j, int i) -> ll {
            ll s = pre[i] - pre[j];
            if (type == 0) return s * s;
            if (type == 1) return s * (i - j);
            return (ll)(i - j) * (i - j);
        };
        vector<vl> dp(n + 1, vl(n + 1, INFL));  // dp[t][i]
        dp[0][0] = 0;
        for (int t = 1; t <= n; t++)
            for (int i = 1; i <= n; i++)
                for (int j = 0; j < i; j++)
                    if (dp[t - 1][j] < INFL) dp[t][i] = min(dp[t][i], dp[t - 1][j] + C(j, i));
        int k = stress::rnd(1, n);
        CHECK(partition_dnc(n, k, C) == dp[k][n], "dnc n=" + to_string(n) + " k=" + to_string(k));
        if (type == 0) {
            auto f = [&](ll lam) {
                vector<pll> d(n + 1, {INFL, 0});
                d[0] = {0, 0};
                for (int i = 1; i <= n; i++)
                    for (int j = 0; j < i; j++) d[i] = min(d[i], pll{d[j].F + C(j, i) + lam, d[j].S + 1});
                return d[n];
            };
            ll H = pre[n] * pre[n] + 1;
            for (int kk = 1; kk <= n; kk++)
                CHECK(aliens(kk, 0, H, f) == dp[kk][n], "aliens n=" + to_string(n) + " k=" + to_string(kk));
        }
        // Knuth: optimal merge of piles with positive sizes
        vl b(n), pb(n + 1, 0);
        for (ll &x : b) x = stress::rnd(1, 100);
        for (int i = 0; i < n; i++) pb[i + 1] = pb[i] + b[i];
        auto w = [&](int i, int j) { return pb[j + 1] - pb[i]; };
        vector<vl> e(n, vl(n, 0));
        for (int len = 2; len <= n; len++)
            for (int i = 0; i + len - 1 < n; i++) {
                int j = i + len - 1;
                e[i][j] = INFL;
                for (int m = i; m < j; m++) e[i][j] = min(e[i][j], e[i][m] + e[m + 1][j] + w(i, j));
            }
        CHECK(knuth(n, w) == e[0][n - 1], "knuth n=" + to_string(n));
    }
    puts("OK");
}
