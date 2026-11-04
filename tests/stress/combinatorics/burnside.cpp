// Stress test for src/combinatorics/burnside.cpp
// Oracles: brute force canonical forms. Every coloring of n beads with k
// colors (k^n <= 6e4) is reduced to its lexicographically smallest
// rotation (necklaces) or rotation/reflection (bracelets); distinct
// canonical forms are counted. burnside: the same for the cube face group
// generated from two explicit face permutations (closure of the group).
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/number_theory/multiplicative_functions.cpp"
#include "src/combinatorics/burnside.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_burnside, "src/combinatorics/burnside.cpp", "example_burnside");
    for (ll k = 1; k <= 6; k++)
        for (ll n = 1; n <= 14; n++) {
            ll total = 1;
            for (int i = 0; i < n; i++) total *= k;
            if (total > 60000) break;
            set<vi> neck, brace;
            vi c(n, 0);
            for (ll code = 0; code < total; code++) {
                ll t = code;
                for (int i = 0; i < n; i++) c[i] = (int)(t % k), t /= k;
                vi best = c, bestb = c;
                for (int r = 0; r < n; r++) {
                    vi rot(n), ref(n);
                    for (int i = 0; i < n; i++) rot[i] = c[(i + r) % n], ref[i] = c[(r - i + n) % n];
                    best = min(best, rot);
                    bestb = min({bestb, rot, ref});
                }
                neck.insert(best), brace.insert(bestb);
            }
            string in = "n=" + to_string(n) + " k=" + to_string(k);
            CHECK(necklaces(n, k) == (i128)sz(neck), in + " necklaces");
            CHECK(bracelets(n, k) == (i128)sz(brace), in + " bracelets");
            CHECK(necklaces_mod(n, k, MOD) == sz(neck) % MOD, in + " mod");
        }
    // necklaces_mod vs exact for larger n (exact fits while k^n < 2^127)
    for (int it = 0; it < stress::scaled(3000); it++) {
        stress::iter = it;
        ll k = stress::rnd(1, 30), n = stress::rnd(1, 25);
        ll p = it % 2 ? MOD : 998244353;
        CHECK(necklaces_mod(n, k, p) == (ll)(necklaces(n, k) % p), "n=" + to_string(n) + " k=" + to_string(k));
    }
    // cube faces: build the rotation group as permutations of 6 faces
    // (0 up, 1 down, 2 front, 3 back, 4 left, 5 right) from two generators
    {
        vi a = {0, 1, 4, 5, 3, 2};  // rotate around the vertical axis
        vi b = {2, 3, 1, 0, 4, 5};  // rotate around the left-right axis
        set<vi> G = {{0, 1, 2, 3, 4, 5}};
        bool grew = true;
        while (grew) {
            grew = false;
            for (vi g : vector<vi>(all(G)))
                for (const vi &h : {a, b}) {
                    vi gh(6);
                    for (int i = 0; i < 6; i++) gh[i] = g[h[i]];
                    if (G.insert(gh).S) grew = true;
                }
        }
        CHECK(sz(G) == 24, "cube group size");
        vl cyc;
        for (const vi &g : G) {
            vector<bool> seen(6);
            ll c = 0;
            for (int i = 0; i < 6; i++)
                if (!seen[i]) { c++; for (int j = i; !seen[j]; j = g[j]) seen[j] = true; }
            cyc.pb(c);
        }
        for (ll k = 1; k <= 6; k++) {
            set<vi> orb;
            ll total = 1;
            for (int i = 0; i < 6; i++) total *= k;
            for (ll code = 0; code < total; code++) {
                vi col(6);
                ll t = code;
                for (int i = 0; i < 6; i++) col[i] = (int)(t % k), t /= k;
                vi best = col;
                for (const vi &g : G) {
                    vi im(6);
                    for (int i = 0; i < 6; i++) im[i] = col[g[i]];
                    best = min(best, im);
                }
                orb.insert(best);
            }
            CHECK(burnside(cyc, k) == (i128)sz(orb), "cube k=" + to_string(k));
        }
    }
    puts("OK");
}
