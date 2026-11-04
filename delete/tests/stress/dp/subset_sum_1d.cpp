// Stress test for src/dp/subset_sum_1d.cpp
// Oracle: enumerate every subset of a (n <= 15).
#include "template/template.cpp"
#include "src/dp/subset_sum_1d.cpp"
#include "tests/stress/common.h"

static string t_desc(int X, const vi &a) {
    string s = "X=" + to_string(X) + " a:";
    for (int x : a) s += " " + to_string(x);
    return s;
}

static bool t_brute(int X, const vi &a) {
    int n = sz(a);
    for (int mask = 0; mask < (1 << n); mask++) {
        int t = 0;
        for (int i = 0; i < n; i++) if (mask >> i & 1) t += a[i];
        if (t == X) return true;
    }
    return false;
}

int main() {
    // Deterministic edge cases
    { vi a; int n = 0; int X = 0; CHECK(subsetSum1d(X, a) == true, "n=0 X=0"); X = 5; CHECK(subsetSum1d(X, a) == false, "n=0 X=5"); }
    { vi a = {3, 34, 4, 12, 5, 2}; int n = sz(a); int X = 9; CHECK(subsetSum1d(X, a) == true, "classic 9");
      X = 30; CHECK(subsetSum1d(X, a) == false, "classic 30"); X = 0; CHECK(subsetSum1d(X, a) == true, "X=0"); }
    { vi a = {0, 0, 7}; int n = sz(a); int X = 7; CHECK(subsetSum1d(X, a) == true, "zeros 7");
      X = 0; CHECK(subsetSum1d(X, a) == true, "zeros 0"); X = 1; CHECK(subsetSum1d(X, a) == false, "zeros 1"); }
    { vi a = {10, 20}; int n = sz(a); int X = 5; CHECK(subsetSum1d(X, a) == false, "a_i > X"); }

    for (int it = 0; it < 4000; it++) {
        stress::iter = it;
        int n = (int)stress::rnd(0, 15);
        int maxa = (int)stress::rnd(1, 30);
        vi a(n);
        for (auto &x : a) x = (int)stress::rnd(0, maxa); // zeros and a_i > X both occur
        int X = (int)stress::rnd(0, 60);
        CHECK(subsetSum1d(X, a) == t_brute(X, a), t_desc(X, a));
    }
    puts("OK");
}
