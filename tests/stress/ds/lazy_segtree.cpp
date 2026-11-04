// Stress test for src/ds/lazy_segtree.cpp
// Oracle: plain array; range assign / range add applied element by element,
// sum and min folded directly; values up to 1e12, sizes 1..80; one run on
// 2e5 elements with sampled checks.
#include "tests/stress/template_lib.h"
#include "src/ds/lazy_segtree.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_lazy_segtree, "src/ds/lazy_segtree.cpp", "example_lazy_segtree");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 80);
        ll V = stress::rnd(0, 1) ? 5 : (ll)1e12;
        vl a(n);
        for (ll &x : a) x = stress::rnd(-V, V);
        LazySeg st(a);
        for (int op = 0; op < 200; op++) {
            int l = stress::rnd(0, n - 1), r = stress::rnd(l, n - 1);
            int ty = stress::rnd(0, 2);
            ll v = stress::rnd(-V, V);
            if (ty == 0) {
                st.update(l, r, Tag{true, v, 0});
                for (int i = l; i <= r; i++) a[i] = v;
            } else if (ty == 1) {
                st.update(l, r, Tag{false, 0, v});
                for (int i = l; i <= r; i++) a[i] += v;
            } else {
                ll s = 0, m = INFL;
                for (int i = l; i <= r; i++) s += a[i], m = min(m, a[i]);
                Node q = st.query(l, r);
                CHECK(q.sum == s && q.mn == m && q.len == r - l + 1, "query l=" + to_string(l) + " r=" + to_string(r));
            }
        }
    }
    int n = stress::SLOW ? 20000 : 200000;
    vl a(n, 0);
    LazySeg st(a);
    for (int op = 0; op < 2000; op++) {
        int l = stress::rnd(0, n - 1), r = stress::rnd(l, min(n - 1, l + 300));
        ll v = stress::rnd(-1000000000, 1000000000);
        if (stress::rnd(0, 1)) {
            st.update(l, r, Tag{true, v, 0});
            for (int i = l; i <= r; i++) a[i] = v;
        } else {
            st.update(l, r, Tag{false, 0, v});
            for (int i = l; i <= r; i++) a[i] += v;
        }
        int x = stress::rnd(0, n - 1), y = stress::rnd(x, min(n - 1, x + 300));
        ll s = 0;
        for (int i = x; i <= y; i++) s += a[i];
        CHECK(st.query(x, y).sum == s, "big sum");
    }
    puts("OK");
}
