// Stress test for src/data_structures/fenwick_tree.cpp (1-indexed)
// Oracle: plain arrays (prefix sums / range adds done element by element).
//
// KNOWN BUG: RURQ::range_update computes `v * (ui - 1)` and `-v * uj` in `int`,
//   so it overflows once |v| * n >= 2^31 (e.g. v = 1e9, n >= 3). The test keeps
//   |v| * m < 2^31 for RURQ.
// Note: update(0, v) loops forever (LSOne(0) == 0); index 0 is never used here.
#include "template/template.cpp"
#include "src/data_structures/fenwick_tree.cpp"
#include "tests/stress/common.h"

static string t_ops(const vector<array<ll, 4>> &ops) {
    string s;
    for (auto &o : ops) s += "(" + to_string(o[0]) + "," + to_string(o[1]) + "," + to_string(o[2]) + "," + to_string(o[3]) + ") ";
    return s;
}

static ll t_sum(const vll &a, int i, int j) { // a is 1-indexed
    ll s = 0;
    for (int k = i; k <= j; k++) s += a[k];
    return s;
}

// Smallest i in [1, m] with prefix(i) >= k, or m if none (non-negative values).
static int t_select(const vll &a, ll k) {
    int m = sz(a) - 1;
    ll s = 0;
    for (int i = 1; i <= m; i++) { s += a[i]; if (s >= k) return i; }
    return m;
}

static void t_check_all(FenwickTree &ft, const vll &a, const string &tag) {
    int m = sz(a) - 1;
    for (int i = 1; i <= m; i++) {
        CHECK(ft.rsq(i) == t_sum(a, 1, i), tag + " rsq(" + to_string(i) + ")");
        for (int j = i; j <= m; j++)
            CHECK(ft.rsq(i, j) == t_sum(a, i, j), tag + " rsq(" + to_string(i) + "," + to_string(j) + ")");
    }
}

