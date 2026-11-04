// Stress test for src/ds/segtree_beats.cpp
// Oracle: plain array with element-wise chmin / chmax / add and direct
// sum / max / min; few distinct values (many ties of max / second max),
// negatives, values up to 1e12; one 2e5-element run with sampled checks.
#include "tests/stress/template_lib.h"
#include "src/ds/segtree_beats.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_segtree_beats, "src/ds/segtree_beats.cpp", "example_segtree_beats");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 60);
        ll V = stress::rnd(0, 1) ? 4 : (ll)1e12;
        vl a(n);
        for (ll &x : a) x = stress::rnd(-V, V);
        Beats b(a);
        for (int op = 0; op < 200; op++) {
            int l = stress::rnd(0, n - 1), r = stress::rnd(l, n - 1), ty = stress::rnd(0, 3);
            ll x = stress::rnd(-V, V);
            if (ty == 0) { b.chmin(l, r, x); for (int i = l; i <= r; i++) a[i] = min(a[i], x); }
            else if (ty == 1) { b.chmax(l, r, x); for (int i = l; i <= r; i++) a[i] = max(a[i], x); }
            else if (ty == 2) { b.add(l, r, x / 2); for (int i = l; i <= r; i++) a[i] += x / 2; }
            else {
                ll s = 0, mx = LLONG_MIN, mn = LLONG_MAX;
                for (int i = l; i <= r; i++) s += a[i], mx = max(mx, a[i]), mn = min(mn, a[i]);
                CHECK(b.sum(l, r) == s && b.qmax(l, r) == mx && b.qmin(l, r) == mn, "query l=" + to_string(l) + " r=" + to_string(r));
            }
        }
    }
    int n = stress::SLOW ? 20000 : 200000;
    vl a(n);
    for (ll &x : a) x = stress::rnd(0, 1000000000);
    Beats b(a);
    for (int op = 0; op < 3000; op++) {
        int l = stress::rnd(0, n - 1), r = stress::rnd(l, min(n - 1, l + 500)), ty = stress::rnd(0, 2);
        ll x = stress::rnd(0, 1000000000);
        if (ty == 0) { b.chmin(l, r, x); for (int i = l; i <= r; i++) a[i] = min(a[i], x); }
        else if (ty == 1) { b.chmax(l, r, x); for (int i = l; i <= r; i++) a[i] = max(a[i], x); }
        else { b.add(l, r, x); for (int i = l; i <= r; i++) a[i] += x; }
        int x1 = stress::rnd(0, n - 1), x2 = stress::rnd(x1, min(n - 1, x1 + 500));
        ll s = 0;
        for (int i = x1; i <= x2; i++) s += a[i];
        CHECK(b.sum(x1, x2) == s, "big sum");
    }
    puts("OK");
}
