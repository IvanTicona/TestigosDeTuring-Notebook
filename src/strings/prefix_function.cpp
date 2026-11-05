/**
 * Usage: pi = prefix_function(s): pi[i] = longest proper border of s[0..i]
 *        kmp(text, pat)  -> all start positions of pat in text (an empty
 *                           pattern matches at 0..|text|)
 *        period(s)       -> smallest p with s[i] == s[i+p] (|s| - pi.back())
 *        auto aut = kmp_automaton(s, 'a', 26): aut[i][c] = next state from
 *                   state i (matched i chars) reading char c; state |s| =
 *                   full match (it keeps going as a border)
 * Complexity: O(n) (automaton O(n * alphabet))
 * Source: UMSS KMP + UMSS KMP Automaton (global arrays) + UPB KMP + UPB
 *         KMP_Automaton + UPB isc string_KMP + old kmp (empty pattern bug)
 * Note: #occurrences of each prefix of s in s: cnt[pi[i]]++, then
 *       cnt[pi[i-1]] += cnt[i] going down, then +1 each. String period
 *       divides |s| iff s is a power of a shorter string. Automaton + DP:
 *       count strings of length L avoiding a pattern.
 * Status: untested
 */
vi prefix_function(const string &s) {
    int n = sz(s);
    vi pi(n, 0);
    for (int i = 1; i < n; i++) {
        int k = pi[i - 1];
        while (k && s[i] != s[k]) k = pi[k - 1];
        pi[i] = k + (s[i] == s[k]);
    }
    return pi;
}

vi kmp(const string &text, const string &pat) {
    vi res;
    if (pat.empty()) {
        for (int i = 0; i <= sz(text); i++) res.pb(i);
        return res;
    }
    vi pi = prefix_function(pat);
    for (int i = 0, k = 0; i < sz(text); i++) {
        while (k && text[i] != pat[k]) k = pi[k - 1];
        if (text[i] == pat[k]) k++;
        if (k == sz(pat)) res.pb(i - k + 1), k = pi[k - 1];
    }
    return res;
}

int period(const string &s) {
    return s.empty() ? 0 : sz(s) - prefix_function(s).back();
}

vector<vi> kmp_automaton(const string &s, char lo = 'a', int K = 26) {
    int n = sz(s);
    vi pi = prefix_function(s);
    vector<vi> aut(n + 1, vi(K, 0));
    for (int i = 0; i <= n; i++)
        for (int c = 0; c < K; c++) {
            if (i < n && s[i] - lo == c) aut[i][c] = i + 1;
            else aut[i][c] = i ? aut[pi[i - 1]][c] : 0;
        }
    return aut;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_prefix_function() {
    vi pi = prefix_function("abacaba");
    cout << pi[6] << ' ' << period("abcabcab") << '\n';     // 3 3
    vi occ = kmp("abababa", "aba");
    cout << sz(occ) << ' ' << occ[0] << occ[1] << occ[2] << '\n';  // 3 024
    auto aut = kmp_automaton("ab");
    // after "a": 'a' restarts at 1, 'b' completes; after "ab": 'a' -> 1
    cout << aut[1][0] << aut[1][1] << aut[2][0] << '\n';  // 121
}
