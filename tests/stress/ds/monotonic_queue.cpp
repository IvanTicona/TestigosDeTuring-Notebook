// Stress test for src/ds/monotonic_queue.cpp
// Oracles: direct min / max over every window; a std::deque mirror for
// MonoQueue and for Swag with a NON-commutative op (affine composition
// mod p) folded front to back.
#include "tests/stress/template_lib.h"
#include "src/ds/monotonic_queue.cpp"
#include "tests/stress/common.h"

const ll P = 1000003;

int main() {
    stress::check_example(example_monotonic_queue, "src/ds/monotonic_queue.cpp", "example_monotonic_queue");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 60), k = stress::rnd(1, n);
        vl a(n);
        for (ll &x : a) x = stress::rnd(-5, 5);
        vl mn = window_min(a, k), mx = window_min(a, k, greater<ll>());
        CHECK(sz(mn) == n - k + 1 && sz(mx) == n - k + 1, "sizes");
        for (int i = 0; i + k <= n; i++) {
            CHECK(mn[i] == *min_element(a.begin() + i, a.begin() + i + k), "window min");
            CHECK(mx[i] == *max_element(a.begin() + i, a.begin() + i + k), "window max");
        }
    }
    auto comp = [](pll f, pll g) { return pll{f.F * g.F % P, (f.S * g.F + g.S) % P}; };
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        MonoQueue q;
        Swag s(comp, pll{1, 0});
        deque<ll> d;
        deque<pll> ds;
        for (int op = 0; op < 300; op++) {
            if (d.empty() || stress::rnd(0, 2)) {
                ll x = stress::rnd(-20, 20);
                pll f = {stress::rnd(0, P - 1), stress::rnd(0, P - 1)};
                q.push(x), d.pb(x), s.push(f), ds.pb(f);
            } else {
                q.pop(), d.pop_front(), s.pop(), ds.pop_front();
            }
            CHECK(q.size() == sz(d) && s.size() == sz(ds), "sizes");
            if (!d.empty()) CHECK(q.best() == *min_element(all(d)), "MonoQueue min");
            pll want = {1, 0};
            for (auto f : ds) want = comp(want, f);
            CHECK(s.fold() == want, "Swag fold");
        }
    }
    puts("OK");
}
