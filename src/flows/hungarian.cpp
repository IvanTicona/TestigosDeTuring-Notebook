/**
 * Usage: auto [cost, mt] = hungarian(a);  a = n x m, 1 <= n <= m
 *        every row gets a DIFFERENT column, minimizing the total;
 *        mt[i] = column of row i. Max weight: negate the costs.
 * Complexity: O(n^2 m)
 * Source: UMSS Hungarian (e-maxx, 1-indexed copy) + UPB Hungarian +
 *         UPB MinCostMatching; 0-indexed interface: own
 * Note: costs may be negative; |cost| * n must fit in ll. Forbidden pair:
 *       a big cost (check the answer). n > m: transpose. Perfect matching
 *       of min cost on general graphs needs blossom, not this.
 * Status: untested
 */
pair<ll, vi> hungarian(const vector<vl> &a) {
    int n = sz(a), m = sz(a[0]);
    vl u(n + 1, 0), v(m + 1, 0);
    vi p(m + 1, 0), way(m + 1, 0);  // p[j] = row matched to column j
    for (int i = 1; i <= n; i++) {
        p[0] = i;
        int j0 = 0;
        vl mn(m + 1, INFL);
        vector<char> used(m + 1, 0);
        do {
            used[j0] = 1;
            int i0 = p[j0], j1 = 0;
            ll delta = INFL;
            for (int j = 1; j <= m; j++)
                if (!used[j]) {
                    ll cur = a[i0 - 1][j - 1] - u[i0] - v[j];
                    if (cur < mn[j]) mn[j] = cur, way[j] = j0;
                    if (mn[j] < delta) delta = mn[j], j1 = j;
                }
            for (int j = 0; j <= m; j++)
                if (used[j]) u[p[j]] += delta, v[j] -= delta;
                else mn[j] -= delta;
            j0 = j1;
        } while (p[j0]);
        do {
            int j1 = way[j0];
            p[j0] = p[j1], j0 = j1;
        } while (j0);
    }
    vi mt(n);
    ll cost = 0;
    for (int j = 1; j <= m; j++)
        if (p[j]) mt[p[j] - 1] = j - 1, cost += a[p[j] - 1][j - 1];
    return {cost, mt};
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_hungarian() {
    vector<vl> a = {{4, 1, 3}, {2, 0, 5}, {3, 2, 2}};
    auto [c, mt] = hungarian(a);  // rows -> cols 1, 0, 2: 1 + 2 + 2
    cout << c << ' ' << mt[0] << mt[1] << mt[2] << '\n';  // 5 102
    auto [c2, m2] = hungarian({{7, 3, 9, 1}});  // 1 row, 4 columns
    cout << c2 << ' ' << m2[0] << '\n';  // 1 3
}
