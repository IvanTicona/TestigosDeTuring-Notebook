/**
 * Usage: Eertree e(s);  lowercase letters
 *        e.distinct()   -> #distinct palindromic substrings
 *        e.len[v], e.occ[v], e.pos[v] -> length, #occurrences and end of
 *                         the first occurrence of palindrome v (nodes 2..)
 *        e.ends[i]      -> #palindromes ending at position i (their sum =
 *                         #palindromic substrings by position)
 * Complexity: O(n K) build; memory ~(n + 2) K ints (n = 1e6: ~110 MB)
 * Source: UPB PalindromicTree_Eertree (copies s char by char; occ needs a
 *         separate build_occ); ends[] and pos[]: own
 * Note: node 0 = root of length -1, node 1 = root of length 0. Longest
 *       palindromic suffix after each character = last. Palindromic
 *       factorization / series links: not here.
 * Status: untested
 */
struct Eertree {
    static const int K = 26;
    vector<array<int, K>> nx;
    vi len, link, ends, pos;
    vl occ;
    int distinct() const { return sz(len) - 2; }
    int node(int l) {
        nx.pb({}), nx.back().fill(0), len.pb(l), link.pb(0), occ.pb(0);
        pos.pb(-1);
        return sz(len) - 1;
    }
    Eertree(const string &s) {
        node(-1), node(0);  // link[1] = 0, link[0] = 0
        int last = 1;
        vi depth = {0, 0};  // #palindromic suffixes of each node (incl.)
        for (int i = 0; i < sz(s); i++) {
            int c = s[i] - 'a', v = last;
            auto fits = [&](int u) {  // s[i - 1 - len[u]] == s[i]
                return i - 1 - len[u] >= 0 && s[i - 1 - len[u]] == s[i];
            };
            while (!fits(v) && v != 0) v = link[v];
            if (!nx[v][c]) {
                int u = node(len[v] + 2);
                pos[u] = i;
                if (len[u] == 1) link[u] = 1;
                else {
                    int w = link[v];
                    while (!fits(w)) w = link[w];
                    link[u] = nx[w][c];
                }
                nx[v][c] = u;  // only after computing the link
                depth.pb(depth[link[u]] + 1);
            }
            last = nx[v][c];
            occ[last]++, ends.pb(depth[last]);
        }
        for (int v = sz(len) - 1; v >= 2; v--) occ[link[v]] += occ[v];
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_eertree() {
    Eertree e("abaaba");  // a b aba aa baab abaaba
    ll tot = accumulate(all(e.ends), 0LL);
    cout << e.distinct() << ' ' << tot << '\n';  // 6 11
}
