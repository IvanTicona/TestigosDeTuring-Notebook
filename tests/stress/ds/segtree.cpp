// Stress test for src/ds/segtree.cpp
// Oracles: plain array with direct folds for sum, max and a NON-commutative
// monoid (composition of affine maps x -> a x + b mod p, order matters);
// max_right / min_left by linear scans with the same predicate; sizes that
// are not powers of two.
#include "tests/stress/template_lib.h"
#include "src/ds/segtree.cpp"
#include "tests/stress/common.h"

const ll P = 1000003;
struct AffM {  // (a, b): x -> a x + b; op(f, g) = apply f then g
    typedef pll T;
    static T e() { return {1, 0}; }
    static T op(T f, T g) { return {f.F * g.F % P, (f.S * g.F + g.S) % P}; }
};

int main() {
    stress::check_example(example_segtree, "src/ds/segtree.cpp", "example_segtree");
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        int n = stress::rnd(1, 70);
        vl a(n);
        for (ll &x : a) x = stress::rnd(0, 100);
        vector<pll> f(n);
        for (auto &x : f) x = {stress::rnd(0, P - 1), stress::rnd(0, P - 1)};
        SegTree<SumM> s(a);
        SegTree<MaxM> mx(n);
        for (int i = 0; i < n; i++) mx.set(i, a[i]);
        SegTree<AffM> af(f);
        for (int op = 0; op < 150; op++) {
            int i = stress::rnd(0, n - 1);
            if (stress::rnd(0, 1)) {
                a[i] = stress::rnd(0, 100), s.set(i, a[i]), mx.set(i, a[i]);
                f[i] = {stress::rnd(0, P - 1), stress::rnd(0, P - 1)}, af.set(i, f[i]);
            }
            int l = stress::rnd(0, n - 1), r = stress::rnd(-1, n - 1);
            ll sum = 0, m = LLONG_MIN;
            pll comp = {1, 0};
            for (int k = l; k <= r; k++) sum += a[k], m = max(m, a[k]), comp = AffM::op(comp, f[k]);
            CHECK(s.query(l, r) == sum && mx.query(l, r) == m && af.query(l, r) == comp, "query l=" + to_string(l) + " r=" + to_string(r));
            CHECK(s.get(i) == a[i], "get");
            ll lim = stress::rnd(0, 3000);
            auto ok = [&](ll x) { return x <= lim; };
            int L = stress::rnd(0, n), want = L;  // max_right
            for (ll acc = 0; want < n && acc + a[want] <= lim; want++) acc += a[want];
            CHECK(s.max_right(L, ok) == want, "max_right l=" + to_string(L));
            int R = stress::rnd(0, n), wl = R;  // min_left
            for (ll acc = 0; wl > 0 && acc + a[wl - 1] <= lim; wl--) acc += a[wl - 1];
            CHECK(s.min_left(R, ok) == wl, "min_left r=" + to_string(R));
            ll th = stress::rnd(0, 101);
            int w2 = L;
            while (w2 < n && a[w2] < th) w2++;
            CHECK(mx.max_right(L, [&](ll x) { return x < th; }) == w2, "first >= th");
        }
        CHECK(s.total() == accumulate(all(a), 0LL), "all");
    }
    puts("OK");
}
