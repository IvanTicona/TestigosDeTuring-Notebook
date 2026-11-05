/**
 * Usage: kadane(a)        -> {best sum, l, r}: a[l..r] max sum (a and
 *                           the answer non-empty)
 *        max_sum_2d(g)    -> max sum of a non-empty sub-rectangle
 *        max_sum_3d(c)    -> max sum of a non-empty sub-box (c[x][y][z])
 * Complexity: kadane O(n); 2d O(R^2 C); 3d O(X^2 Y^2 Z)
 * Source: UPB Kadane (+ indices) + UMSS Max Sum 1D / 2D (O(n^4) with
 *         prefix sums) / 3D (O(n^6); wrong index acum[x][k][z-1] instead
 *         of acum[h][k][z-1]); reduction to Kadane: own
 * Note: works with all-negative input (best = max element). Max sum with
 *       length in [L, R]: prefix sums + monotonic deque / sparse table.
 *       Circular array: max(kadane(a), total - min subarray), unless all
 *       elements are negative.
 * Status: ok
 */
array<ll, 3> kadane(const vl &a) {
    array<ll, 3> best = {a[0], 0, 0};
    ll cur = 0, l = 0;
    for (int i = 0; i < sz(a); i++) {
        if (cur <= 0) cur = 0, l = i;  // restart at i
        cur += a[i];
        if (cur > best[0]) best = {cur, l, i};
    }
    return best;
}

ll max_sum_2d(const vector<vl> &g) {
    int R = sz(g), C = sz(g[0]);
    ll best = g[0][0];
    for (int top = 0; top < R; top++) {
        vl col(C, 0);  // sums of rows top..bot per column
        for (int bot = top; bot < R; bot++) {
            for (int j = 0; j < C; j++) col[j] += g[bot][j];
            best = max(best, kadane(col)[0]);
        }
    }
    return best;
}

ll max_sum_3d(const vector<vector<vl>> &c) {
    int X = sz(c), Y = sz(c[0]), Z = sz(c[0][0]);
    ll best = c[0][0][0];
    for (int x1 = 0; x1 < X; x1++) {
        vector<vl> plane(Y, vl(Z, 0));  // sums over x in [x1, x2]
        for (int x2 = x1; x2 < X; x2++) {
            for (int y = 0; y < Y; y++)
                for (int z = 0; z < Z; z++) plane[y][z] += c[x2][y][z];
            best = max(best, max_sum_2d(plane));
        }
    }
    return best;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_max_subarray() {
    auto [s, l, r] = kadane({-2, 1, -3, 4, -1, 2, 1, -5, 4});
    cout << s << " [" << l << ", " << r << "]\n";      // 6 [3, 6]
    cout << kadane({-5, -2, -7})[0] << '\n';           // -2
    vector<vl> g = {{0, -2, -7, 0}, {9, 2, -6, 2},
                    {-4, 1, -4, 1}, {-1, 8, 0, -2}};  // rows 1-3, cols 0-1
    cout << max_sum_2d(g) << '\n';                     // 15
}
