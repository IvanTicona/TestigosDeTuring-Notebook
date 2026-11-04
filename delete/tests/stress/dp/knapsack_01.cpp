// Stress test for src/dp/knapsack_01.cpp (w, v are 1-indexed: index 0 is a dummy)
// Oracle: enumerate every subset of items (n <= 15).
#include "template/template.cpp"
#include "src/dp/knapsack_01.cpp"
#include "tests/stress/common.h"

static string t_desc(int n, int W, const vi &w, const vi &v) {
    string s = "n=" + to_string(n) + " W=" + to_string(W) + " (w,v):";
    for (int i = 1; i <= n; i++) s += " (" + to_string(w[i]) + "," + to_string(v[i]) + ")";
    return s;
}

static int t_brute(int n, int W, const vi &w, const vi &v) {
    int best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int tw = 0, tv = 0;
        for (int i = 0; i < n; i++) if (mask >> i & 1) { tw += w[i + 1]; tv += v[i + 1]; }
        if (tw <= W) best = max(best, tv);
    }
    return best;
}

int main() {
    // Deterministic edge cases
    { vi w = {0}, v = {0}; CHECK(knapsack01(0, 0, w, v) == 0, "n=0 W=0"); CHECK(knapsack01(0, 10, w, v) == 0, "n=0 W=10"); }
    { vi w = {0, 5}, v = {0, 7}; CHECK(knapsack01(1, 0, w, v) == 0, "W=0, w>W");
      CHECK(knapsack01(1, 4, w, v) == 0, "w>W"); CHECK(knapsack01(1, 5, w, v) == 7, "w==W"); }
    { vi w = {0, 0, 0}, v = {0, 3, 4}; CHECK(knapsack01(2, 0, w, v) == 7, "w=0 items, W=0"); }
    { vi w = {0, 1, 3, 4, 5}, v = {0, 1, 4, 5, 7}; CHECK(knapsack01(4, 7, w, v) == 9, "classic"); }

    for (int it = 0; it < 4000; it++) {
        stress::iter = it;
        int n = (int)stress::rnd(0, 15);
        int W = (int)stress::rnd(0, 40);
        int maxw = (int)stress::rnd(1, 50); // sometimes most items exceed W
        vi w(n + 1, 0), v(n + 1, 0);
        for (int i = 1; i <= n; i++) { w[i] = (int)stress::rnd(0, maxw); v[i] = (int)stress::rnd(0, 100); }
        CHECK(knapsack01(n, W, w, v) == t_brute(n, W, w, v), t_desc(n, W, w, v));
    }
    puts("OK");
}
