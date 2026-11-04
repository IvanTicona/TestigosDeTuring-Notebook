// Stress test for src/dp/bitmask_dp.cpp
// Oracles: all permutations (n <= 7) for TSP cycles and Hamiltonian paths
// with missing edges; returned orders re-evaluated; naive O(4^n) subset
// sums; recursive enumeration of all set partitions for partition_dp and
// for min_rides (bin packing).
#include "tests/stress/template_lib.h"
#include "src/dp/bitmask_dp.cpp"
#include "tests/stress/common.h"

ll cost_of(const vector<vl> &d, const vi &p, bool cycle) {
    ll c = 0;
    for (int i = 0; i + 1 < sz(p); i++) {
        if (d[p[i]][p[i + 1]] >= INFL) return -1;
        c += d[p[i]][p[i + 1]];
    }
    if (cycle && sz(p) > 1) {
        if (d[p.back()][p[0]] >= INFL) return -1;
        c += d[p.back()][p[0]];
    }
    return c;
}

int main() {
    stress::check_example(example_bitmask_dp, "src/dp/bitmask_dp.cpp", "example_bitmask_dp");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 7);
        vector<vl> d(n, vl(n));
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) d[i][j] = stress::rnd(0, 4) ? stress::rnd(0, 1000000000) : INFL;
        vi p(n);
        iota(all(p), 0);
        ll bc = INFL, bp = INFL;
        do {
            ll c = cost_of(d, p, false);
            if (c >= 0) bp = min(bp, c);
            ll cc = cost_of(d, p, true);
            if (p[0] == 0 && cc >= 0) bc = min(bc, cc);
        } while (next_permutation(all(p)));
        auto [tc, tp] = tsp(d);
        auto [hc, hp] = ham_path(d);
        CHECK(tc == (bc == INFL ? -1 : bc), "tsp n=" + to_string(n));
        CHECK(hc == (bp == INFL ? -1 : bp), "ham_path n=" + to_string(n));
        if (tc >= 0) {
            CHECK(sz(tp) == n && tp[0] == 0 && cost_of(d, tp, true) == tc, "tsp order");
            CHECK(sz(set<int>(all(tp))) == n, "tsp order is a permutation");
        }
        if (hc >= 0) CHECK(sz(hp) == n && sz(set<int>(all(hp))) == n && cost_of(d, hp, false) == hc, "path order");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(1000); stress::iter++) {
        int n = stress::rnd(0, 8), N = 1 << n;
        vl f(N);
        for (ll &x : f) x = stress::rnd(-1000, 1000);
        vl g = f, h = f;
        sos(g), sos(h, false);
        for (int m = 0; m < N; m++) {
            ll a = 0, b = 0;
            for (int s = 0; s < N; s++) {
                if ((s & m) == s) a += f[s];
                if ((s & m) == m) b += f[s];
            }
            CHECK(g[m] == a && h[m] == b, "sos m=" + to_string(m));
        }
        sos(g, true, true), sos(h, false, true);
        CHECK(g == f && h == f, "sos inverse");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(800); stress::iter++) {
        int n = stress::rnd(0, 8), N = 1 << n;
        vl c(N);
        for (ll &x : c) x = stress::rnd(0, 100);
        vl w(n);
        ll X = stress::rnd(1, 30);
        for (ll &x : w) x = stress::rnd(1, X);
        // all set partitions: element i joins an existing group or a new one
        ll best = INFL;
        int rides = n + 1;
        vi grp(n);
        auto rec = [&](auto &&self, int i, int k) -> void {
            if (i == n) {
                vi masks(k, 0);
                vl load(k, 0);
                for (int j = 0; j < n; j++) masks[grp[j]] |= 1 << j, load[grp[j]] += w[j];
                ll s = 0;
                bool fits = true;
                for (int g2 = 0; g2 < k; g2++) s += c[masks[g2]], fits &= load[g2] <= X;
                best = min(best, s);
                if (fits) rides = min(rides, max(k, 0));
                return;
            }
            for (int g2 = 0; g2 <= k; g2++) grp[i] = g2, self(self, i + 1, max(k, g2 + 1));
        };
        rec(rec, 0, 0);
        CHECK(partition_dp(c) == (n == 0 ? 0 : best), "partition_dp n=" + to_string(n));
        CHECK(min_rides(w, X) == (n == 0 ? 0 : rides), "min_rides n=" + to_string(n));
    }
    puts("OK");
}
