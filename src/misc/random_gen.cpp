/**
 * Usage: generators for brute-force vs solution stress tests (uses rng /
 *        randint from the template; reproducible: in main of the
 *        generator call rng.seed(atoi(argv[1])) before anything else)
 *        rand_perm(n)            -> random permutation of 0..n-1
 *        rand_distinct(k, lo, hi)-> k distinct values in [lo, hi] (sorted)
 *        rand_tree(n)            -> n - 1 edges of a uniform-ish random tree
 *                                   with shuffled labels
 *        rand_graph(n, m)        -> m distinct edges, no self-loops,
 *                                   connected (needs n - 1 <= m <= n(n-1)/2)
 *        rand_str(n, k)          -> string over the first k letters
 * Complexity: O(n + m) expected (rand_distinct O(k log k))
 * Source: UMSS Randoms + UPB RandomSTL; generators: own
 * Note: stress loop (bash): for i in $(seq 1 1000); do ./gen $i > in;
 *       ./sol < in > a; ./brute < in > b; cmp -s a b || break; done
 *       (see the contest sheet). Small inputs find bugs faster.
 * Status: untested
 */
vi rand_perm(int n) {
    vi p(n);
    iota(all(p), 0);
    shuffle(all(p), rng);
    return p;
}

vl rand_distinct(int k, ll lo, ll hi) {  // needs k <= hi - lo + 1
    set<ll> s;
    while (sz(s) < k) s.insert(randint(lo, hi));
    return vl(all(s));
}

vector<pii> rand_tree(int n) {
    vi lab = rand_perm(n);
    vector<pii> e;
    for (int i = 1; i < n; i++) e.pb({lab[i], lab[randint(0, i - 1)]});
    shuffle(all(e), rng);
    return e;
}

vector<pii> rand_graph(int n, int m) {
    vector<pii> e = rand_tree(n);
    set<pii> s;
    for (auto [u, v] : e) s.insert({min(u, v), max(u, v)});
    while (sz(e) < m) {
        int u = randint(0, n - 1), v = randint(0, n - 1);
        if (u == v || !s.insert({min(u, v), max(u, v)}).second) continue;
        e.pb({u, v});
    }
    shuffle(all(e), rng);
    return e;
}

string rand_str(int n, int k) {
    string s(n, 'a');
    for (char &c : s) c = 'a' + randint(0, k - 1);
    return s;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_random_gen() {
    auto t = rand_tree(5);
    auto g = rand_graph(6, 9);
    vl d = rand_distinct(3, 1, 3);
    int len = sz(rand_str(4, 2));  // values are random, sizes are not
    cout << sz(t) << ' ' << sz(g) << ' '  // 4 9 123 4
         << d[0] << d[1] << d[2] << ' ' << len << '\n';
}
