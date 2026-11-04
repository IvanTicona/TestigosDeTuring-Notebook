// Stress test for src/number_theory/floor_blocks.cpp
// Oracles: plain loops over every i (block boundaries, values, sums), a
// divisor-count / divisor-sum sieve for sum_floor / sum_sigma, and the
// documented bound (< 2 sqrt(n) blocks) for large n.
#include "tests/stress/template_lib.h"
#include "src/number_theory/floor_blocks.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_floor_blocks, "src/number_theory/floor_blocks.cpp",
                          "example_floor_blocks");
    // ---- blocks partition [1, n] into maximal constant runs ----
    for (ll n = 0; n <= 3000; n++) {
        vector<array<ll, 3>> got, want;
        for_floor_blocks(n, [&](ll l, ll r, ll q) { got.push_back({l, r, q}); });
        for (ll i = 1; i <= n; i++) {
            if (want.empty() || want.back()[2] != n / i) want.push_back({i, i, n / i});
            else want.back()[1] = i;
        }
        CHECK(got == want, "blocks n=" + to_string(n));
    }
    // ---- two-variable blocks ----
    for (int it = 0; it < 3000; it++) {
        stress::iter = it;
        ll n = stress::rnd(0, 2000), m = stress::rnd(0, 2000);
        vector<array<ll, 4>> got, want;
        for_floor_blocks2(n, m, [&](ll l, ll r, ll a, ll b) { got.push_back({l, r, a, b}); });
        for (ll i = 1; i <= min(n, m); i++) {
            if (want.empty() || want.back()[2] != n / i || want.back()[3] != m / i)
                want.push_back({i, i, n / i, m / i});
            else want.back()[1] = i;
        }
        CHECK(got == want, "blocks2 " + to_string(n) + " " + to_string(m));
    }
    // ---- sums vs divisor sieve ----
    const int N = 200000;
    vl d(N + 1, 0), sg(N + 1, 0);
    for (int i = 1; i <= N; i++) for (int j = i; j <= N; j += i) d[j]++, sg[j] += i;
    vl pd(N + 1, 0), ps(N + 1, 0);
    for (int n = 1; n <= N; n++) pd[n] = pd[n - 1] + d[n], ps[n] = ps[n - 1] + sg[n];
    auto check_n = [&](ll n) {
        string in = "n=" + to_string(n);
        CHECK(sum_floor(n) == pd[n] && sum_sigma(n) == ps[n], in);
        if (n <= 3000) {
            ll pm = 0;
            for (ll i = 1; i <= n; i++) pm += n % i;
            CHECK(sum_mod(n) == pm, in + " mod");
        }
    };
    for (ll n = 1; n <= 60000; n++) check_n(n);  // every n
    for (int it = 0; it < 2000; it++) stress::iter = it, check_n(stress::rnd(60000, N));
    // ---- large n (<= 1e12, ~2e6 blocks each): block bound and coverage ----
    for (int it = 0; it < 20; it++) {
        stress::iter = it;
        ll n = stress::rnd(1, 1000000000000LL), cnt = 0, covered = 0, prev_r = 0;
        bool ok = true;
        for_floor_blocks(n, [&](ll l, ll r, ll q) {
            cnt++, covered += r - l + 1;
            ok &= l == prev_r + 1 && n / l == q && n / r == q && (r == n || n / (r + 1) != q);
            prev_r = r;
        });
        string in = "n=" + to_string(n);
        CHECK(ok && covered == n, in + " partition");
        CHECK(cnt <= 2 * (ll)sqrtl((long double)n) + 1, in + " block count");
    }
    puts("OK");
}
