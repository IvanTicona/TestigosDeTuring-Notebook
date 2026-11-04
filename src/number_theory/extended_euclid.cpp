/**
 * Usage: ll g = extgcd(a, b, x, y);  // a*x + b*y == g == gcd(a, b) >= 0
 *        gcd / lcm: use std::gcd(a, b), std::lcm(a, b) (C++17)
 * Complexity: O(log min(|a|, |b|))
 * Source: UPB Euclid.cpp (extended_euclid) + UTP ext euclidean, ported to ll
 * Note: works with negatives and zeros; extgcd(0, 0) = 0 (x = 1, y = 0).
 *       Bounds: |x| <= |b| / g and |y| <= |a| / g, so no overflow.
 *       lcm overflows ll if it is > 9.2e18: compute as a / g * b.
 * Status: untested
 */
ll extgcd(ll a, ll b, ll &x, ll &y) {
    ll x1 = 0, y1 = 1;  // invariants: a == a0*x + b0*y, b == a0*x1 + b0*y1
    x = 1, y = 0;
    while (b) {
        ll q = a / b;
        a -= q * b; swap(a, b);
        x -= q * x1; swap(x, x1);
        y -= q * y1; swap(y, y1);
    }
    if (a < 0) a = -a, x = -x, y = -y;  // make the gcd non-negative
    return a;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_extended_euclid() {
    ll x, y;
    ll g = extgcd(14, 30, x, y);
    cout << g << ' ' << x << ' ' << y << '\n';      // 2 -2 1
    g = extgcd(-6, 4, x, y);
    cout << g << ' ' << x << ' ' << y << '\n';      // 2 -1 -1
    cout << gcd(12, 18) << ' ' << lcm(4, 6) << '\n';  // 6 12
}
