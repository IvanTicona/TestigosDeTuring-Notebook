// Stress test for src/dp/knapsack.cpp
// Oracles: enumeration of all choices: 2^n subsets (0/1, by value), all
// count vectors within limits (bounded), recursion over copies (unbounded);
// knap01_items checked for weight <= W and value == optimum.
#include "tests/stress/template_lib.h"
#include "src/dp/knapsack.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_knapsack, "src/dp/knapsack.cpp", "example_knapsack");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        int n = stress::rnd(1, 10), W = stress::rnd(0, 40);
        vl w(n), v(n), c(n);
        for (int i = 0; i < n; i++)
            w[i] = stress::rnd(0, 15), v[i] = stress::rnd(0, 30), c[i] = stress::rnd(0, 3);
        ll best = 0;
        for (int m = 0; m < (1 << n); m++) {
            ll sw = 0, sv = 0;
            for (int i = 0; i < n; i++) if (m >> i & 1) sw += w[i], sv += v[i];
            if (sw <= W) best = max(best, sv);
        }
        CHECK(knap01(w, v, W) == best, "knap01 n=" + to_string(n));
        CHECK(knap_by_value(w, v, W) == best, "by_value n=" + to_string(n));
        vi it = knap01_items(w, v, W);
        ll sw = 0, sv = 0;
        for (int k = 0; k < sz(it); k++) {
            if (k) CHECK(it[k] > it[k - 1], "items sorted, distinct");
            sw += w[it[k]], sv += v[it[k]];
        }
        CHECK(sw <= W && sv == best, "items optimal");
        // bounded: enumerate count vectors
        ll bb = 0;
        vl cur(n, 0);
        auto rec = [&](auto &&self, int i, ll cw, ll cv) -> void {
            if (cw > W) return;
            if (i == n) { bb = max(bb, cv); return; }
            for (ll k = 0; k <= c[i]; k++) self(self, i + 1, cw + k * w[i], cv + k * v[i]);
        };
        rec(rec, 0, 0, 0);
        CHECK(knap_bounded(w, v, c, W) == bb, "bounded n=" + to_string(n));
        // unbounded: items with weight 0 and value > 0 are unbounded -> skip
        vl uw, uv;
        for (int i = 0; i < n; i++) if (w[i] > 0) uw.pb(w[i]), uv.pb(v[i]);
        vl best_u(W + 1, 0);
        for (int x = 1; x <= W; x++)
            for (int i = 0; i < sz(uw); i++)
                if (uw[i] <= x) best_u[x] = max(best_u[x], best_u[x - uw[i]] + uv[i]);
        ll ub = 0;  // independent: recursion over the number of copies
        auto rec2 = [&](auto &&self, int i, ll cw, ll cv) -> void {
            if (i == sz(uw)) { ub = max(ub, cv); return; }
            for (ll k = 0; cw + k * uw[i] <= W; k++) self(self, i + 1, cw + k * uw[i], cv + k * uv[i]);
        };
        if (sz(uw) <= 5) {
            rec2(rec2, 0, 0, 0);
            CHECK(knap_unbounded(uw, uv, W) == ub, "unbounded n=" + to_string(sz(uw)));
        }
        CHECK(knap_unbounded(uw, uv, W) == best_u[W], "unbounded dp");
    }
    // by_value with huge weights
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int n = stress::rnd(1, 12);
        ll W = stress::rnd(0, (ll)1e12);
        vl w(n), v(n);
        for (int i = 0; i < n; i++) w[i] = stress::rnd(0, (ll)3e11), v[i] = stress::rnd(0, 100);
        ll best = 0;
        for (int m = 0; m < (1 << n); m++) {
            ll sw = 0, sv = 0;
            for (int i = 0; i < n; i++) if (m >> i & 1) sw += w[i], sv += v[i];
            if (sw <= W) best = max(best, sv);
        }
        CHECK(knap_by_value(w, v, W) == best, "by_value huge W");
    }
    puts("OK");
}
