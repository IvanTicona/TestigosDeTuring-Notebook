// Stress test for src/ds/sparse_table.cpp
// Oracles: direct folds over every interval [l, r] (n <= 70) for min, max,
// gcd, sum, and a non-commutative op (2x2 matrix product mod p, as an
// affine composition) in the disjoint table; random sizes incl. 1 and
// powers of two +- 1.
#include "tests/stress/template_lib.h"
#include "src/ds/sparse_table.cpp"
#include "tests/stress/common.h"

const ll P = 1000003;

int main() {
    stress::check_example(example_sparse_table, "src/ds/sparse_table.cpp", "example_sparse_table");
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        int n = stress::rnd(0, 3) ? stress::rnd(1, 70) : (1 << stress::rnd(0, 6)) + stress::rnd(-1, 1);
        n = max(n, 1);
        vl a(n);
        for (ll &x : a) x = stress::rnd(-1000, 1000) * (stress::rnd(0, 1) ? 1 : 6);
        vector<pll> fs(n);
        for (auto &x : fs) x = {stress::rnd(0, P - 1), stress::rnd(0, P - 1)};
        auto comp = [](pll f, pll g) { return pll{f.F * g.F % P, (f.S * g.F + g.S) % P}; };
        Sparse mn(a, [](ll x, ll y) { return min(x, y); });
        Sparse mx(a, [](ll x, ll y) { return max(x, y); });
        Sparse gc(a, [](ll x, ll y) { return __gcd(llabs(x), llabs(y)); });
        DisjointSparse sm(a, [](ll x, ll y) { return x + y; });
        DisjointSparse af(fs, comp);
        for (int l = 0; l < n; l++) {
            ll m1 = a[l], m2 = a[l], g = llabs(a[l]), s = 0;
            pll c = {1, 0};
            for (int r = l; r < n; r++) {
                m1 = min(m1, a[r]), m2 = max(m2, a[r]), g = __gcd(g, llabs(a[r])), s += a[r];
                c = comp(c, fs[r]);
                CHECK(mn.query(l, r) == m1 && mx.query(l, r) == m2, "min/max");
                CHECK(gc.query(l, r) == g, "gcd");
                CHECK(sm.query(l, r) == s && af.query(l, r) == c, "disjoint l=" + to_string(l) + " r=" + to_string(r));
            }
        }
    }
    puts("OK");
}
