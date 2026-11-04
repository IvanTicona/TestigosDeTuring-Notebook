/**
 * Usage: divisors(n)          -> all divisors of n (n < 2^63), sorted
 *        divisors_from(pe)    -> same, from {prime, exponent} pairs
 *        divisor_lists(N)[x]  -> divisors of every x <= N, increasing
 * Complexity: divisors: factoring (Pollard) + O(d(n) log d(n));
 *             divisor_lists O(N log N) time and memory (N <= ~1e6)
 * Source: UMSS Get All Divisors (fragment; needed an external fact());
 *         rewritten iteratively; divisor_lists: own
 * Note: d(n) <= 103680 for n <= 1e18 (see MAX_DIVISORS_POW10 in sieves).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Depends: src/number_theory/pollard_rho.cpp
 * Status: untested
 */
vl divisors_from(const vector<pll> &pe) {
    vl ds = {1};
    for (auto [p, e] : pe) {
        int cur = sz(ds);
        ll pk = 1;
        for (int k = 1; k <= e; k++) {  // multiply every old divisor by p^k
            pk *= p;
            for (int i = 0; i < cur; i++) ds.pb(ds[i] * pk);
        }
    }
    sort(all(ds));
    return ds;
}

vl divisors(ll n) { return divisors_from(factor_pe(n)); }

vector<vi> divisor_lists(int N) {
    vector<vi> ds(N + 1);
    for (int i = 1; i <= N; i++)
        for (int j = i; j <= N; j += i) ds[j].pb(i);
    return ds;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_divisors() {
    string s;
    for (ll d : divisors(12)) s += to_string(d) + " ";
    cout << s << '\n';                              // 1 2 3 4 6 12
    cout << sz(divisors(963761198400)) << '\n';     // 6720
    cout << sz(divisors_from({{2, 18}, {5, 18}})) << '\n';  // 361
    auto L = divisor_lists(10);
    cout << sz(L[6]) << ' ' << L[6].back() << '\n'; // 4 6
}
