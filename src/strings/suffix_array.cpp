/**
 * Usage: vi sa = suffix_array(s);  sa[i] = start of the i-th smallest
 *        suffix (any bytes, including '\0')
 *        vi lcp = lcp_array(s, sa);  lcp[i] = LCP(sa[i], sa[i + 1])
 *        distinct substrings = n (n + 1) / 2 - sum(lcp)
 * Complexity: O(n log n) (cyclic doubling + counting sort), Kasai O(n)
 * Source: UPB SuffixArrayLCP (sorted CYCLIC shifts without a sentinel:
 *         wrong for suffixes) + UPB isc string_SA (n log^2 n, ok) + UPB
 *         isc string_kasai + UPB isc string_ukkonen (not included)
 * Note: LCP of any two suffixes: rank = inverse of sa, min of lcp over
 *       [rank_i, rank_j) (sparse table). Pattern search: binary search
 *       on sa, O(m log n). Longest repeated substring: max lcp. LCS of
 *       a and b: s = a + sep + b, max lcp over adjacent pairs from
 *       different sides.
 * Status: ok
 */
vi suffix_array(const string &s) {
    int n = sz(s) + 1, K = max(257, n);
    vi a(n), p(n), c(n), pn(n), cn(n), cnt(K, 0);
    for (int i = 0; i + 1 < n; i++) a[i] = (unsigned char)s[i] + 1;
    a[n - 1] = 0;  // sentinel: smallest, unique
    for (int x : a) cnt[x]++;
    for (int i = 1; i < K; i++) cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; i--) p[--cnt[a[i]]] = i;
    c[p[0]] = 0;
    for (int i = 1; i < n; i++)
        c[p[i]] = c[p[i - 1]] + (a[p[i]] != a[p[i - 1]]);
    for (int k = 1; k < n; k *= 2) {  // sort by (c[i], c[i + k]) cyclically
        for (int i = 0; i < n; i++) pn[i] = (p[i] - k + n) % n;
        fill(all(cnt), 0);
        for (int x : c) cnt[x]++;
        for (int i = 1; i < K; i++) cnt[i] += cnt[i - 1];
        for (int i = n - 1; i >= 0; i--) p[--cnt[c[pn[i]]]] = pn[i];
        cn[p[0]] = 0;
        for (int i = 1; i < n; i++) {
            int x = p[i], y = p[i - 1];
            pii cur = {c[x], c[(x + k) % n]}, prv = {c[y], c[(y + k) % n]};
            cn[x] = cn[y] + (cur != prv);
        }
        c.swap(cn);
        if (c[p[n - 1]] == n - 1) break;  // all ranks distinct
    }
    return vi(p.begin() + 1, p.end());  // drop the sentinel suffix
}

vi lcp_array(const string &s, const vi &sa) {
    int n = sz(s);
    vi rk(n), lcp(max(0, n - 1), 0);
    for (int i = 0; i < n; i++) rk[sa[i]] = i;
    for (int i = 0, h = 0; i < n; i++) {
        if (rk[i] == n - 1) { h = 0; continue; }
        int j = sa[rk[i] + 1];
        while (i + h < n && j + h < n && s[i + h] == s[j + h]) h++;
        lcp[rk[i]] = h;
        if (h) h--;
    }
    return lcp;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_suffix_array() {
    string s = "banana";  // a, ana, anana, banana, na, nana
    vi sa = suffix_array(s), lcp = lcp_array(s, sa);
    cout << sa[0] << sa[1] << sa[2] << sa[3] << sa[4] << sa[5]  // 531042
         << '\n';
    cout << lcp[0] << lcp[1] << lcp[2] << lcp[3] << lcp[4] << '\n';  // 13002
    ll distinct = 6 * 7 / 2 - accumulate(all(lcp), 0LL);
    cout << distinct << '\n';  // 15
}
