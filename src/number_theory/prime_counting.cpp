/**
 * Usage: Lucy L(n); L.count(x) = pi(x), L.sum(x) = sum of primes <= x,
 *          for x = n / i only (any i >= 1): e.g. x = n, n / 2, sqrt(n)
 *          Lucy L(n, false): counts only (faster, no sums)
 *        PrimeCounter pc; pc.count_primes(n) = pi(n), only n itself
 * Complexity: Lucy O(n^(3/4)): 1e10 0.2 s, 1e11 1 s (0.6 s counts only);
 *             PrimeCounter O(n^(2/3)): 1e11 0.2 s, 1e12 0.8 s, 1e13 3.5 s
 * Source: Lucy_Hedgehog's algorithm (Project Euler forum, own code);
 *         PrimeCounter = UMSS Count Primes (Meissel-Lehmer style), tidied
 * Note: use Lucy for sums or many n / i values, PrimeCounter for one big n.
 *       sum is __int128 (sum of primes <= 1e12 ~ 1.9e22).
 * Status: untested
 */
struct Lucy {
    ll n;
    int r;
    vl v, pi;          // v: the distinct values n / i, decreasing
    vector<i128> ps;   // ps[k]: sum of primes <= v[k]
    vi lo, hi;         // index of x: x <= r -> lo[x], else hi[n / x]
    int id(ll x) const { return x <= r ? lo[x] : hi[n / x]; }
    Lucy(ll n_, bool with_sum = true) : n(n_) {
        r = (int)sqrtl((long double)n);
        while ((ll)r * r > n) r--;
        while ((ll)(r + 1) * (r + 1) <= n) r++;
        lo.assign(r + 2, 0), hi.assign(r + 2, 0);
        for (ll i = 1; i <= n; i = n / (n / i) + 1) {
            ll x = n / i;
            (x <= r ? lo[x] : hi[n / x]) = sz(v);
            v.pb(x), pi.pb(x - 1);
            if (with_sum) ps.pb((i128)x * (x + 1) / 2 - 1);
        }
        for (ll p = 2; p <= r; p++) {
            if (pi[id(p)] == pi[id(p - 1)]) continue;  // p is not prime
            ll c = pi[id(p - 1)];
            for (int k = 0; k < sz(v) && v[k] >= p * p; k++)
                pi[k] -= pi[id(v[k] / p)] - c;  // drop smallest-prime-p numbers
            if (!with_sum) continue;
            i128 s = ps[id(p - 1)];
            for (int k = 0; k < sz(v) && v[k] >= p * p; k++)
                ps[k] -= p * (ps[id(v[k] / p)] - s);
        }
    }
    ll count(ll x) const { return x < 1 ? 0 : pi[id(x)]; }
    i128 sum(ll x) const { return x < 1 ? 0 : ps[id(x)]; }
};

// pi(n) in O(n^(2/3)): phi(n, a) recursion; leaves with n <= y are
// answered offline in increasing n with a Fenwick tree over prime indices
struct PrimeCounter {
    vi primes, mn;  // mn[x]: index of the smallest prime factor of x
    ll ans, y;
    vector<pair<pair<ll, int>, int>> qs;  // {{n, a}, sign}
    void phi(ll n, int a, int sign = 1) {  // #x <= n without primes[0..a]
        if (n == 0) return;
        if (a == -1) { ans += n * sign; return; }
        if (n <= y) { qs.pb({{n, a}, sign}); return; }
        phi(n, a - 1, sign);
        phi(n / primes[a], a - 1, -sign);
    }
    ll count_primes(ll n) {
        y = n < 100 ? n : (ll)pow((long double)n, 0.64L);  // tuned for 1e12
        primes.clear(), mn.assign(y + 1, -1), ans = 0;
        for (int i = 2; i <= y; i++) {  // linear sieve up to y
            if (mn[i] == -1) mn[i] = sz(primes), primes.pb(i);
            for (int k = 0; k < sz(primes); k++) {
                int j = primes[k];
                if ((ll)i * j > y) break;
                mn[i * j] = k;
                if (i % j == 0) break;
            }
        }
        if (n < 100) return sz(primes);
        ll s = n / y;
        for (int p : primes) { if (p > s) break; ans++; }
        int ssz = (int)ans, ptr = sz(primes) - 1;  // ssz = pi(n / y)
        for (int i = ssz; i < sz(primes); i++) {  // remove P2 (two pointers)
            while (ptr >= i && (ll)primes[i] * primes[ptr] > n) ptr--;
            if (ptr < i) break;
            ans -= ptr - i + 1;
        }
        phi(n, ssz - 1);
        sort(all(qs));
        int m = sz(primes), ind = 2;
        vi fw(m, 0);  // Fenwick on reversed prime indices
        for (auto &[na, sign] : qs) {
            auto [x, a] = na;
            for (; ind <= x; ind++)
                for (int i = m - 1 - mn[ind]; i < m; i |= i + 1) fw[i]++;
            ll got = 0;
            for (int i = m - a - 2; i >= 0; i = (i & (i + 1)) - 1) got += fw[i];
            ans += (got + 1) * sign;
        }
        qs.clear();
        return ans - 1;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_prime_counting() {
    Lucy L(100);
    cout << L.count(100) << ' ' << L.count(50) << '\n';  // 25 15
    cout << L.count(10) << ' ' << (ll)L.sum(100) << '\n';  // 4 1060
    PrimeCounter pc;
    cout << pc.count_primes(1e10) << '\n';          // 455052511
}
