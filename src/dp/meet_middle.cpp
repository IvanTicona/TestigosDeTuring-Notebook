/**
 * Usage: subset_sums_sorted(a) -> all 2^n subset sums, sorted
 *        mitm_count(a, S)      -> #subsets (by index) with sum exactly S
 *        mitm_best(a, W)       -> max subset sum <= W (LLONG_MIN if none)
 * Complexity: subset_sums_sorted O(2^n) (merging); mitm O(2^(n/2) n)
 *             -> n = 40 in ~0.1 s
 * Source: UPB MeetInTheMiddle (O(2^n n) generation, sorts) + own (sorted
 *         generation by merging, two pointers)
 * Note: values may be negative. Same idea for XOR (hash map of one half),
 *       4-SUM (pairs), and for counting paths in grids (split at the
 *       anti-diagonal, map of (cell, value)).
 * Status: untested
 */
vl subset_sums_sorted(const vl &a) {
    vl s = {0};
    for (ll x : a) {
        vl t(sz(s)), r(2 * sz(s));
        for (int i = 0; i < sz(s); i++) t[i] = s[i] + x;
        merge(all(s), all(t), r.begin());
        s.swap(r);
    }
    return s;
}

ll mitm_count(const vl &a, ll S) {
    int h = sz(a) / 2;
    vl L = subset_sums_sorted(vl(a.begin(), a.begin() + h));
    vl R = subset_sums_sorted(vl(a.begin() + h, a.end()));
    ll cnt = 0;  // two pointers: L ascending, R descending
    for (int i = 0, j = sz(R) - 1; i < sz(L) && j >= 0;) {
        ll s = L[i] + R[j];
        if (s < S) i++;
        else if (s > S) j--;
        else {
            ll ci = 0, cj = 0, x = L[i], y = R[j];
            while (i < sz(L) && L[i] == x) i++, ci++;
            while (j >= 0 && R[j] == y) j--, cj++;
            cnt += ci * cj;
        }
    }
    return cnt;
}

ll mitm_best(const vl &a, ll W) {
    int h = sz(a) / 2;
    vl L = subset_sums_sorted(vl(a.begin(), a.begin() + h));
    vl R = subset_sums_sorted(vl(a.begin() + h, a.end()));
    ll best = LLONG_MIN;
    for (int i = 0, j = sz(R) - 1; i < sz(L); i++) {
        while (j >= 0 && L[i] + R[j] > W) j--;
        if (j < 0) break;
        best = max(best, L[i] + R[j]);
    }
    return best;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_meet_middle() {
    vl s = subset_sums_sorted({3, 1, 2});
    cout << s[0] << s[1] << s[2] << s[3]  // 01233456
         << s[4] << s[5] << s[6] << s[7] << '\n';
    vl a = {2, 4, 1, 3, 5, 7, 9, 11};
    cout << mitm_count(a, 10) << ' ' << mitm_best(a, 41) << '\n';  // 6 41
}
