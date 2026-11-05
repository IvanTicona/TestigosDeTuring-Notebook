/**
 * Usage: TwoSat ts(n);  literal of "x == v": TwoSat::lit(x, v); negation:
 *        literal ^ 1.  ts.either(a, b) (a or b), ts.imply(a, b),
 *        ts.force(a) (a must hold), ts.at_most_one({a, b, c, ...})
 *        vb val; ts.solve(val) -> satisfiable?, val[x] = assignment
 * Complexity: O(n + #clauses)
 * Source: UMSS Two Sat (Kosaraju, recursive) + UPB TwoSAT + UTP two sat;
 *         at_most_one with prefix variables: KACTL
 * Note: x xor y: either(x, y), either(~x, ~y). x == y: imply both ways.
 *       Assignment: x true iff comp(x) comes AFTER comp(~x) in the
 *       topological order of the implication graph.
 * Depends: src/graphs/scc.cpp
 * Status: untested
 */
struct TwoSat {
    int n;
    vector<vi> g;
    TwoSat(int n_) : n(n_), g(2 * n_) {}
    static int lit(int x, bool v) { return 2 * x + !v; }
    int add_var() { g.resize(2 * ++n); return 2 * (n - 1); }
    void imply(int a, int b) { g[a].pb(b), g[b ^ 1].pb(a ^ 1); }
    void either(int a, int b) { imply(a ^ 1, b); }
    void force(int a) { imply(a ^ 1, a); }
    void at_most_one(const vi &ls) {
        if (sz(ls) <= 1) return;
        int cur = ls[0] ^ 1;  // cur = "none of the prefix is true"
        for (int i = 2; i < sz(ls); i++) {
            int nx = add_var();
            either(cur, ls[i] ^ 1), either(cur, nx), either(ls[i] ^ 1, nx);
            cur = nx ^ 1;
        }
        either(cur, ls[1] ^ 1);
    }
    bool solve(vb &val) {
        SCC s(g);
        val.assign(n, false);
        for (int x = 0; x < n; x++) {
            if (s.comp[2 * x] == s.comp[2 * x + 1]) return false;
            val[x] = s.comp[2 * x] > s.comp[2 * x + 1];
        }
        return true;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_two_sat() {
    TwoSat ts(3);  // (x0 or x1), (!x0 or x2), (!x1 or !x2), x1
    auto L = TwoSat::lit;
    ts.either(L(0, 1), L(1, 1)), ts.either(L(0, 0), L(2, 1));
    ts.either(L(1, 0), L(2, 0)), ts.force(L(1, 1));
    vb v;
    bool ok = ts.solve(v);
    cout << ok << ' ' << v[0] << v[1] << v[2] << '\n';  // 1 010
    ts.force(L(0, 1));  // now x0 -> x2 -> !x1: contradiction
    cout << ts.solve(v) << '\n';  // 0
}
