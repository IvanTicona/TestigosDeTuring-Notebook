// Stress test for src/dp/max_subarray.cpp
// Oracles: all O(n^2) intervals, all O(R^2 C^2) rectangles and all
// O(X^2 Y^2 Z^2) boxes summed directly; kadane's [l, r] must sum to best.
#include "tests/stress/template_lib.h"
#include "src/dp/max_subarray.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_max_subarray, "src/dp/max_subarray.cpp", "example_max_subarray");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        int n = stress::rnd(1, 40);
        ll V = stress::rnd(0, 1) ? 5 : (ll)1e12;
        vl a(n);
        for (ll &x : a) x = stress::rnd(-V, stress::rnd(0, 3) ? V : 0);
        ll best = LLONG_MIN;
        for (int l = 0; l < n; l++) {
            ll s = 0;
            for (int r = l; r < n; r++) s += a[r], best = max(best, s);
        }
        auto [b, l, r] = kadane(a);
        ll s = 0;
        for (ll i = l; i <= r; i++) s += a[i];
        CHECK(b == best && 0 <= l && l <= r && r < n && s == b, "kadane n=" + to_string(n));
    }
    for (stress::iter = 0; stress::iter < stress::scaled(800); stress::iter++) {
        int R = stress::rnd(1, 7), C = stress::rnd(1, 7);
        vector<vl> g(R, vl(C));
        for (auto &row : g) for (ll &x : row) x = stress::rnd(-10, stress::rnd(0, 1) ? 10 : 0);
        ll best = LLONG_MIN;
        for (int a = 0; a < R; a++) for (int b = a; b < R; b++)
            for (int c = 0; c < C; c++) for (int d = c; d < C; d++) {
                ll s = 0;
                for (int i = a; i <= b; i++) for (int j = c; j <= d; j++) s += g[i][j];
                best = max(best, s);
            }
        CHECK(max_sum_2d(g) == best, "2d");
        int X = stress::rnd(1, 4), Y = stress::rnd(1, 4), Z = stress::rnd(1, 4);
        vector<vector<vl>> cube(X, vector<vl>(Y, vl(Z)));
        for (auto &pl : cube) for (auto &row : pl) for (ll &x : row) x = stress::rnd(-10, stress::rnd(0, 1) ? 10 : 0);
        ll b3 = LLONG_MIN;
        for (int x1 = 0; x1 < X; x1++) for (int x2 = x1; x2 < X; x2++)
            for (int y1 = 0; y1 < Y; y1++) for (int y2 = y1; y2 < Y; y2++)
                for (int z1 = 0; z1 < Z; z1++) for (int z2 = z1; z2 < Z; z2++) {
                    ll s = 0;
                    for (int x = x1; x <= x2; x++) for (int y = y1; y <= y2; y++) for (int z = z1; z <= z2; z++) s += cube[x][y][z];
                    b3 = max(b3, s);
                }
        CHECK(max_sum_3d(cube) == b3, "3d");
    }
    puts("OK");
}
