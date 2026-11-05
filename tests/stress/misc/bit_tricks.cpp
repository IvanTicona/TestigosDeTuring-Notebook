// Stress test for src/misc/bit_tricks.cpp
// Oracles: next_comb vs scanning upwards for the next number with the same
// popcount (and the full enumeration of k-subsets of n bits in order);
// Gray codes: consecutive codes differ in exactly one bit, all distinct,
// gray_inv(gray(i)) == i also for 64-bit values; submasks vs filtering
// all numbers <= m.
#include "tests/stress/template_lib.h"
#include "src/misc/bit_tricks.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_bit_tricks, "src/misc/bit_tricks.cpp", "example_bit_tricks");
    for (ull x = 1; x < (1 << 16); x++) {
        ull y = x + 1;
        while (__builtin_popcountll(y) != __builtin_popcountll(x)) y++;
        CHECK(next_comb(x) == y, "next_comb x=" + to_string(x));
    }
    for (int n = 1; n <= 12; n++)
        for (int k = 1; k <= n; k++) {
            vector<ull> want, got;
            for (ull m = 0; m < (1ULL << n); m++) if (__builtin_popcountll(m) == k) want.pb(m);
            for (ull m = (1ULL << k) - 1; m < (1ULL << n); m = next_comb(m)) got.pb(m);
            CHECK(got == want, "k-subsets n=" + to_string(n) + " k=" + to_string(k));
        }
    set<ull> seen;
    for (ull i = 0; i < (1 << 14); i++) {
        CHECK(seen.insert(gray(i)).second && gray(i) < (1 << 14), "gray distinct");
        if (i) CHECK(__builtin_popcountll(gray(i) ^ gray(i - 1)) == 1, "one bit");
        CHECK(gray_inv(gray(i)) == i, "inverse");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(100000); stress::iter++) {
        ull i = stress::rng();
        CHECK(gray_inv(gray(i)) == i, "inverse 64-bit");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        ull m = stress::rnd(0, (1 << 12) - 1);
        vector<ull> want, got;
        for (ull s = m + 1; s-- > 0;) if ((s & m) == s) want.pb(s);
        for_submasks(m, [&](ull s) { got.pb(s); });
        CHECK(got == want, "submasks m=" + to_string(m));
    }
    puts("OK");
}