int main() {
    // Deterministic edge cases
    {
        FenwickTree ft(1);
        CHECK(ft.rsq(1) == 0, "m=1 empty");
        ft.update(1, 5); ft.update(1, -2);
        CHECK(ft.rsq(1) == 3 && ft.rsq(1, 1) == 3, "m=1 updates");
        CHECK(ft.select(3) == 1 && ft.select(0) == 1, "m=1 select");
        FenwickTree z(0);
        CHECK(z.rsq(0) == 0, "m=0");
        z.update(1, 4); // index past the end is ignored
    }
    {
        vll f = {0, 1, 2, 3, 4, 5};
        FenwickTree ft(f);
        CHECK(ft.rsq(5) == 15 && ft.rsq(2, 4) == 9 && ft.rsq(1, 1) == 1, "classic");
        CHECK(ft.select(1) == 1 && ft.select(3) == 2 && ft.select(15) == 5 && ft.select(100) == 5, "classic select");
        FenwickTree c(5, vi{1, 1, 3, 5, 5, 5});
        CHECK(c.rsq(1) == 2 && c.rsq(2) == 2 && c.rsq(3) == 3 && c.rsq(5) == 6, "freq ctor");
    }

    // Random: FenwickTree via size ctor + updates + queries
    for (int it = 0; it < 400; it++) {
        stress::iter = it;
        int m = (int)stress::rnd(1, it % 10 == 0 ? 300 : 20);
        FenwickTree ft(m);
        vll a(m + 1, 0);
        vector<array<ll, 4>> ops;
        for (int step = 0; step < 60; step++) {
            int kind = (int)stress::rnd(0, 2);
            if (kind == 0) {
                int i = (int)stress::rnd(1, m); ll v = stress::rnd(-1000000000LL, 1000000000LL);
                ops.push_back({0, i, v, 0}); ft.update(i, v); a[i] += v;
            } else if (kind == 1) {
                int i = (int)stress::rnd(1, m), j = (int)stress::rnd(i, m);
                ops.push_back({1, i, j, 0});
                CHECK(ft.rsq(i, j) == t_sum(a, i, j), t_ops(ops));
            } else {
                int j = (int)stress::rnd(1, m);
                ops.push_back({2, j, 0, 0});
                CHECK(ft.rsq(j) == t_sum(a, 1, j), t_ops(ops));
            }
        }
        if (m <= 20) t_check_all(ft, a, "m=" + to_string(m) + " " + t_ops(ops));
    }

    // Random: build(f) / vll ctor, then update + select on non-negative values
    for (int it = 0; it < 400; it++) {
        stress::iter = it;
        int m = (int)stress::rnd(1, it % 10 == 0 ? 300 : 20);
        vll f(m + 1, 0);
        f[0] = stress::rnd(-5, 5) * 777; // slot 0 must be ignored
        ll maxv = stress::rnd(0, 1) ? 5 : 1000000;
        for (int i = 1; i <= m; i++) f[i] = stress::rnd(0, maxv);
        FenwickTree ft(f);
        FenwickTree ft2(1);
        ft2.build(f);
        vll a = f; a[0] = 0;
        string tag = "m=" + to_string(m) + " it=" + to_string(it);
        t_check_all(ft, a, tag);
        for (int j = 1; j <= m; j++) CHECK(ft2.rsq(j) == t_sum(a, 1, j), tag + " build()");
        for (int step = 0; step < 20; step++) {
            int i = (int)stress::rnd(1, m); ll v = stress::rnd(0, maxv);
            ft.update(i, v); a[i] += v;
            ll k = stress::rnd(-2, t_sum(a, 1, m) + 3);
            CHECK(ft.select(k) == t_select(a, k), tag + " select(" + to_string(k) + ")");
        }
    }

    // Random: frequency ctor FenwickTree(m, s)
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        int m = (int)stress::rnd(1, 30);
        int cnt = (int)stress::rnd(0, 40);
        vi s(cnt);
        vll a(m + 1, 0);
        for (auto &x : s) { x = (int)stress::rnd(1, m); a[x]++; }
        FenwickTree ft(m, s);
        t_check_all(ft, a, "freq m=" + to_string(m));
    }

    // Random: RUPQ (range update, point query)
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        int m = (int)stress::rnd(1, it % 10 == 0 ? 300 : 20);
        RUPQ r(m);
        vll a(m + 1, 0);
        vector<array<ll, 4>> ops;
        for (int step = 0; step < 50; step++) {
            if (stress::rnd(0, 1)) {
                int i = (int)stress::rnd(1, m), j = (int)stress::rnd(i, m);
                int v = (int)stress::rnd(-1000000, 1000000);
                ops.push_back({0, i, j, v});
                r.range_update(i, j, v);
                for (int k = i; k <= j; k++) a[k] += v;
            } else {
                int i = (int)stress::rnd(1, m);
                ops.push_back({1, i, 0, 0});
                CHECK(r.point_query(i) == a[i], t_ops(ops));
            }
        }
        for (int i = 1; i <= m; i++) CHECK(r.point_query(i) == a[i], "final " + t_ops(ops));
    }

    // Random: RURQ (range update, range sum) with |v| * m < 2^31 (see KNOWN BUG above)
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        int m = (int)stress::rnd(1, it % 10 == 0 ? 300 : 20);
        RURQ r(m);
        vll a(m + 1, 0);
        vector<array<ll, 4>> ops;
        for (int step = 0; step < 50; step++) {
            int kind = (int)stress::rnd(0, 2);
            if (kind == 0) {
                int i = (int)stress::rnd(1, m), j = (int)stress::rnd(i, m);
                int v = (int)stress::rnd(-1000000, 1000000); // |v| * m <= 3e8 < 2^31
                ops.push_back({0, i, j, v});
                r.range_update(i, j, v);
                for (int k = i; k <= j; k++) a[k] += v;
            } else if (kind == 1) {
                int i = (int)stress::rnd(1, m), j = (int)stress::rnd(i, m);
                ops.push_back({1, i, j, 0});
                CHECK(r.rsq(i, j) == t_sum(a, i, j), t_ops(ops));
            } else {
                int j = (int)stress::rnd(1, m);
                ops.push_back({2, j, 0, 0});
                CHECK(r.rsq(j) == t_sum(a, 1, j), t_ops(ops));
            }
        }
    }
    puts("OK");
}
