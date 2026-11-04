// Stress test for src/dp/li_chao.cpp
// Oracle: min over every inserted line / segment (restricted to its
// interval) evaluated directly, inserts and queries interleaved, ranges
// with negative coordinates and single-point ranges.
#include "tests/stress/template_lib.h"
#include "src/dp/li_chao.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_li_chao, "src/dp/li_chao.cpp", "example_li_chao");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        ll lo = stress::rnd(-1000, 1000), hi = lo + stress::rnd(0, stress::rnd(0, 1) ? 20 : 2000000);
        ll K = stress::rnd(0, 1) ? 3 : 1000000;
        LiChao t(lo, hi);
        vector<array<ll, 4>> fs;  // k, m, l, r
        for (int op = 0; op < 80; op++) {
            int ty = stress::rnd(0, 2);
            if (ty == 0) {
                ll k = stress::rnd(-K, K), m = stress::rnd(-(ll)1e12, (ll)1e12);
                t.add(k, m), fs.pb({k, m, lo, hi});
            } else if (ty == 1) {
                ll k = stress::rnd(-K, K), m = stress::rnd(-(ll)1e12, (ll)1e12);
                ll a = stress::rnd(lo, hi), b = stress::rnd(lo, hi);
                if (a > b) swap(a, b);
                t.add_seg(k, m, a, b), fs.pb({k, m, a, b});
            } else {
                ll x = stress::rnd(lo, hi), want = INFL;
                for (auto [k, m, a, b] : fs) if (a <= x && x <= b) want = min(want, k * x + m);
                CHECK(t.query(x) == want, "x=" + to_string(x) + " lo=" + to_string(lo) + " hi=" + to_string(hi));
            }
        }
    }
    puts("OK");
}
