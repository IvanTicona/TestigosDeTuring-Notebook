/**
 * Usage: fib(n, m)   -> {F_n, F_{n+1}} mod m (m up to ~1e18), F_0 = 0
 *        pisano(m)   -> period of F mod m (F_{i + pi(m)} = F_i), m <= ~1e12
 * Complexity: fib O(log n); pisano O(sqrt m + sum over p | m of sqrt p log)
 * Source: UMSS Fast Fibonacci (doubling, MOD only, recursion; identities)
 *         + pisano: own
 * Note: F_2k = F_k (2 F_{k+1} - F_k), F_2k+1 = F_k^2 + F_{k+1}^2.
 *       F_{n+k} = F_k F_{n+1} + F_{k-1} F_n;  gcd(F_m, F_n) = F_gcd(m, n);
 *       Cassini: F_{n-1} F_{n+1} - F_n^2 = (-1)^n;  sum_{i<=n} F_i = F_{n+2}-1;
 *       sum F_i^2 = F_n F_{n+1}.  pi(m) <= 6m; pi(p^e) = p^(e-1) pi(p);
 *       pi(p) | p-1 if p = +-1 (mod 5), | 2(p+1) if p = +-2 (mod 5).
 *       F_(F_n) mod m: inner index mod pisano(m).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: ok
 */
pll fib(ll n, ll m = MOD) {
    if (n == 0) return {0, 1 % m};
    auto [a, b] = fib(n >> 1, m);  // a = F_k, b = F_{k+1}
    ll c = mulmod(a, ((2 * (ull)b + m - a) % m), m);
    ll d = (mulmod(a, a, m) + mulmod(b, b, m)) % m;
    return n & 1 ? pll{d, (c + d) % m} : pll{c, d};
}

ll pisano(ll m) {
    auto factor = [](ll x) {  // trial division
        vpll f;
        for (ll q = 2; q * q <= x; q++)
            if (x % q == 0) {
                int e = 0;
                while (x % q == 0) x /= q, e++;
                f.pb({q, e});
            }
        if (x > 1) f.pb({x, 1});
        return f;
    };
    ll res = 1;
    for (auto [p, e] : factor(m)) {
        ll N = p == 2 ? 3 : p == 5 ? 20
             : (p % 5 == 1 || p % 5 == 4) ? p - 1 : 2 * (p + 1);
        for (auto [q, k] : factor(N))  // smallest d | N with F_d = 0, F_d+1 = 1
            while (N % q == 0 && fib(N / q, p) == pll{0, 1}) N /= q;
        for (int i = 1; i < e; i++) N *= p;
        res = res / __gcd(res, N) * N;
    }
    return res;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_fibonacci() {
    cout << fib(10).F << ' ' << fib(90).F << '\n';  // 55 210345902
    cout << fib((ll)1e18, (ll)1e18).F << '\n';      // 183788299560546875
    cout << pisano(10) << ' ' << pisano(MOD) << '\n';  // 60 2000000016
}
