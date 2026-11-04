// Stress test for src/algebra/fibonacci.cpp
// Oracles: iterative Fibonacci mod m (n <= 1e5, many m up to 1e18 with
// __int128 sums); huge n via the identity F_{a+b} = F_a F_{b+1} + F_{a-1} F_b
// on random splits (i128); pisano by brute-force period search (m <= 3000)
// and, for big m, F_pi == 0, F_pi+1 == 1 plus minimality over prime factors.
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/algebra/fibonacci.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_fibonacci, "src/algebra/fibonacci.cpp", "example_fibonacci");
    vl mods = {1, 2, 10, MOD, MOD2, (ll)1e18, (ll)1e18 - 11, 4611686018427387847LL / 5};
    for (ll m : mods) {
        ll a = 0, b = 1 % m;
        for (int n = 0; n <= (stress::SLOW ? 20000 : 100000); n++) {
            if (n < 2000 || n % 97 == 0) CHECK(fib(n, m) == pll(a, b), "fib n=" + to_string(n) + " m=" + to_string(m));
            ll c = (ll)(((i128)a + b) % m);
            a = b, b = c;
        }
    }
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        ll m = mods[stress::rnd(0, sz(mods) - 1)];
        ll x = stress::rnd(1, (ll)5e17), y = stress::rnd(0, (ll)5e17);
        auto [fx, fx1] = fib(x, m);
        auto [fy, fy1] = fib(y, m);
        ll fxm1 = ((fx1 - fx) % m + m) % m;  // F_{x-1}
        ll want = (ll)(((i128)fx * fy1 + (i128)fxm1 * fy) % m);
        CHECK(fib(x + y, m).F == want, "addition rule x=" + to_string(x) + " y=" + to_string(y));
    }
    for (ll m = 1; m <= (stress::SLOW ? 600 : 3000); m++) {
        ll a = 0, b = 1 % m, per = 0;
        do { ll c = (a + b) % m; a = b, b = c, per++; } while (!(a == 0 && b == 1 % m));
        CHECK(pisano(m) == per, "pisano m=" + to_string(m));
    }
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        ll m = stress::rnd(2, (ll)1e10), P = pisano(m);
        CHECK(fib(P, m) == pll(0, 1 % m), "pisano period m=" + to_string(m));
        ll x = P;
        for (ll q = 2; q * q <= x; q++)
            if (x % q == 0) {
                CHECK(fib(P / q, m) != pll(0, 1 % m), "pisano minimal m=" + to_string(m));
                while (x % q == 0) x /= q;
            }
        if (x > 1) CHECK(fib(P / x, m) != pll(0, 1 % m), "pisano minimal m=" + to_string(m));
    }
    puts("OK");
}
