// Stress test for src/ds/fenwick.cpp
// Oracles: plain arrays updated and summed directly (1D, range add, 2D),
// linear scan of prefix sums for lower_bound; O(n) build vs adds; values
// up to 1e12 with range adds over 2e5 positions (no int overflow).
#include "tests/stress/template_lib.h"
#include "src/ds/fenwick.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_fenwick, "src/ds/fenwick.cpp", "example_fenwick");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 60);
        bool nonneg = stress::rnd(0, 1);
        ll V = stress::rnd(0, 1) ? 5 : (ll)1e12;
        vl a(n);
        for (ll &x : a) x = stress::rnd(nonneg ? 0 : -V, V);
        BIT b(a), c(n);
        for (int i = 0; i < n; i++) c.add(i, a[i]);
        RangeBIT rb(n);
        vl ra(n, 0);
        for (int op = 0; op < 200; op++) {
            int l = stress::rnd(0, n - 1), r = stress::rnd(0, n - 1);
            if (l > r) swap(l, r);
            ll v = stress::rnd(nonneg ? 0 : -V, V);
            if (stress::rnd(0, 1)) b.add(l, v), c.add(l, v), a[l] += v;
            if (stress::rnd(0, 1)) {
                rb.add(l, r, v);
                for (int i = l; i <= r; i++) ra[i] += v;
            }
            ll s = 0, s2 = 0;
            for (int i = l; i <= r; i++) s += a[i], s2 += ra[i];
            CHECK(b.sum(l, r) == s && c.sum(l, r) == s, "sum l=" + to_string(l) + " r=" + to_string(r));
            CHECK(rb.sum(l, r) == s2, "range sum");
            if (nonneg) {
                ll q = stress::rnd(0, accumulate(all(a), 0LL) + 2), pre = 0;
                int want = n;
                for (int i = 0; i < n; i++) {
                    pre += a[i];
                    if (pre >= q) { want = i; break; }
                }
                CHECK(b.lower_bound(q) == want, "lower_bound q=" + to_string(q));
            }
        }
        CHECK(b.sum(3, 2) == 0, "empty range");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(200); stress::iter++) {
        int n = stress::rnd(1, 12), m = stress::rnd(1, 12);
        BIT2D t(n, m);
        vector<vl> g(n, vl(m, 0));
        for (int op = 0; op < 100; op++) {
            int x = stress::rnd(0, n - 1), y = stress::rnd(0, m - 1);
            ll v = stress::rnd(-(ll)1e12, (ll)1e12);
            t.add(x, y, v), g[x][y] += v;
            int x1 = stress::rnd(0, n - 1), x2 = stress::rnd(x1, n - 1), y1 = stress::rnd(0, m - 1), y2 = stress::rnd(y1, m - 1);
            ll s = 0;
            for (int i = x1; i <= x2; i++) for (int j = y1; j <= y2; j++) s += g[i][j];
            CHECK(t.sum(x1, y1, x2, y2) == s, "2d sum");
        }
    }
    int n = 200000;  // 40 range adds of 1e9 over 2e5 cells: 8e15 (no int overflow)
    RangeBIT rb(n);
    for (int i = 0; i < 40; i++) rb.add(0, n - 1, 1000000000);
    CHECK(rb.sum(0, n - 1) == 40LL * 1000000000 * n, "big range sum");
    puts("OK");
}
