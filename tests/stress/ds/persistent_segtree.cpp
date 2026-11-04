// Stress test for src/ds/persistent_segtree.cpp
// Oracles: a full copy of the array per version (old versions must stay
// unchanged after later updates, including branching from old versions);
// k-th smallest in ranges by sorting the subarray.
#include "tests/stress/template_lib.h"
#include "src/ds/persistent_segtree.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_persistent_segtree, "src/ds/persistent_segtree.cpp", "example_persistent_segtree");
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        int n = stress::rnd(1, 40);
        PST t(n);
        vi roots = {0};
        vector<vl> ver = {vl(n, 0)};
        for (int op = 0; op < 150; op++) {
            if (stress::rnd(0, 1)) {  // new version from a random old one
                int b = stress::rnd(0, sz(roots) - 1), pos = stress::rnd(0, n - 1);
                ll v = stress::rnd(-(ll)1e12, (ll)1e12);
                roots.pb(t.add(roots[b], pos, v));
                ver.pb(ver[b]), ver.back()[pos] += v;
            } else {
                int k = stress::rnd(0, sz(roots) - 1), l = stress::rnd(0, n - 1), r = stress::rnd(l, n - 1);
                ll s = 0;
                for (int i = l; i <= r; i++) s += ver[k][i];
                CHECK(t.sum(roots[k], l, r) == s, "sum version " + to_string(k));
            }
        }
    }
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        int n = stress::rnd(1, 60), V = stress::rnd(1, 30);
        vi a(n);
        for (int &x : a) x = stress::rnd(0, V - 1);
        PST t(V);
        vi root = {0};
        for (int x : a) root.pb(t.add(root.back(), x, 1));
        for (int q = 0; q < 50; q++) {
            int l = stress::rnd(0, n - 1), r = stress::rnd(l, n - 1), k = stress::rnd(0, r - l);
            vi s(a.begin() + l, a.begin() + r + 1);
            sort(all(s));
            CHECK(t.kth(root[l], root[r + 1], k) == s[k], "kth l=" + to_string(l) + " r=" + to_string(r) + " k=" + to_string(k));
            int x = stress::rnd(0, V - 1);
            CHECK(t.sum(root[r + 1], 0, x) - t.sum(root[l], 0, x) == upper_bound(all(s), x) - s.begin(), "count <= x");
        }
    }
    puts("OK");
}
