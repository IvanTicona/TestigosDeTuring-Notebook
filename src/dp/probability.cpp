/**
 * Usage: dice_pmf(n, f)          -> p[s] = P(sum of n fair f-sided dice = s)
 *        dice_count(n, f, m)     -> #outcomes (of f^n) with sum s, mod m
 *        markov(pi, T, k)        -> distribution after k steps (T row-
 *                                   stochastic, doubles, k up to 1e18)
 *        absorb_steps(T, isabs)  -> E[#steps until an absorbing state] from
 *                                   each state (absorption must be certain)
 * Complexity: dice O(n * n f); markov O(S^3 log k); absorb O(S^3)
 * Source: UPB ProbabilityDP_Dice (O(n f * nf)) + UPB MarkovSmall (k
 *         steps one by one, power iteration) + UPB ExpectedValueDP_Rolls;
 *         prefix sums, fast power and absorbing chain: own
 * Note: linearity: E[sum] = sum E (no independence needed). Geometric
 *       with success p: E = 1/p. Coupon collector: n (1 + 1/2 + ... + 1/n).
 *       E[max of n draws <= k] via P(max <= x) = F(x)^n. Answer "P mod p":
 *       same DP with 1/f = invmod(f). E[x] = 1 + sum p_i E[y_i] with
 *       cycles -> linear system (absorb_steps / gauss.cpp).
 * Depends: src/algebra/gauss.cpp
 * Status: ok
 */
vd dice_pmf(int n, int f) {
    vd p = {1};  // sum of 0 dice
    for (int t = 1; t <= n; t++) {
        vd q(t * f + 1, 0), pre(sz(p) + 1, 0);
        for (int s = 0; s < sz(p); s++) pre[s + 1] = pre[s] + p[s];
        for (int s = t; s <= t * f; s++)  // q[s] = sum_{d=1..f} p[s - d] / f
            q[s] = (pre[min(s, sz(p))] - pre[max(0, s - f)]) / f;
        p = q;
    }
    return p;
}

vl dice_count(int n, int f, ll m = MOD) {
    vl p = {1 % m};
    for (int t = 1; t <= n; t++) {
        vl q(t * f + 1, 0), pre(sz(p) + 1, 0);
        for (int s = 0; s < sz(p); s++) pre[s + 1] = (pre[s] + p[s]) % m;
        for (int s = t; s <= t * f; s++)
            q[s] = (pre[min(s, sz(p))] - pre[max(0, s - f)] + m) % m;
        p = q;
    }
    return p;
}

vd markov(vd pi, vector<vd> T, ll k) {
    int n = sz(T);
    auto mul = [&](const vector<vd> &A, const vector<vd> &B) {
        vector<vd> C(n, vd(n, 0));
        for (int i = 0; i < n; i++)
            for (int l = 0; l < n; l++)
                for (int j = 0; j < n; j++) C[i][j] += A[i][l] * B[l][j];
        return C;
    };
    for (; k; k >>= 1, T = mul(T, T))
        if (k & 1) {
            vd r(n, 0);
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++) r[j] += pi[i] * T[i][j];
            pi = r;
        }
    return pi;
}

vd absorb_steps(const vector<vd> &T, const vb &isabs) {
    int n = sz(T);  // E_i - sum_j T_ij E_j = 1 (transient), E_i = 0 (abs)
    vector<vd> a(n, vd(n + 1, 0));
    for (int i = 0; i < n; i++) {
        a[i][i] = 1;
        if (isabs[i]) continue;
        for (int j = 0; j < n; j++) a[i][j] -= T[i][j];
        a[i][n] = 1;
    }
    vd e;
    gauss(a, e);
    return e;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_probability() {
    vd p = dice_pmf(2, 6);  // two dice: P(7) = 6/36
    cout << to_string(p[7]) << ' '  // 0.166667 0.027778
         << to_string(p[2]) << '\n';
    vl c = dice_count(3, 6);
    cout << c[10] << ' ' << c[3] << '\n';  // 27 1
    // walk on 0-1-2, absorbing at 0 and 2; from 1 go left/right 1/2 each
    vector<vd> T = {{1, 0, 0}, {0.5, 0, 0.5}, {0, 0, 1}};
    cout << to_string(absorb_steps(T, {1, 0, 1})[1]) << '\n';  // 1.000000
    vd d = markov({0, 1, 0}, T, 3);
    cout << to_string(d[0]) << ' '  // 0.500000 0.000000
         << to_string(d[1]) << '\n';
}
