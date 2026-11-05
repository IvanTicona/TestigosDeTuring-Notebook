/**
 * Usage: phi(n), mobius(n), divisor_count_sum(n) -> {d(n), sigma(n)}
 *        MultSieve s(n); s.phi[x], s.mu[x], s.d[x], s.sigma[x] (x <= n)
 * Complexity: single n O(sqrt n); MultSieve O(n), ~40 bytes per number
 *             (n <= ~1e7; delete the arrays you do not need)
 * Source: UPB DivisorsTotientMobius + UMSS Mobius (+ its .tex theory) +
 *         UTP totient/sigma; d and sigma in the linear sieve: own
 * Note: sigma(n) <= ~5n for n <= 1e18, fits in ll.
 * Facts: phi(p^k) = p^k - p^(k-1);  mu(n) = 0 if a square divides n,
 *        else (-1)^(#primes);  d(n) = prod (e+1);  sigma(p^k) = 1+p+..+p^k
 *   sum_{d|n} phi(d) = n;   sum_{d|n} mu(d) = [n == 1]
 *   Mobius inversion: g(n) = sum_{d|n} f(d) <=> f(n) = sum_{d|n} mu(d) g(n/d)
 *   sum_{i=1..n} gcd(i, n) = sum_{d|n} d * phi(n/d)
 *   #{1<=i<=n : gcd(i, n) = 1} = phi(n); sum of those i = n*phi(n)/2 (n>1)
 * Status: ok
 */
// ---- Single n, trial division ----
ll phi(ll n) {
    ll r = n;
    for (ll p = 2; p * p <= n; p++)
        if (n % p == 0) { while (n % p == 0) n /= p; r -= r / p; }
    if (n > 1) r -= r / n;
    return r;
}
int mobius(ll n) {
    int m = 1;
    for (ll p = 2; p * p <= n; p++)
        if (n % p == 0) {
            n /= p;
            if (n % p == 0) return 0;  // p^2 divides n
            m = -m;
        }
    return n > 1 ? -m : m;
}
pair<ll, ll> divisor_count_sum(ll n) {  // {d(n), sigma(n)}
    ll cnt = 1, sum = 1;
    for (ll p = 2; p * p <= n; p++)
        if (n % p == 0) {
            ll e = 0, pk = 1, s = 1;
            while (n % p == 0) n /= p, e++, pk *= p, s += pk;
            cnt *= e + 1, sum *= s;
        }
    if (n > 1) cnt *= 2, sum *= n + 1;
    return {cnt, sum};
}

// ---- Linear sieve: phi, mu, d, sigma for every x <= n ----
struct MultSieve {
    vi primes, phi, mu, d, e;  // e[x]: exponent of the smallest prime of x
    vl sigma, sp;              // sp[x]: sigma(p^e) for that prime power
    MultSieve(int n) : phi(n + 1), mu(n + 1), d(n + 1), e(n + 1),
                       sigma(n + 1), sp(n + 1) {
        if (n >= 1) phi[1] = mu[1] = d[1] = 1, sigma[1] = 1;
        vector<bool> comp(n + 1);
        for (int i = 2; i <= n; i++) {
            if (!comp[i]) {
                primes.pb(i);
                phi[i] = i - 1, mu[i] = -1, d[i] = 2, e[i] = 1;
                sigma[i] = sp[i] = i + 1;
            }
            for (int p : primes) {
                if ((ll)p * i > n) break;
                int x = p * i;
                comp[x] = true;
                if (i % p == 0) {  // p is also the smallest prime of i
                    phi[x] = phi[i] * p, mu[x] = 0, e[x] = e[i] + 1;
                    d[x] = d[i] / (e[i] + 1) * (e[x] + 1);
                    sp[x] = 1 + p * sp[i];
                    sigma[x] = sigma[i] / sp[i] * sp[x];
                    break;
                }
                phi[x] = phi[i] * (p - 1), mu[x] = -mu[i], e[x] = 1;
                d[x] = d[i] * 2, sp[x] = p + 1, sigma[x] = sigma[i] * (p + 1);
            }
        }
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_multiplicative_functions() {
    auto [dc, ds] = divisor_count_sum(360);
    cout << phi(36) << ' ' << dc << ' ' << ds << '\n';  // 12 24 1170
    cout << mobius(30) << ' ' << mobius(12) << '\n';  // -1 0
    MultSieve s(12);
    cout << s.phi[12] << ' ' << s.mu[12] << '\n';    // 4 0
    cout << s.d[12] << ' ' << s.sigma[12] << '\n';   // 6 28
}
