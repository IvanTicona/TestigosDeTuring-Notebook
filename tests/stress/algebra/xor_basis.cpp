// Stress test for src/algebra/xor_basis.cpp
// Oracles: explicit span by closure (set of all XORs of subsets, values
// < 2^12) for rank / contains / max / min / kth; merge vs inserting all;
// GF(2) systems solved by enumerating all 2^m assignments.
#include "tests/stress/template_lib.h"
#include "src/algebra/xor_basis.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_xor_basis, "src/algebra/xor_basis.cpp", "example_xor_basis");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int bits = stress::rnd(1, 12), n = stress::rnd(0, 15);
        XorBasis xb, h1, h2;
        set<ull> span = {0};
        for (int i = 0; i < n; i++) {
            ull v = stress::rnd(0, (1 << bits) - 1);
            bool indep = !span.count(v);
            CHECK(xb.insert(v) == indep, "insert result");
            (i % 2 ? h1 : h2).insert(v);
            set<ull> nx = span;
            for (ull s : span) nx.insert(s ^ v);
            span = nx;
        }
        CHECK(sz(span) == (1 << xb.r), "rank");
        vector<ull> sorted(all(span));
        CHECK(xb.max_xor() == sorted.back(), "max_xor");
        for (int t = 0; t < 20; t++) {
            ull q = stress::rnd(0, (1 << bits) - 1);
            CHECK(xb.contains(q) == (span.count(q) > 0), "contains");
            ull mx = 0, mn = ~0ULL;
            for (ull s : span) mx = max(mx, s ^ q), mn = min(mn, s ^ q);
            CHECK(xb.max_xor(q) == mx && xb.min_xor(q) == mn, "max/min with x");
        }
        for (int k = 0; k < sz(sorted); k++) CHECK(xb.kth(k) == sorted[k], "kth k=" + to_string(k));
        h1.merge(h2);
        CHECK(h1.r == xb.r && h1.max_xor() == xb.max_xor(), "merge");
    }
    // 64-bit values: basis size and dependent inserts
    XorBasis big;
    for (int i = 0; i < 64; i++) CHECK(big.insert(1ULL << i), "unit vectors independent");
    CHECK(big.r == 64 && big.max_xor() == ~0ULL && !big.insert(stress::rng()), "full basis");
    // GF(2) systems
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 8), m = stress::rnd(1, 9);
        vector<bitset<16>> a(n);
        for (auto &r : a) for (int j = 0; j <= m; j++) r[j] = stress::rnd(0, 1);
        int cnt = 0;
        for (int s = 0; s < (1 << m); s++) {
            bool ok = true;
            for (int i = 0; i < n; i++) {
                int v = 0;
                for (int j = 0; j < m; j++) v ^= a[i][j] & (s >> j & 1);
                ok &= v == a[i][m];
            }
            cnt += ok;
        }
        bitset<16> x;
        int k = gauss_xor<16>(a, m, x);
        CHECK(k == (cnt == 0 ? 0 : cnt == 1 ? 1 : 2), "gauss_xor kind n=" + to_string(n) + " m=" + to_string(m));
        if (k)
            for (int i = 0; i < n; i++) {
                int v = 0;
                for (int j = 0; j < m; j++) v ^= a[i][j] & x[j];
                CHECK(v == a[i][m], "gauss_xor solution");
            }
    }
    puts("OK");
}
