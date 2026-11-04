// Stress test for src/ds/treap.cpp
// Oracle: std::vector with insert / erase / std::reverse / loops for range
// add and sum, plus block moves (cut [l, r] and paste at the front) done
// with split / join; full contents compared after each step.
#include "tests/stress/template_lib.h"
#include "src/ds/treap.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_treap, "src/ds/treap.cpp", "example_treap");
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        Treap t;
        vl a;
        for (int op = 0; op < 200; op++) {
            int ty = stress::rnd(0, 6), n = sz(a);
            if (ty <= 1 || n == 0) {
                int pos = stress::rnd(0, n);
                ll v = stress::rnd(-(ll)1e9, (ll)1e9);
                t.insert(pos, v), a.insert(a.begin() + pos, v);
            } else {
                int l = stress::rnd(0, n - 1), r = stress::rnd(l, n - 1);
                if (ty == 2) t.erase(l), a.erase(a.begin() + l);
                else if (ty == 3) t.reverse(l, r), reverse(a.begin() + l, a.begin() + r + 1);
                else if (ty == 4) {
                    ll v = stress::rnd(-1000, 1000);
                    t.add(l, r, v);
                    for (int i = l; i <= r; i++) a[i] += v;
                } else if (ty == 5) {
                    ll s = 0;
                    for (int i = l; i <= r; i++) s += a[i];
                    CHECK(t.sum(l, r) == s && t.get(l) == a[l], "sum l=" + to_string(l) + " r=" + to_string(r));
                } else {  // cut [l, r], paste at the front
                    int x, y, z;
                    t.split(t.root, l, x, y), t.split(y, r - l + 1, y, z);
                    t.root = t.join(y, t.join(x, z));
                    vl mid(a.begin() + l, a.begin() + r + 1);
                    a.erase(a.begin() + l, a.begin() + r + 1);
                    a.insert(a.begin(), all(mid));
                }
            }
            CHECK(t.size() == sz(a), "size");
            if (op % 10 == 0) CHECK(t.to_vec() == a, "contents");
        }
        CHECK(t.to_vec() == a, "final contents");
    }
    Treap big;  // 2e5 elements, depth must stay logarithmic
    int n = stress::SLOW ? 20000 : 200000;
    for (int i = 0; i < n; i++) big.insert(i, i);
    big.reverse(0, n - 1);
    CHECK(big.get(0) == n - 1 && big.sum(0, n - 1) == (ll)n * (n - 1) / 2, "big");
    puts("OK");
}
