/**
 * Usage: PushRelabel p(n); p.add_edge(u, v, cap, rcap = 0);
 *        ll f = p.calc(s, t);  p.left_of_cut(v) -> v on the source side
 * Complexity: O(V^2 sqrt E) (highest label + gap heuristic); usually the
 *             fastest max flow for dense / big graphs
 * Source: UPB PushRelabel + KACTL (highest-label version with gaps)
 * Note: calc() once per object (it leaves preflow state). Only the flow
 *       VALUE and the cut are reliable (edge flows may still route excess
 *       back to s); use Dinic when the flow decomposition is needed.
 * Status: ok
 */
struct PushRelabel {
    struct Edge { int to, back; ll f, c; };
    vector<vector<Edge>> g;
    vl ec;
    vector<Edge *> cur;
    vector<vi> hs;
    vi H;
    PushRelabel(int n) : g(n), ec(n), cur(n), hs(2 * n), H(n) {}
    void add_edge(int s, int t, ll cap, ll rcap = 0) {
        if (s == t) return;
        g[s].pb({t, sz(g[t]), 0, cap});
        g[t].pb({s, sz(g[s]) - 1, 0, rcap});
    }
    void push(Edge &e, ll f) {
        Edge &b = g[e.to][e.back];
        if (!ec[e.to] && f) hs[H[e.to]].pb(e.to);
        e.f += f, e.c -= f, ec[e.to] += f;
        b.f -= f, b.c += f, ec[b.to] -= f;
    }
    ll calc(int s, int t) {
        int v = sz(g);
        H[s] = v, ec[t] = 1;
        vi co(2 * v);
        co[0] = v - 1;
        for (int i = 0; i < v; i++) cur[i] = g[i].data();
        for (Edge &e : g[s]) push(e, e.c);
        for (int hi = 0;;) {
            while (hs[hi].empty())
                if (!hi--) return -ec[s];
            int u = hs[hi].back();
            hs[hi].pop_back();
            while (ec[u] > 0) {  // discharge u
                if (cur[u] == g[u].data() + sz(g[u])) {  // relabel
                    H[u] = 1e9;
                    for (Edge &e : g[u])
                        if (e.c && H[u] > H[e.to] + 1)
                            H[u] = H[e.to] + 1, cur[u] = &e;
                    if (++co[H[u]], !--co[hi] && hi < v)  // gap: lift above v
                        for (int i = 0; i < v; i++)
                            if (hi < H[i] && H[i] < v) --co[H[i]], H[i] = v + 1;
                    hi = H[u];
                } else if (cur[u]->c && H[u] == H[cur[u]->to] + 1) {
                    push(*cur[u], min(ec[u], cur[u]->c));
                } else ++cur[u];
            }
        }
    }
    bool left_of_cut(int a) const { return H[a] >= sz(g); }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_push_relabel() {
    PushRelabel p(4);
    p.add_edge(0, 1, 4), p.add_edge(0, 2, 2), p.add_edge(1, 2, 5);
    p.add_edge(1, 3, 2), p.add_edge(2, 3, 3);
    ll f = p.calc(0, 3);
    cout << f << ' ' << p.left_of_cut(0) << p.left_of_cut(3) << '\n';  // 5 10
}
