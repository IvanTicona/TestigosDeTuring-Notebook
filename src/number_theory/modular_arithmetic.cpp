/**
 * Usage: mulmod(a, b, m), powmod(a, e, m), invmod(a, m) (-1 if none),
 *        invmod_prime(a, p), inv_table(n, p), batch_inv(a, p);
 *        Mint<M> x = 5; x /= 3; x.pow(e); x.inv()
 *        dmint::md = m;  (modulus read from input) dmint y = 7;
 * Complexity: mulmod O(1), powmod/invmod O(log m), inv_table O(n),
 *             batch_inv O(n + log p), Mint ops O(1), / and inv O(log M)
 * Source: UPB ModularArithmetic + UTP "mod integer" (consolidated)
 * Note: m up to ~2^63 (__int128); Mint modulus < 2^31, prime for / and inv
 * Status: ok
 */
// ---- 64-bit helpers (any modulus m >= 1) ----
ull mulmod(ull a, ull b, ull m) { return (unsigned __int128)a * b % m; }

ull powmod(ull a, ull e, ull m) {
    ull r = 1 % m; a %= m;
    for (; e; e >>= 1, a = mulmod(a, a, m))
        if (e & 1) r = mulmod(r, a, m);
    return r;
}

// inverse of a mod m via extended Euclid; -1 if gcd(a, m) != 1
ll invmod(ll a, ll m) {
    ll b = a % m; if (b < 0) b += m;  // no (a % m + m): overflows near 2^63
    ll g = m, x = 0, x1 = 1;
    while (b) {  // invariant: x1 * a == b (mod m), x * a == g (mod m)
        ll q = g / b;
        g -= q * b; swap(g, b);
        x -= q * x1; swap(x, x1);
    }
    if (g != 1) return -1;
    return x < 0 ? x + m : x;  // |x| < m
}

// Fermat: p prime, a % p != 0
ll invmod_prime(ll a, ll p) {
    a %= p; if (a < 0) a += p;
    return powmod(a, p - 2, p);
}

// ---- Many inverses (p prime) ----
// inv[i] for all 1 <= i <= n (n < p): inv[i] = -(p / i) * inv[p % i]
vl inv_table(int n, ll p = MOD) {
    vl inv(n + 1, 1);  // inv[0] unused
    for (int i = 2; i <= n; i++) inv[i] = (ll)mulmod(p - p / i, inv[p % i], p);
    return inv;
}
// inverses of arbitrary values (all != 0 mod p) with ONE powmod
vl batch_inv(const vl &a, ll p = MOD) {
    int n = sz(a);
    vl pre(n + 1, 1), res(n), b(n);
    for (int i = 0; i < n; i++) {
        b[i] = a[i] % p; if (b[i] < 0) b[i] += p;
        pre[i + 1] = (ll)mulmod(pre[i], b[i], p);
    }
    ll inv = invmod_prime(pre[n], p);  // 1 / (b0 * b1 * ... * bn-1)
    for (int i = n - 1; i >= 0; i--) {
        res[i] = (ll)mulmod(inv, pre[i], p);
        inv = (ll)mulmod(inv, b[i], p);
    }
    return res;
}

// ---- Modular integer: Mint<M> fixed modulus; Mint<0> set at run time ----
template <int M = MOD>
struct Mint {
    static inline int md = M;  // for Mint<0>: assign before use
    static int m() { return M ? M : md; }  // constant-folded if M != 0
    int v;
    Mint(ll x = 0) { v = (int)(x % m()); if (v < 0) v += m(); }
    // v + o.v - m() never overflows int (v + o.v could, for m() near 2^31)
    Mint &operator+=(Mint o) {
        if ((v += o.v - m()) < 0) v += m();
        return *this;
    }
    Mint &operator-=(Mint o) { if ((v -= o.v) < 0) v += m(); return *this; }
    Mint &operator*=(Mint o) { v = (int)((ll)v * o.v % m()); return *this; }
    Mint pow(ll e) const {  // e < 0: power of the inverse (m prime)
        if (e < 0) return inv().pow(-e);
        Mint r = 1, b = *this;
        for (; e; e >>= 1, b *= b) if (e & 1) r *= b;
        return r;
    }
    Mint inv() const { return pow(m() - 2); }  // md prime
    Mint &operator/=(Mint o) { return *this *= o.inv(); }
    friend Mint operator+(Mint a, Mint b) { return a += b; }
    friend Mint operator-(Mint a, Mint b) { return a -= b; }
    friend Mint operator*(Mint a, Mint b) { return a *= b; }
    friend Mint operator/(Mint a, Mint b) { return a /= b; }
    Mint operator-() const { return Mint(0) - *this; }
    bool operator==(Mint o) const { return v == o.v; }
    bool operator!=(Mint o) const { return v != o.v; }
    friend ostream &operator<<(ostream &os, Mint a) { return os << a.v; }
    friend istream &operator>>(istream &is, Mint &a) {
        ll x; is >> x; a = Mint(x); return is;
    }
};
using mint = Mint<>;    // modulus MOD from the template
using dmint = Mint<0>;  // dmint::md = m; before creating values

// ---- Example: call from solve(); each // is the line it prints ----
void example_modular_arithmetic() {
    ull m61 = (1ULL << 61) - 1, big = 1e18;
    cout << powmod(2, 10, 1000) << '\n';            // 24
    cout << mulmod(big, big, m61) << '\n';          // 1824060670036424877
    cout << invmod(3, 11) << ' ' << invmod(2, 4) << '\n';  // 4 -1
    cout << invmod_prime(3, MOD) << '\n';           // 333333336
    vl it = inv_table(4, 7);                        // 1/1..1/4 mod 7
    cout << it[1] << it[2] << it[3] << it[4] << '\n';  // 1452
    vl bi = batch_inv({3, 5, 6}, 7);
    cout << bi[0] << bi[1] << bi[2] << '\n';        // 536
    mint a = 5, b = 3;
    cout << a / b << ' ' << a - b * 2 << '\n';      // 666666673 1000000006
    cout << mint(10).pow(9) << ' ' << b.inv() * b << '\n';  // 1000000000 1
    dmint::md = 13;                                 // e.g. m read from input
    cout << dmint(20) << ' ' << dmint(1) / 2 << '\n';  // 7 7
}
