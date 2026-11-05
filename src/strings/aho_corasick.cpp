/**
 * Usage: Aho ac; int id = ac.add(p) (id = 0, 1, ...); ac.build();
 *        (lowercase; build() once after all add(), before count / go)
 *        ac.count(text) -> occurrences of each pattern in text (overlaps
 *        and duplicate patterns counted)
 *        ac.go[v][c] = full automaton transition (for DP over strings)
 * Complexity: build O(L K), L = total pattern length; count O(|text| + L);
 *             memory ~(L + 1) K ints (L = 1e6: ~110 MB)
 * Source: UPB AhoCorasick (pointer nodes with unordered_map; missed
 *         matches of patterns that are suffixes of other matches) + UPB
 *         isc string_AC (ok) + UTP incremental Aho-Corasick
 * Note: a node "matches" every pattern on its suffix-link chain: either
 *       push counts up the links (done here, in reverse BFS order) or keep
 *       an output link to the next terminal node. DP "strings of length L
 *       containing no pattern": forbid nodes whose chain has a terminal.
 * Status: untested
 */
struct Aho {
    static const int K = 26;
    static const char base = 'a';
    vector<array<int, K>> go;
    vi link, bfs;
    vector<vi> ends;  // pattern ids ending exactly at a node
    int npat = 0;
    Aho() { node(); }
    int node() {
        go.pb({}), go.back().fill(-1), link.pb(0), ends.pb({});
        return sz(go) - 1;
    }
    int add(const string &p) {
        int v = 0;
        for (char ch : p) {
            int c = ch - base;
            if (go[v][c] == -1) { int u = node(); go[v][c] = u; }
            v = go[v][c];
        }
        ends[v].pb(npat);
        return npat++;
    }
    void build() {
        bfs = {0};
        for (int i = 0; i < sz(bfs); i++) {
            int v = bfs[i];
            for (int c = 0; c < K; c++) {
                int u = go[v][c];
                if (u == -1) go[v][c] = v ? go[link[v]][c] : 0;
                else link[u] = v ? go[link[v]][c] : 0, bfs.pb(u);
            }
        }
    }
    vl count(const string &text) const {
        vl hits(sz(go), 0), res(npat, 0);
        for (int v = 0, i = 0; i < sz(text); i++)
            v = go[v][text[i] - base], hits[v]++;
        for (int i = sz(bfs) - 1; i > 0; i--)  // push up the suffix links
            hits[link[bfs[i]]] += hits[bfs[i]];
        for (int v = 0; v < sz(go); v++)
            for (int id : ends[v]) res[id] += hits[v];
        return res;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_aho_corasick() {
    Aho ac;
    for (string p : {"he", "she", "his", "hers", "e"}) ac.add(p);
    ac.build();
    vl c = ac.count("ahishershe");
    cout << c[0] << c[1] << c[2] << c[3] << c[4] << '\n';  // 22112
}
