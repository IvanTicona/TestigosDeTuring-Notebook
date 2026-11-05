/**
 * Usage: window_min(a, k)  -> min of each window a[i..i+k-1] (pass
 *                             greater<ll>() as 3rd arg for max)
 *        MonoQueue q; q.push(x); q.pop(); q.best()  (min of the queue)
 *        Swag<T, F> s(op, e); s.push(x); s.pop(); s.fold() -> op over the
 *          queue in order, for ANY associative op (gcd, matrix, affine)
 * Complexity: O(1) amortized per operation
 * Source: UPB MonotonicQueue (window min / max) + UTP sliding window;
 *         two-stack queue (SWAG): own (classic)
 * Note: DP dp[i] = c_i + min_{i-K<=j<i} dp[j]: push dp[j], pop when j
 *       leaves. Next smaller element / histogram: monotonic STACK, same
 *       pop rule. Swag fold order = push order (front first).
 * Status: ok
 */
template <class C = less<ll>>
vl window_min(const vl &a, int k, C cmp = C()) {
    deque<int> dq;  // indices, values increasing by cmp from the front
    vl res;
    for (int i = 0; i < sz(a); i++) {
        while (!dq.empty() && !cmp(a[dq.back()], a[i])) dq.pop_back();
        dq.pb(i);
        if (dq.front() <= i - k) dq.pop_front();
        if (i >= k - 1) res.pb(a[dq.front()]);
    }
    return res;
}

struct MonoQueue {  // queue of values with O(1) min
    deque<ll> all_, mn;
    void push(ll x) {
        all_.pb(x);
        while (!mn.empty() && mn.back() > x) mn.pop_back();
        mn.pb(x);
    }
    void pop() {
        if (mn.front() == all_.front()) mn.pop_front();
        all_.pop_front();
    }
    ll best() const { return mn.front(); }
    int size() const { return sz(all_); }
};

template <class T, class F>
struct Swag {  // front stack holds suffix folds, back stack a running fold
    F op;
    T e;
    vector<T> fv, fa, bv;  // front values / front folds / back values
    T ba;
    Swag(F op_, T e_) : op(op_), e(e_), ba(e_) {}
    void push(T x) { bv.pb(x), ba = op(ba, x); }
    void pop() {
        if (fv.empty()) {  // move back stack to the front, reversed
            while (!bv.empty()) {
                fa.pb(fa.empty() ? bv.back() : op(bv.back(), fa.back()));
                fv.pb(bv.back()), bv.pop_back();
            }
            ba = e;
        }
        fv.pop_back(), fa.pop_back();
    }
    T fold() const { return fa.empty() ? ba : op(fa.back(), ba); }
    int size() const { return sz(fv) + sz(bv); }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_monotonic_queue() {
    vl a = {4, 2, 5, 1, 3, 6};
    vl mn = window_min(a, 3), mx = window_min(a, 3, greater<ll>());
    cout << mn[0] << mn[1] << mn[2] << mn[3] << ' '  // 2111 5556
         << mx[0] << mx[1] << mx[2] << mx[3] << '\n';
    Swag s([](ll x, ll y) { return __gcd(x, y); }, 0LL);
    for (ll x : {12, 18, 8}) s.push(x);
    ll g1 = s.fold();
    s.pop();  // drop 12 -> gcd(18, 8)
    cout << g1 << ' ' << s.fold() << '\n';  // 2 2
}
