/**
 * Usage: Frac f(p, q) (always reduced, q > 0); + - * / with i128 middle
 *        steps; comparisons exact; f.to_ld()
 *        best_approx(a, b, N) -> fraction p / q with q <= N closest to a / b
 *        (b > 0, a >= 0): continued fraction convergents + semiconvergents
 * Complexity: O(log) per operation (gcd); best_approx O(log b)
 * Source: UTP fraction + UMSS Fraction Modular (mixed % MOD into exact
 *         fractions: neither exact nor modular); best approximation:
 *         Stern-Brocot / continued fractions (own)
 * Note: results must fit in ll after reduction. For probabilities mod p
 *       use Mint instead. Fractions as map keys: compare with operator<.
 * Status: untested
 */
struct Frac {
    ll p, q;
    Frac(ll p_ = 0, ll q_ = 1) {
        ll g = __gcd(llabs(p_), llabs(q_));
        if (q_ < 0) p_ = -p_, q_ = -q_;
        p = g ? p_ / g : 0, q = g ? q_ / g : 1;
    }
    static Frac mk(i128 a, i128 b) {  // reduce in __int128, then narrow
        i128 x = a < 0 ? -a : a, y = b < 0 ? -b : b;
        while (y) x %= y, swap(x, y);
        if (b < 0) a = -a, b = -b;
        return x ? Frac((ll)(a / x), (ll)(b / x)) : Frac(0, 1);
    }
    Frac operator+(Frac o) const {
        return mk((i128)p * o.q + (i128)o.p * q, (i128)q * o.q);
    }
    Frac operator-(Frac o) const {
        return mk((i128)p * o.q - (i128)o.p * q, (i128)q * o.q);
    }
    Frac operator*(Frac o) const { return mk((i128)p * o.p, (i128)q * o.q); }
    Frac operator/(Frac o) const { return mk((i128)p * o.q, (i128)q * o.p); }
    bool operator<(Frac o) const { return (i128)p * o.q < (i128)o.p * q; }
    bool operator==(Frac o) const { return p == o.p && q == o.q; }
    ld to_ld() const { return (ld)p / q; }
};

Frac best_approx(ll a, ll b, ll N) {
    ll h0 = 0, k0 = 1, h1 = 1, k1 = 0;  // last two convergents h / k
    ll x = a, y = b;
    while (y) {
        ll t = x / y;
        if (k1 * t + k0 > N) {  // next convergent too big: semiconvergent
            ll s = (N - k0) / k1;
            Frac c1(h1, k1), c2(s * h1 + h0, s * k1 + k0), f(a, b);
            Frac d1 = c1 < f ? f - c1 : c1 - f, d2 = c2 < f ? f - c2 : c2 - f;
            return d2 < d1 ? c2 : c1;
        }
        ll h2 = t * h1 + h0, k2 = t * k1 + k0;
        h0 = h1, k0 = k1, h1 = h2, k1 = k2;
        ll r = x % y;
        x = y, y = r;
    }
    return Frac(h1, k1);  // a / b itself fits
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_fraction() {
    Frac a(1, 6), b(-3, 4), s = a - b;  // 1/6 + 3/4 = 11/12
    Frac m = a * b;  // -1/8
    cout << s.p << '/' << s.q << ' ' << (b < a) << ' '  // 11/12 1 -1/8
         << m.p << '/' << m.q << '\n';
    Frac pi = best_approx(314159265358979LL, 100000000000000LL, 1000);
    cout << pi.p << '/' << pi.q << '\n';  // 355/113
}
