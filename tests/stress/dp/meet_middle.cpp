// Stress test for src/dp/meet_middle.cpp
// Oracle: enumeration of all 2^n subsets (n <= 16, negative values too)
// for the sorted multiset of sums, exact-sum counts and best sum <= W;
// n = 40 with small values checked against an O(n * sum) counting DP.
#include "tests/stress/template_lib.h"
#include "src/dp/meet_middle.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_meet_middle, "src/dp/meet_middle.cpp", "example_meet_middle");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(0, 16);
        ll V = stress::rnd(0, 1) ? 5 : (ll)1e12;
        vl a(n);
        for (ll &x : a) x = stress::rnd(stress::rnd(0, 1) ? -V : 0, V);
        vl all_s;
        for (int m = 0; m < (1 << n); m++) {
            ll s = 0;
            for (int i = 0; i < n; i++) if (m >> i & 1) s += a[i];
            all_s.pb(s);
        }
        sort(all(all_s));
        CHECK(subset_sums_sorted(a) == all_s, "sorted sums n=" + to_string(n));
        for (int t = 0; t < 5; t++) {
            ll S = stress::rnd(0, 1) ? all_s[stress::rnd(0, sz(all_s) - 1)] : stress::rnd(-3 * V, 3 * V);
            ll c = count(all(all_s), S);
            CHECK(mitm_count(a, S) == c, "count S=" + to_string(S));
            ll W = stress::rnd(-3 * V, 3 * V), best = LLONG_MIN;
            for (ll s : all_s) if (s <= W) best = max(best, s);
            CHECK(mitm_best(a, W) == best, "best W=" + to_string(W));
        }
    }
    for (int rep = 0; rep < (stress::SLOW ? 1 : 3); rep++) {
        int n = stress::SLOW ? 32 : 40;
        vl a(n);
        for (ll &x : a) x = stress::rnd(0, 20);
        ll tot = accumulate(all(a), 0LL);
        vl dp(tot + 1, 0);
        dp[0] = 1;
        for (ll x : a)
            for (ll s = tot; s >= x; s--) dp[s] += dp[s - x];
        for (int t = 0; t < 5; t++) {
            ll S = stress::rnd(0, tot);
            CHECK(mitm_count(a, S) == dp[S], "n=40 count S=" + to_string(S));
        }
    }
    puts("OK");
}
