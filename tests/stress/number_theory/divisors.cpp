// Stress test for src/number_theory/divisors.cpp
// Oracle: trial division up to sqrt(n) (sorted list of divisors), for every
// n <= 20000, random n <= 1e12, and highly composite / prime-power n; for
// huge n (no trial division possible): every listed value divides n, the
// list is strictly increasing, and its size equals prod(e + 1).
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/number_theory/pollard_rho.cpp"
#include "src/number_theory/divisors.cpp"
#include "tests/stress/common.h"

static vl divs_trial(ll n) {
    vl lo, hi;
    for (ll i = 1; i * i <= n; i++)
        if (n % i == 0) { lo.pb(i); if (i != n / i) hi.pb(n / i); }
    reverse(all(hi));
    lo.insert(lo.end(), all(hi));
    return lo;
}

int main() {
    stress::check_example(example_divisors, "src/number_theory/divisors.cpp", "example_divisors");
    for (ll n = 1; n <= 20000; n++) CHECK(divisors(n) == divs_trial(n), "n=" + to_string(n));
    auto L = divisor_lists(20000);
    CHECK(L[0].empty(), "lists[0]");
    for (int x = 1; x <= 20000; x++) {
        vl got(all(L[x]));
        CHECK(got == divs_trial(x), "list x=" + to_string(x));
    }
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        ll n = stress::rnd(1, 1000000000000LL);
        CHECK(divisors(n) == divs_trial(n), "n=" + to_string(n));
    }
    for (ll n : {963761198400LL, 897612484786617600LL, 1LL << 62, 999999999999999989LL,
                 1000000007LL * 998244353LL, 2LL * 3 * 5 * 7 * 11 * 13 * 17 * 19 * 23 * 29 * 31 * 37 * 41 * 43 * 47}) {
        vl ds = divisors(n);
        ll want = 1;
        for (auto [p, e] : factor_pe(n)) want *= e + 1;
        bool ok = sz(ds) == want && ds.front() == 1 && ds.back() == n;
        for (int i = 0; i < sz(ds); i++) ok &= n % ds[i] == 0 && (i == 0 || ds[i - 1] < ds[i]);
        CHECK(ok, "n=" + to_string(n));
        if (n <= 1000000000000LL) CHECK(ds == divs_trial(n), "trial n=" + to_string(n));
    }
    CHECK(sz(divisors(897612484786617600LL)) == 103680, "max divisor count below 1e18");
    puts("OK");
}
