// Stress test for src/misc/fraction.cpp
// Oracles: arithmetic re-done with __int128 cross products (a/b op c/d =
// x/y checked as x * (denominators) == numerator expression), reduction
// (gcd 1, positive denominator), ordering vs long double for distinct
// values; best_approx vs scanning every denominator q <= N with the two
// closest numerators (exact distance comparison).
#include "tests/stress/template_lib.h"
#include "src/misc/fraction.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_fraction, "src/misc/fraction.cpp", "example_fraction");
    for (stress::iter = 0; stress::iter < stress::scaled(20000); stress::iter++) {
        ll C = stress::rnd(0, 1) ? 20 : 1000000;
        ll a = stress::rnd(-C, C), b = stress::rnd(1, C) * (stress::rnd(0, 1) ? 1 : -1);
        ll c = stress::rnd(-C, C), d = stress::rnd(1, C) * (stress::rnd(0, 1) ? 1 : -1);
        Frac x(a, b), y(c, d);
        for (Frac f : {x, y}) CHECK(f.q > 0 && __gcd(llabs(f.p), f.q) == (f.p ? 1 : f.q), "reduced");
        CHECK((i128)x.p * b == (i128)a * x.q, "value kept");
        Frac s = x + y, m = x * y;
        CHECK((i128)s.p * b * d == ((i128)a * d + (i128)c * b) * s.q, "sum");
        CHECK((i128)m.p * b * d == (i128)a * c * m.q, "product");
        Frac df = x - y;
        CHECK((i128)df.p * b * d == ((i128)a * d - (i128)c * b) * df.q, "difference");
        if (c) {
            Frac q = x / y;
            CHECK((i128)q.p * b * c == (i128)a * d * q.q, "quotient");
        }
        if (!(x == y)) CHECK((x < y) == (x.to_ld() < y.to_ld()), "order");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        ll b = stress::rnd(1, 1000), a = stress::rnd(0, 3000), N = stress::rnd(1, 60);
        Frac f(a, b), got = best_approx(a, b, N);
        CHECK(got.q <= N, "denominator bound");
        auto dist = [&](Frac g) { return g < f ? f - g : g - f; };
        Frac best = got;
        for (ll q = 1; q <= N; q++)
            for (ll p : {(ll)((i128)a * q / b), (ll)((i128)a * q / b) + 1}) {
                Frac g(p, q);
                if (dist(g) < dist(best)) best = g;
            }
        CHECK(dist(best) == dist(got), "closest a=" + to_string(a) + " b=" + to_string(b) + " N=" + to_string(N));
    }
    puts("OK");
}
