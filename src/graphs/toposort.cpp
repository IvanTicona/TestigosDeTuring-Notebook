/**
 * Usage: toposort(g)            -> order (Kahn), {} if there is a cycle
 *        toposort(g, true)      -> lexicographically smallest order
 *        find_cycle(g)          -> vertices of a directed cycle in order,
 *                                  {} if the graph is a DAG
 *        longest_path(g, ord)   -> dp[v] = max #edges of a path ending at v
 * Complexity: O(n + m) (lexicographic: O(m log n))
 * Source: UMSS Topological Sort + UPB TopologicalSort + UPB
 *         TopoSort_DAG_LongestPath + UMSS Cycle Detection (recursive DFS,
 *         globals); iterative cycle search: own
 * Note: DP on a DAG: process in topological order (paths count, longest
 *       weighted path: dp[v] = max(dp[u] + w)). Undirected cycle: DSU or a
 *       DFS that skips the parent edge.
 * Status: untested
 */
vi toposort(const vector<vi> &g, bool lex = false) {
    int n = sz(g);
    vi in(n, 0), ord;
    for (auto &l : g) for (int v : l) in[v]++;
    priority_queue<int, vi, greater<int>> pq;  // lex: smallest first
    queue<int> q;
    for (int v = 0; v < n; v++) if (!in[v]) lex ? pq.push(v) : q.push(v);
    while (lex ? !pq.empty() : !q.empty()) {
        int u;
        if (lex) u = pq.top(), pq.pop();
        else u = q.front(), q.pop();
        ord.pb(u);
        for (int v : g[u])
            if (!--in[v]) lex ? pq.push(v) : q.push(v);
    }
    return sz(ord) == n ? ord : vi();
}

vi find_cycle(const vector<vi> &g) {  // iterative DFS, colors 0/1/2
    int n = sz(g);
    vi col(n, 0), par(n, -1), it(n, 0);
    for (int s = 0; s < n; s++) {
        if (col[s]) continue;
        vi st = {s};
        col[s] = 1;
        while (!st.empty()) {
            int u = st.back();
            if (it[u] == sz(g[u])) { col[u] = 2, st.pop_back(); continue; }
            int v = g[u][it[u]++];
            if (col[v] == 0) col[v] = 1, par[v] = u, st.pb(v);
            else if (col[v] == 1) {  // back edge u -> v closes a cycle
                vi cyc;
                for (int x = u; x != v; x = par[x]) cyc.pb(x);
                cyc.pb(v);
                reverse(all(cyc));
                return cyc;
            }
        }
    }
    return {};
}

vi longest_path(const vector<vi> &g, const vi &ord) {
    vi dp(sz(g), 0);
    for (int u : ord)
        for (int v : g[u]) dp[v] = max(dp[v], dp[u] + 1);
    return dp;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_toposort() {
    vector<vi> g = {{2}, {2, 3}, {4}, {4}, {}};  // 0,1 -> 2 -> 4, 1 -> 3 -> 4
    vi o = toposort(g, true);
    cout << o[0] << o[1] << o[2] << o[3] << o[4] << ' '  // 01234 2
         << longest_path(g, o)[4] << '\n';
    vector<vi> h = {{1}, {2}, {0, 3}, {}};
    vi c = find_cycle(h);
    cout << sz(c) << ' ' << c[0] << c[1] << c[2] << ' '  // 3 012 0
         << sz(toposort(h)) << '\n';
}
