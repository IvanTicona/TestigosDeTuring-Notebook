// Stress test for src/flows/lower_bounds.cpp
// Oracle: enumerate every integer flow vector with lo <= f <= hi (n <= 5,
// m <= 6, hi <= 3) and keep those conserving flow at every node except
// s and t (or at every node for circulations); the max value s -> t (net
// outflow of s) must match; the returned f must be one of the valid ones.
#include "tests/stress/template_lib.h"
#include "src/flows/dinic.cpp"
#include "src/flows/lower_bounds.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_lower_bounds, "src/flows/lower_bounds.cpp", "example_lower_bounds");
    for (stress::iter = 0; stress::iter < stress::scaled(2500); stress::iter++) {
        int n = stress::rnd(2, 5), m = stress::rnd(0, 6);
        vector<array<ll, 4>> E;
        for (int i = 0; i < m; i++) {
            ll u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            if (u == v) continue;
            ll lo = stress::rnd(0, 2), hi = lo + stress::rnd(0, 2);
            E.pb({u, v, lo, hi});
        }
        bool circ = stress::rnd(0, 2) == 0;
        int s = circ ? -1 : stress::rnd(0, n - 1), t = circ ? -1 : stress::rnd(0, n - 2);
        if (!circ && t >= s) t++;
        int me = sz(E);
        vl x(me);
        ll best = -1;
        auto valid = [&](const vl &fl, ll &val) {
            vl bal(n, 0);
            for (int i = 0; i < me; i++) {
                if (fl[i] < E[i][2] || fl[i] > E[i][3]) return false;
                bal[E[i][0]] -= fl[i], bal[E[i][1]] += fl[i];
            }
            for (int v = 0; v < n; v++) if (v != s && v != t && bal[v]) return false;
            val = circ ? 0 : -bal[s];
            return true;
        };
        auto rec = [&](auto &&self, int i) -> void {
            if (i == me) {
                ll val;
                if (valid(x, val)) best = max(best, val);
                return;
            }
            for (x[i] = E[i][2]; x[i] <= E[i][3]; x[i]++) self(self, i + 1);
        };
        rec(rec, 0);
        vl f;
        ll F = flow_lower(n, E, s, t, f);
        CHECK(F == best, "value n=" + to_string(n) + " circ=" + to_string(circ));
        if (F >= 0) {
            ll val;
            CHECK(valid(f, val) && val == F, "returned flow is valid");
        }
    }
    puts("OK");
}
