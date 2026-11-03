// Stress test for src/data_structures/segment_tree.cpp (0-indexed, range assign + RMQ)
// Oracle: plain array (assign element by element, min by scan).
//
// KNOWN BUG: SegmentTree(int sz) leaves `A` empty, so any update that reaches a
//   leaf writes A[L] out of bounds. The test builds only through SegmentTree(const vi&).
// KNOWN BUG: -1 is used as the "no lazy" / "empty result" sentinel, so values equal
//   to -1 (and negatives in general) give wrong answers. The test uses values in
//   [0, 1e9] only.
// KNOWN BUG: n = 0 crashes (build(1, 0, -1) reads A[0] of an empty vector).
//   The test uses n >= 1.
#include "template/template.cpp"
#include "src/data_structures/segment_tree.cpp"
#include "tests/stress/common.h"

static string t_log(const vi &init, const vector<array<int, 4>> &ops) {
    string s = "A:";
    for (int x : init) s += " " + to_string(x);
    s += " | ops(kind,i,j,val):";
    for (auto &o : ops) s += " (" + to_string(o[0]) + "," + to_string(o[1]) + "," + to_string(o[2]) + "," + to_string(o[3]) + ")";
    return s;
}

static int t_min(const vi &a, int i, int j) { return *min_element(a.begin() + i, a.begin() + j + 1); }

int main() {
    // Deterministic edge cases
    {
        SegmentTree st(vi{5});
        CHECK(st.RMQ(0, 0) == 5, "n=1");
        st.update(0, 0, 0);
        CHECK(st.RMQ(0, 0) == 0, "n=1 assign 0");
        st.update(0, 0, 9);
        CHECK(st.RMQ(0, 0) == 9, "n=1 assign 9");
    }
    {
        SegmentTree st(vi{18, 17, 13, 19, 15, 11, 20});
        CHECK(st.RMQ(0, 6) == 11 && st.RMQ(1, 3) == 13 && st.RMQ(3, 3) == 19 && st.RMQ(4, 5) == 11, "CP4 example");
        st.update(5, 5, 100);
        CHECK(st.RMQ(4, 6) == 15 && st.RMQ(0, 6) == 13, "point assign");
        st.update(0, 6, 7);
        CHECK(st.RMQ(0, 6) == 7 && st.RMQ(2, 4) == 7, "full range assign");
        st.update(1, 2, 0);
        CHECK(st.RMQ(0, 6) == 0 && st.RMQ(3, 6) == 7 && st.RMQ(0, 0) == 7, "assign 0 then query");
    }

    for (int it = 0; it < 1500; it++) {
        stress::iter = it;
        int n = (int)stress::rnd(1, it % 10 == 0 ? 300 : 20);
        int hi = stress::rnd(0, 2) == 0 ? 3 : 1000000000; // small values force ties
        vi init(n);
        for (auto &x : init) x = (int)stress::rnd(0, hi);
        SegmentTree st(init);
        vi a = init;
        vector<array<int, 4>> ops;
        for (int step = 0; step < 60; step++) {
            int i = (int)stress::rnd(0, n - 1), j = (int)stress::rnd(i, n - 1);
            if (stress::rnd(0, 1)) {
                int v = (int)stress::rnd(0, hi);
                ops.push_back({0, i, j, v});
                st.update(i, j, v);
                for (int k = i; k <= j; k++) a[k] = v;
            } else {
                ops.push_back({1, i, j, 0});
                CHECK(st.RMQ(i, j) == t_min(a, i, j), t_log(init, ops));
            }
        }
        for (int i = 0; i < n; i++) CHECK(st.RMQ(i, i) == a[i], "final " + t_log(init, ops));
        CHECK(st.RMQ(0, n - 1) == t_min(a, 0, n - 1), "final " + t_log(init, ops));
    }
    puts("OK");
}
