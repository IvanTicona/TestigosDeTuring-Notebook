/**
 * Usage: count_not_divisible(N, {d1, d2, ...}) -> #x in [1, N] divisible by
 *          none of the di (any positive di, not necessarily coprime)
 *        count_coprime(x, n) -> #i in [1, x] with gcd(i, n) = 1
 *        exactly_k(S, k), at_least_one(S): S[j] = sum over all j-subsets of
 *          |intersection| (S[0] = universe size) -> #elements in exactly k
 *          sets / in at least one set
 * Complexity: count_not_divisible O(2^m * m); count_coprime O(sqrt n + 2^w)
 *             (w = #distinct primes of n <= 15 for n <= 1e18); exactly_k O(m)
 * Source: UPB InclusionExclusion (count_not_divisible, lcm with overflow
 *         guard) + own generalizations
 * Note: |A1 u ... u Am| = sum_{j>=1} (-1)^(j+1) S_j;
 *       exactly k = sum_{j>=k} (-1)^(j-k) C(j, k) S_j.
 * Depends: src/combinatorics/binomial.cpp
 * Status: ok
 */
ll count_not_divisible(ll N, const vl &d) {
    int m = sz(d);
    ll bad = 0;  // #x divisible by at least one d_i
    for (int mask = 1; mask < (1 << m); mask++) {
        i128 l = 1;
        for (int i = 0; i < m && l <= N; i++)
            if (mask >> i & 1) l = l / gcd((ll)l, d[i]) * d[i];
        if (l > N) continue;  // contributes floor(N / l) = 0
        bad += (__builtin_popcount(mask) & 1 ? 1 : -1) * (N / (ll)l);
    }
    return N - bad;
}

ll count_coprime(ll x, ll n) {
    vl ps;  // distinct primes of n
    for (ll p = 2; p * p <= n; p++)
        if (n % p == 0) { ps.pb(p); while (n % p == 0) n /= p; }
    if (n > 1) ps.pb(n);
    return count_not_divisible(x, ps);
}

ll at_least_one(const vl &S) {
    ll r = 0;
    for (int j = 1; j < sz(S); j++) r += (j & 1 ? 1 : -1) * S[j];
    return r;
}

ll exactly_k(const vl &S, int k) {
    i128 r = 0;
    for (int j = k; j < sz(S); j++)
        r += ((j - k) & 1 ? -1 : 1) * binom_exact(j, k) * S[j];
    return (ll)r;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_inclusion_exclusion() {
    cout << count_not_divisible(100, {2, 3, 5}) << '\n';  // 26
    cout << count_not_divisible(100, {4, 6}) << '\n';     // 67
    cout << count_coprime(1000000, 30) << '\n';           // 266666
    // A1 = {1,2,3}, A2 = {2,3,4}, A3 = {3,4,5} in {1..5}: S = 5 9 5 1
    vl S = {5, 9, 5, 1};
    cout << exactly_k(S, 1) << ' ' << exactly_k(S, 3) << '\n';  // 2 1
    cout << at_least_one(S) << '\n';                      // 5
}
