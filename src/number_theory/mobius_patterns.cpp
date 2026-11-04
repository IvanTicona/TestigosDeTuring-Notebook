/**
 * Usage: MobiusPrefix mp(N);   // N >= every n, m, n/k used below
 *        mp.coprime_pairs(n, m)  -> #(i <= n, j <= m) with gcd(i, j) = 1
 *        mp.gcd_pairs(n, m, k)   -> same with gcd(i, j) = k
 *        mp.gcd_pairs_unordered(n, m, k) -> (i, j) == (j, i) (HDU 1695)
 *        squarefree_count(n)     -> #square-free x in [1, n]
 *        gcd_pair_counts(a, V)[k] -> #(i < j) with gcd(a_i, a_j) = k, a_i <= V
 *        sum_n_over_gcd(N)[n] = sum_{i=1..n} n / gcd(i, n), all n <= N
 *        pillai(N)[n]         = sum_{i=1..n} gcd(i, n),     all n <= N
 * Complexity: pair counts O(sqrt n) per query; squarefree O(sqrt n);
 *             gcd_pair_counts O(V log V); sum_n_over_gcd, pillai O(N log N)
 * Source: UMSS Mobius examples (3 problem solutions + .tex), turned into
 *         functions. Fixed: UMSS gcd(a_i, a_j) == k counted i <= j; its
 *         square-free .tex code returned the NON square-free count.
 * Note: [gcd(i, j) = 1] = sum_{d | gcd} mu(d)  =>  coprime_pairs(n, m) =
 *       sum_d mu(d) (n/d)(m/d). sum n/gcd(i,n) = sum_{d|n} d*phi(d);
 *       pillai(n) = sum_{d|n} d*phi(n/d).
 * Depends: src/number_theory/multiplicative_functions.cpp
 * Depends: src/number_theory/floor_blocks.cpp
 * Status: untested
 */
struct MobiusPrefix {
    vl pre;  // pre[x] = mu(1) + ... + mu(x)
    MobiusPrefix(int N) : pre(N + 1, 0) {
        MultSieve s(N);
        for (int x = 1; x <= N; x++) pre[x] = pre[x - 1] + s.mu[x];
    }
    ll coprime_pairs(ll n, ll m) const {
        ll r = 0;
        for_floor_blocks2(n, m, [&](ll l, ll rr, ll a, ll b) {
            r += (pre[rr] - pre[l - 1]) * a * b;
        });
        return r;
    }
    ll gcd_pairs(ll n, ll m, ll k) const {
        return k <= 0 ? 0 : coprime_pairs(n / k, m / k);
    }
    ll gcd_pairs_unordered(ll n, ll m, ll k) const {
        if (k <= 0) return 0;
        ll a = min(n, m) / k, b = max(n, m) / k;
        if (a == 0) return 0;
        // pairs with both coordinates <= a were counted twice, except (1, 1)
        return coprime_pairs(a, b) - (coprime_pairs(a, a) - 1) / 2;
    }
};

ll squarefree_count(ll n) {  // sum_{i <= sqrt n} mu(i) * floor(n / i^2)
    ll r = sqrtl((long double)n);
    while (r * r > n) r--;
    while ((r + 1) * (r + 1) <= n) r++;
    MultSieve s((int)r);
    ll cnt = 0;
    for (ll i = 1; i <= r; i++) cnt += s.mu[i] * (n / (i * i));
    return cnt;
}

vl gcd_pair_counts(const vi &a, int V) {
    vl cnt(V + 1, 0), res(V + 1, 0);
    for (int x : a) cnt[x]++;
    for (int k = V; k >= 1; k--) {
        ll m = 0;  // number of a_i divisible by k
        for (int j = k; j <= V; j += k) m += cnt[j];
        res[k] = m * (m - 1) / 2;  // pairs whose gcd is a multiple of k
        for (int j = 2 * k; j <= V; j += k) res[k] -= res[j];
    }
    return res;
}

vl sum_n_over_gcd(int N) {
    MultSieve s(N);
    vl f(N + 1, 0);
    for (int d = 1; d <= N; d++)
        for (int n = d; n <= N; n += d) f[n] += (ll)d * s.phi[d];
    return f;
}

vl pillai(int N) {
    MultSieve s(N);
    vl f(N + 1, 0);
    for (int d = 1; d <= N; d++)
        for (int n = d, q = 1; n <= N; n += d, q++) f[n] += (ll)d * s.phi[q];
    return f;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_mobius_patterns() {
    MobiusPrefix mp(100);
    cout << mp.coprime_pairs(10, 10) << '\n';         // 63
    cout << mp.gcd_pairs(10, 10, 2) << '\n';          // 19
    cout << mp.gcd_pairs_unordered(3, 5, 1) << '\n';  // 9
    cout << squarefree_count(10) << '\n';             // 7
    cout << squarefree_count(100) << '\n';            // 61
    cout << squarefree_count(1e12) << '\n';           // 607927102274
    vl g = gcd_pair_counts({2, 4, 6, 3}, 6);
    cout << g[1] << ' ' << g[2] << ' ' << g[3] << '\n';  // 2 3 1
    cout << sum_n_over_gcd(6)[6] << ' ' << pillai(6)[6] << '\n';  // 21 15
}
