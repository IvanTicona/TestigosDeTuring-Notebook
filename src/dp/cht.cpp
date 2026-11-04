/**
 * Usage: LineContainer lc; lc.add(k, m); lc.query(x) -> max of k x + m
 *          (any insertion / query order; for MIN add(-k, -m), -query(x))
 *        MonoCHT h; h.add(k, m) with k NON-INCREASING; h.query(x) -> min
 *          (binary search), h.query_inc(x) for non-decreasing x (O(1) am.)
 * Complexity: LineContainer O(log n); MonoCHT add O(1) amortized
 * Source: UMSS Convex Hull Trick (KACTL LineContainer, tested) + UPB
 *         ConvexHullTrick (long double; equal slopes kept the WORSE line:
 *         p = m > m' ? -inf : inf is reversed); MonoCHT: own
 * Note: dp[i] = min_j (dp[j] + b_j a_i) -> line (b_j, dp[j]), query a_i.
 *       Values k x + m must fit in ll. Li Chao (li_chao.cpp) also adds
 *       segments and needs no division.
 * Status: untested
 */
struct CLine {  // (not "Line": geometry uses that name)
    mutable ll k, m, p;  // p = last x where this line is the best
    bool operator<(const CLine &o) const { return k < o.k; }
    bool operator<(ll x) const { return p < x; }
};

struct LineContainer : multiset<CLine, less<>> {
    static const ll inf = LLONG_MAX;
    ll div(ll a, ll b) { return a / b - ((a ^ b) < 0 && a % b); }  // floor
    bool isect(iterator x, iterator y) {
        if (y == end()) return x->p = inf, 0;
        if (x->k == y->k) x->p = x->m > y->m ? inf : -inf;
        else x->p = div(y->m - x->m, x->k - y->k);
        return x->p >= y->p;
    }
    void add(ll k, ll m) {
        auto z = insert({k, m, 0}), y = z++, x = y;
        while (isect(y, z)) z = erase(z);
        if (x != begin() && isect(--x, y)) isect(x, y = erase(y));
        while ((y = x) != begin() && (--x)->p >= y->p) isect(x, erase(y));
    }
    ll query(ll x) {
        auto l = *lower_bound(x);
        return l.k * x + l.m;
    }
};

struct MonoCHT {  // lower envelope, slopes added in non-increasing order
    vector<pll> L;
    int ptr = 0;
    static ll f(pll l, ll x) { return l.F * x + l.S; }
    static bool bad(pll a, pll b, pll c) {  // b never strictly the best
        return (i128)(c.S - a.S) * (a.F - b.F) <=
               (i128)(b.S - a.S) * (a.F - c.F);
    }
    void add(ll k, ll m) {
        if (!L.empty() && L.back().F == k) {
            if (m >= L.back().S) return;
            L.pop_back();
        }
        while (sz(L) >= 2 && bad(L[sz(L) - 2], L.back(), {k, m})) L.pop_back();
        L.pb({k, m});
        ptr = min(ptr, sz(L) - 1);
    }
    ll query(ll x) const {
        int lo = 0, hi = sz(L) - 1;
        while (lo < hi) {
            int md = (lo + hi) / 2;
            if (f(L[md], x) >= f(L[md + 1], x)) lo = md + 1;
            else hi = md;
        }
        return f(L[lo], x);
    }
    ll query_inc(ll x) {
        while (ptr + 1 < sz(L) && f(L[ptr + 1], x) <= f(L[ptr], x)) ptr++;
        return f(L[ptr], x);
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_cht() {
    LineContainer lc;
    lc.add(1, 0), lc.add(-1, 10), lc.add(0, 3);  // max(x, 10 - x, 3)
    cout << lc.query(0) << ' ' << lc.query(5) << ' '  // 10 5 8
         << lc.query(8) << '\n';
    MonoCHT h;
    h.add(2, 0), h.add(0, 5), h.add(-1, 12);  // min(2x, 5, 12 - x)
    cout << h.query(1) << ' ' << h.query(4) << ' '  // 2 5 2
         << h.query(10) << '\n';
}
