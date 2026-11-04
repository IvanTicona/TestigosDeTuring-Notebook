// Stress test for src/dp/lis.cpp
// Oracles: enumeration of all 2^n index subsets (n <= 14) for the longest
// strict / non-strict length and the number of longest strict ones; the
// returned indices must be increasing and form a valid subsequence; O(n^2)
// DP for longer arrays.
#include "tests/stress/template_lib.h"
#include "src/dp/lis.cpp"
#include "tests/stress/common.h"

bool valid(const vl &a, const vi &s, bool strict) {
    for (int i = 1; i < sz(s); i++) {
        if (s[i] <= s[i - 1]) return false;
        if (strict ? a[s[i]] <= a[s[i - 1]] : a[s[i]] < a[s[i - 1]]) return false;
    }
    return true;
}

int main() {
    stress::check_example(example_lis, "src/dp/lis.cpp", "example_lis");
    CHECK(lis({}).empty() && count_lis({}).F == 0, "empty");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        int n = stress::rnd(1, 14);
        ll V = stress::rnd(1, 3) == 1 ? 3 : 1000000000000LL;
        vl a(n);
        for (ll &x : a) x = stress::rnd(-V, V);
        int bs = 0, bn = 0;
        ll cnt = 0;
        for (int mask = 1; mask < (1 << n); mask++) {
            vi s;
            for (int i = 0; i < n; i++) if (mask >> i & 1) s.pb(i);
            if (valid(a, s, true)) {
                if (sz(s) > bs) bs = sz(s), cnt = 0;
                if (sz(s) == bs) cnt++;
            }
            if (valid(a, s, false)) bn = max(bn, sz(s));
        }
        vi s = lis(a), t = lis(a, false);
        CHECK(sz(s) == bs && valid(a, s, true), "strict n=" + to_string(n));
        CHECK(sz(t) == bn && valid(a, t, false), "non-strict n=" + to_string(n));
        CHECK(count_lis(a) == make_pair(bs, cnt), "count n=" + to_string(n));
    }
    for (stress::iter = 0; stress::iter < stress::scaled(200); stress::iter++) {
        int n = stress::rnd(1, 2000);
        vl a(n);
        for (ll &x : a) x = stress::rnd(0, stress::rnd(0, 1) ? 50 : (ll)1e9);
        vi L(n, 1);
        vl C(n, 1);
        int best = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++)
                if (a[j] < a[i]) {
                    if (L[j] + 1 > L[i]) L[i] = L[j] + 1, C[i] = 0;
                    if (L[j] + 1 == L[i]) C[i] = (C[i] + C[j]) % MOD;
                }
            best = max(best, L[i]);
        }
        ll tot = 0;
        for (int i = 0; i < n; i++) if (L[i] == best) tot = (tot + C[i]) % MOD;
        vi s = lis(a);
        CHECK(sz(s) == best && valid(a, s, true), "dp length n=" + to_string(n));
        CHECK(count_lis(a) == make_pair(best, tot), "dp count n=" + to_string(n));
    }
    puts("OK");
}
