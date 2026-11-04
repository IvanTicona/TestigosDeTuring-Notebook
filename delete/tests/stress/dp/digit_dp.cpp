// Stress test for src/dp/digit_dp.cpp
// Counts numbers in [1, N] whose digit sum is divisible by 3.
// Oracles: (1) naive digit-sum loop for N <= 1e5; (2) closed form floor(N/3),
// since digit sum == N (mod 3).
//
// KNOWN BUG: the snippet counts with `int` (dp table, return value, `int &res`),
// so the count overflows for N >~ 6.4e9. The test keeps N <= 1e9 (count <= 3.4e8).
#include "template/template.cpp"
#include "src/dp/digit_dp.cpp"
#include "tests/stress/common.h"

// Runs the snippet the documented way: s = to_string(N), reset dp, solve(0,0,true,false).
static int t_count(ll N) {
    s = to_string(N);
    n = sz(s);
    memset(dp, -1, sizeof(dp));
    return solve(0, 0, true, false);
}

static int t_naive(int N) {
    int c = 0;
    for (int x = 1; x <= N; x++) {
        int d = 0;
        for (int y = x; y; y /= 10) d += y % 10;
        c += (d % 3 == 0);
    }
    return c;
}

int main() {
    // Deterministic edge cases (N = 0 counts nothing: 0 is not "started")
    CHECK(t_count(0) == 0, "N=0");
    CHECK(t_count(1) == 0, "N=1");
    CHECK(t_count(2) == 0, "N=2");
    CHECK(t_count(3) == 1, "N=3");
    CHECK(t_count(9) == 3, "N=9");
    CHECK(t_count(12) == 4, "N=12");
    ll p = 1;
    for (int k = 0; k <= 9; k++, p *= 10) CHECK(t_count(p) == p / 3, "N=10^" + to_string(k));
    CHECK(t_count(999999999) == 333333333, "N=999999999");
    CHECK(t_count(1000000000) == 333333333, "N=1e9");

    // Every N <= 2000 against the naive oracle, with an incremental naive count
    {
        int c = 0;
        for (int x = 1; x <= 2000; x++) {
            int d = 0;
            for (int y = x; y; y /= 10) d += y % 10;
            c += (d % 3 == 0);
            CHECK(t_count(x) == c, "N=" + to_string(x));
        }
    }

    // Random N <= 1e5: naive oracle
    for (int it = 0; it < 100; it++) {
        stress::iter = it;
        int N = (int)stress::rnd(1, 100000);
        CHECK(t_count(N) == t_naive(N), "N=" + to_string(N));
    }

    // Random N <= 1e9: closed form (dp is reset on every call)
    for (int it = 0; it < 3000; it++) {
        stress::iter = it;
        ll N = stress::rnd(0, 1000000000);
        CHECK(t_count(N) == N / 3, "N=" + to_string(N));
    }
    puts("OK");
}
