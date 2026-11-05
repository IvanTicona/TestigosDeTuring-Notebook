/**
 * Usage: first_true(lo, hi, f)   -> smallest x in [lo, hi] with f(x) true
 *                                   (f monotone false..true), hi + 1 if none
 *        real_search(lo, hi, f)  -> boundary of a monotone predicate on
 *                                   reals (f(lo) false, f(hi) true)
 *        argmax_int(lo, hi, f)   -> first argmax of a unimodal f on ints
 *                                   (strictly up, then down; no plateaus
 *                                   except at the top)
 *        argmax_real(lo, hi, f)  -> argmax of a unimodal f on reals
 *        compress(v)             -> v replaced by ranks 0..k-1, returns the
 *                                   sorted distinct values
 *        inversions(v)           -> #pairs i < j with v[i] > v[j]
 * Complexity: O(log range) calls; argmax_real 200 iterations; inversions
 *             O(n log n)
 * Source: UMSS Ternary Search (doubles only) + own (binary search on the
 *         answer, integer ternary by slope, compression, merge count)
 * Note: integer ternary with plateaus away from the max is WRONG: use
 *       the slope f(m) < f(m + 1) only when it is strictly unimodal.
 *       Golden section saves evaluations when f is expensive.
 * Status: ok
 */
template <class F>
ll first_true(ll lo, ll hi, F f) {
    ll r = hi + 1;
    while (lo <= hi) {
        ll m = lo + (hi - lo) / 2;
        if (f(m)) r = m, hi = m - 1;
        else lo = m + 1;
    }
    return r;
}

template <class F>
ld real_search(ld lo, ld hi, F f, int it = 200) {
    while (it--) {
        ld m = (lo + hi) / 2;
        (f(m) ? hi : lo) = m;
    }
    return hi;
}

template <class F>
ll argmax_int(ll lo, ll hi, F f) {  // first m with f(m) >= f(m + 1)
    return first_true(lo, hi - 1, [&](ll m) { return f(m) >= f(m + 1); });
}

template <class F>
ld argmax_real(ld lo, ld hi, F f, int it = 200) {
    while (it--) {
        ld a = lo + (hi - lo) / 3, b = hi - (hi - lo) / 3;
        if (f(a) < f(b)) lo = a;
        else hi = b;
    }
    return (lo + hi) / 2;
}

vl compress(vl &v) {
    vl s = v;
    sort(all(s)), s.erase(unique(all(s)), s.end());
    for (ll &x : v) x = lower_bound(all(s), x) - s.begin();
    return s;
}

ll inversions(vl v) {
    ll cnt = 0;
    vl tmp(sz(v));
    for (int w = 1; w < sz(v); w *= 2)  // bottom-up merge sort
        for (int l = 0; l + w < sz(v); l += 2 * w) {
            int m = l + w, r = min(l + 2 * w, sz(v)), i = l, j = m, k = l;
            while (i < m || j < r)
                if (j == r || (i < m && v[i] <= v[j])) tmp[k++] = v[i++];
                else cnt += m - i, tmp[k++] = v[j++];
            copy(tmp.begin() + l, tmp.begin() + r, v.begin() + l);
        }
    return cnt;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_search() {
    ll sq = first_true(0, 1000000, [](ll x) { return x * x >= 1000; });
    ll top = argmax_int(-100, 100, [](ll x) { return -(x - 7) * (x - 7); });
    cout << sq << ' ' << top << '\n';  // 32 7
    // (32 = ceil(sqrt(1000)); the parabola peaks at 7)
    ld r = real_search(0, 10, [](ld x) { return x * x >= 2; });  // sqrt(2)
    cout << to_string(r) << '\n';  // 1.414214
    vl v = {50, 10, 50, 30};
    vl vals = compress(v);  // v = 2 0 2 1
    ll inv = inversions({3, 1, 2});  // (3, 1), (3, 2)
    cout << v[0] << v[1] << v[2] << v[3] << ' '  // 2021 3 2
         << sz(vals) << ' ' << inv << '\n';
}
