// Stress test for src/algebra/karatsuba.cpp
// Oracle: naive O(n*m) convolution mod m on random vectors (sizes around
// the base-case threshold and unbalanced), worst-case values m - 1.
#include "tests/stress/template_lib.h"
#include "src/algebra/karatsuba.cpp"
#include "tests/stress/common.h"

vl naive(const vl &a, const vl &b, ll m) {
    vl c(sz(a) + sz(b) - 1, 0);
    for (int i = 0; i < sz(a); i++)
        for (int j = 0; j < sz(b); j++) c[i + j] = (c[i + j] + a[i] * b[j]) % m;
    return c;
}

int main() {
    stress::check_example(example_karatsuba, "src/algebra/karatsuba.cpp", "example_karatsuba");
    CHECK(karatsuba({}, {1}, 5).empty(), "empty");
    vl mods = {1, 2, 10, 1000, MOD, MOD2, 1000000000, 2147483647};
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        ll md = mods[stress::rnd(0, sz(mods) - 1)];
        int n = stress::rnd(1, stress::rnd(0, 1) ? 70 : 700), k = stress::rnd(1, 700);
        vl a(n), b(k);
        bool worst = stress::rnd(0, 3) == 0;
        for (ll &x : a) x = worst ? md - 1 : stress::rnd(0, md - 1);
        for (ll &x : b) x = worst ? md - 1 : stress::rnd(0, md - 1);
        CHECK(karatsuba(a, b, md) == naive(a, b, md), "m=" + to_string(md) + " n=" + to_string(n) + " k=" + to_string(k));
    }
    puts("OK");
}
