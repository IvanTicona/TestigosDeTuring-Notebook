/**
 * Usage: min_mean_cycle(n, E) -> {p, q}: minimum over directed cycles of
 *        (total weight / #edges) = p / q exactly (q > 0, reduced);
 *        {0, 0} if the graph is acyclic. E[i] = {u, v, w}
 * Complexity: O(n m) time, O(n^2) memory
 * Source: UPB MinMeanCycle_Karp (double result) + UTP karp min mean
 *         cycle; exact fraction: own
 * Note: Karp: D[k][v] = min weight of a walk with exactly k edges ending at
 *       v (from anywhere); answer = min_v max_k (D[n][v] - D[k][v]) /
 *       (n - k). Max mean: negate weights. Ratio problems (sum a / sum b
 *       over a cycle): binary search lambda + negative cycle check.
 * Status: untested
 */
pll min_mean_cycle(int n, const vector<array<ll, 3>> &E) {
    vector<vl> D(n + 1, vl(n, INFL));
    D[0].assign(n, 0);
    for (int k = 1; k <= n; k++)
        for (auto [u, v, w] : E)
            if (D[k - 1][u] < INFL) D[k][v] = min(D[k][v], D[k - 1][u] + w);
    bool found = false;
    ll bp = 0, bq = 1;
    for (int v = 0; v < n; v++) {
        if (D[n][v] >= INFL) continue;
        ll mp = 0, mq = 0;  // max over k of (D[n][v] - D[k][v]) / (n - k)
        for (int k = 0; k < n; k++) {
            if (D[k][v] >= INFL) continue;
            ll p = D[n][v] - D[k][v], q = n - k;
            if (!mq || (i128)p * mq > (i128)mp * q) mp = p, mq = q;
        }
        if (!found || (i128)mp * bq < (i128)bp * mq)
            bp = mp, bq = mq, found = true;
    }
    if (!found) return {0, 0};
    ll g = __gcd(llabs(bp), bq);
    return {bp / g, bq / g};
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_min_mean_cycle() {
    // cycles: 0->1->0 (mean (2+4)/2 = 3), 1->2->3->1 (mean 7/3)
    vector<array<ll, 3>> E = {{0, 1, 2}, {1, 0, 4}, {1, 2, 1},
                              {2, 3, 1}, {3, 1, 5}};
    auto [p, q] = min_mean_cycle(4, E);
    ll none = min_mean_cycle(2, {{0, 1, 5}}).S;  // acyclic -> {0, 0}
    cout << p << '/' << q << ' ' << none << '\n';  // 7/3 0
}
