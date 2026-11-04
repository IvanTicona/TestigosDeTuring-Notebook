/**
 * Usage: diophantine(a, b, c, x, y, g)  -> one (x, y) with a*x + b*y = c
 *        count_solutions(a, b, c, x1, x2, y1, y2) -> #(x, y) in the box
 *        lin_congruence(a, b, n, x, step) -> a*x == b (mod n), all solutions
 *        are x + k*step (0 <= x < step), or false if none
 * Complexity: O(log min(|a|, |b|))
 * Source: UTP find_any/all_solutions (cp-algorithms) + UPB linear
 *         congruence; rewritten for signs, zeros and overflow
 * Note: all solutions: x + k*(b/g), y - k*(a/g). If b != 0, the x returned
 *       is the smallest non-negative one. |a|, |b|, |c| up to ~1e18 (__int128).
 * Depends: src/number_theory/extended_euclid.cpp
 * Status: untested
 */
bool diophantine(ll a, ll b, ll c, ll &x, ll &y, ll &g) {
    if (a == 0 && b == 0) { x = y = g = 0; return c == 0; }
    ll x0, y0;
    g = extgcd(a, b, x0, y0);
    if (c % g) return false;
    if (b == 0) { x = c / a; y = 0; return true; }
    ll bg = llabs(b / g);  // x is unique modulo |b/g|
    x = (ll)(((__int128)x0 * (c / g) % bg + bg) % bg);
    y = (ll)(((__int128)c - (__int128)a * x) / b);
    return true;
}

// k range such that lo <= base + k*d <= hi (d != 0); returns false if empty
bool dio_k_range(__int128 base, __int128 d, __int128 lo, __int128 hi,
                 __int128 &kl, __int128 &kr) {
    auto fdiv = [](__int128 p, __int128 q) {  // floor(p / q)
        __int128 r = p / q; return r - ((p % q != 0) && ((p < 0) != (q < 0)));
    };
    auto cdiv = [&](__int128 p, __int128 q) { return -fdiv(-p, q); };
    if (d > 0) kl = cdiv(lo - base, d), kr = fdiv(hi - base, d);
    else kl = cdiv(hi - base, d), kr = fdiv(lo - base, d);
    return kl <= kr;
}

// number of integer (x, y), x1 <= x <= x2, y1 <= y <= y2, a*x + b*y = c
ll count_solutions(ll a, ll b, ll c, ll x1, ll x2, ll y1, ll y2) {
    if (x1 > x2 || y1 > y2) return 0;
    if (a == 0 && b == 0) return c == 0 ? (x2 - x1 + 1) * (y2 - y1 + 1) : 0;
    if (a == 0) {  // y = c / b fixed, any x
        if (c % b || c / b < y1 || c / b > y2) return 0;
        return x2 - x1 + 1;
    }
    if (b == 0) {
        if (c % a || c / a < x1 || c / a > x2) return 0;
        return y2 - y1 + 1;
    }
    ll x, y, g;
    if (!diophantine(a, b, c, x, y, g)) return 0;
    // k such that x + k*(b/g) in [x1, x2] and y - k*(a/g) in [y1, y2]
    __int128 l1, r1, l2, r2;
    if (!dio_k_range(x, b / g, x1, x2, l1, r1)) return 0;
    if (!dio_k_range(y, -(a / g), y1, y2, l2, r2)) return 0;
    return (ll)max<__int128>(0, min(r1, r2) - max(l1, l2) + 1);
}

// a*x == b (mod n), n >= 1
bool lin_congruence(ll a, ll b, ll n, ll &x, ll &step) {
    ll y, g;
    if (!diophantine(a, n, b, x, y, g)) return false;  // a*x + n*y = b
    step = n / g;
    x %= step;
    return true;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_diophantine() {
    ll x, y, g;
    diophantine(7, 2, 5, x, y, g);
    cout << x << ' ' << y << ' ' << g << '\n';      // 1 -1 1
    cout << diophantine(4, 6, 5, x, y, g) << '\n';  // 0
    cout << count_solutions(1, 1, 10, 0, 10, 0, 10) << '\n';  // 11
    ll st;
    lin_congruence(14, 30, 100, x, st);             // 14x = 30 (mod 100)
    cout << x << ' ' << st << '\n';                 // 45 50
}
