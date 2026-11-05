/**
 * Usage: SAM sam(s);  lowercase letters (change K / base for others, or
 *        use map<char, int> transitions for big alphabets)
 *        sam.distinct() -> #distinct non-empty substrings
 *        sam.occ(p)     -> #occurrences of p in s (0 if absent)
 *        sam.lcs(t)     -> longest common substring length of s and t
 * Complexity: O(n K) build (<= 2n states), queries O(|p|) / O(|t|);
 *             memory 2n (K + 2) ints: n = 1e6 -> ~230 MB (use map
 *             transitions or a suffix array beyond ~5e5)
 * Source: UPB SuffixAutomaton (reserve(1 << 20) per object: 120 MB) + UPB
 *         isc string_suffix_automaton + UTP suffix automaton
 * Note: endpos classes: len[link[v]] < |substring| <= len[v] for every
 *       substring ending in state v. k-th lexicographic substring: count
 *       paths from each state (DP on the DAG in decreasing len). First
 *       occurrence: store firstpos when creating / cloning.
 * Status: untested
 */
struct SAM {
    static const int K = 26;
    static const char base = 'a';
    vector<array<int, K>> nx;
    vi link, len;
    vl cnt;  // occurrences (endpos size) after build
    int last = 0;
    int add_state(int l) {
        nx.pb({}), nx.back().fill(-1), link.pb(-1), len.pb(l), cnt.pb(0);
        return sz(len) - 1;
    }
    void extend(int c) {
        int cur = add_state(len[last] + 1), p = last;
        cnt[cur] = 1;
        for (; p != -1 && nx[p][c] == -1; p = link[p]) nx[p][c] = cur;
        if (p == -1) link[cur] = 0;
        else {
            int q = nx[p][c];
            if (len[p] + 1 == len[q]) link[cur] = q;
            else {
                int cl = add_state(len[p] + 1);
                nx[cl] = nx[q], link[cl] = link[q];
                for (; p != -1 && nx[p][c] == q; p = link[p]) nx[p][c] = cl;
                link[q] = link[cur] = cl;
            }
        }
        last = cur;
    }
    SAM(const string &s) {
        add_state(0);
        for (char ch : s) extend(ch - base);
        vi ord(sz(len));
        iota(all(ord), 0);
        sort(all(ord), [&](int a, int b) { return len[a] > len[b]; });
        for (int v : ord) if (link[v] >= 0) cnt[link[v]] += cnt[v];
    }
    ll distinct() const {
        ll r = 0;
        for (int v = 1; v < sz(len); v++) r += len[v] - len[link[v]];
        return r;
    }
    ll occ(const string &p) const {
        int v = 0;
        for (char ch : p) if ((v = nx[v][ch - base]) == -1) return 0;
        return p.empty() ? 0 : cnt[v];
    }
    int lcs(const string &t) const {
        int v = 0, l = 0, best = 0;
        for (char ch : t) {
            int c = ch - base;
            while (v && nx[v][c] == -1) v = link[v], l = len[v];
            if (nx[v][c] != -1) v = nx[v][c], l++;
            best = max(best, l);
        }
        return best;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_suffix_automaton() {
    SAM sam("abcbc");
    cout << sam.distinct() << ' ' << sam.occ("bc") << ' '  // 12 2 0
         << sam.occ("ca") << '\n';
    cout << SAM("xabcy").lcs("zzbcyq") << '\n';  // 3
}
