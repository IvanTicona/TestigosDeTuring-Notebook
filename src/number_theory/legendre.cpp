/**
 * Usage: vp_fact(n, p)  -> exponent of prime p in n! (Legendre's formula)
 *        trailing_zeros_fact(n, b) -> trailing zeros of n! in base b
 *        vp_binom(n, k, p) -> exponent of prime p in C(n, k)
 * Complexity: vp_fact O(log_p n); trailing_zeros_fact O(sqrt b + log n)
 * Warning: NOT named legendre(): C++17 has std::legendre (polynomials) and
 *          with using namespace std it silently wins overload resolution.
 * Source: UTP highest exponent factorial (int, t *= p could overflow);
 *         rewritten dividing n instead; trailing zeros and Kummer: own
 * Note: vp_fact(n, p) = sum_{k>=1} floor(n / p^k) = (n - s_p(n)) / (p - 1),
 *       s_p = digit sum in base p. Kummer: vp_binom = number of carries
 *       when adding k and n - k in base p.
 * Status: ok
 */
ll vp_fact(ll n, ll p) {
    ll e = 0;
    while (n) n /= p, e += n;
    return e;
}

ll trailing_zeros_fact(ll n, ll b) {  // b >= 2
    ll best = LLONG_MAX;
    for (ll p = 2; p * p <= b; p++)
        if (b % p == 0) {
            int e = 0;
            while (b % p == 0) b /= p, e++;
            best = min(best, vp_fact(n, p) / e);
        }
    if (b > 1) best = min(best, vp_fact(n, b));
    return best;
}

ll vp_binom(ll n, ll k, ll p) {  // 0 <= k <= n
    return vp_fact(n, p) - vp_fact(k, p) - vp_fact(n - k, p);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_legendre() {
    cout << vp_fact(100, 5) << ' ' << vp_fact(100, 2) << '\n';  // 24 97
    cout << trailing_zeros_fact(100, 10) << '\n';   // 24
    cout << trailing_zeros_fact(100, 12) << '\n';   // 48
    cout << vp_binom(1000, 500, 2) << '\n';         // 6
    cout << vp_binom(1000, 500, 3) << '\n';         // 4
}
