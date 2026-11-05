/**
 * Usage: Big a("123456789012345678901234567890"), b(42LL);  (non-negative)
 *        a + b, a - b (needs a >= b), a * b, a * small, a / small, a % small
 *        (small = ll up to ~9e9), comparisons, a.str()
 * Complexity: + - O(n), * O(n m), / % by small O(n); n = digits / 9
 * Source: own (gap: "big integers" in none of the references; Python is
 *         usually the faster way to write it)
 * Note: signed numbers: keep a sign flag outside. Big / Big: binary search
 *       on each base-1e9 digit or use Python. Factorials of 1000 (2568
 *       digits) are fine with * small.
 * Status: ok
 */
struct Big {
    static const int B = 1000000000;
    vector<int> d;  // little endian, base 1e9, no leading zero limbs
    Big(ll x = 0) { for (; x; x /= B) d.pb(x % B); }
    Big(const string &s) {
        for (int i = sz(s); i > 0; i -= 9)
            d.pb(stoi(s.substr(max(0, i - 9), i - max(0, i - 9))));
        trim();
    }
    void trim() { while (!d.empty() && !d.back()) d.pop_back(); }
    string str() const {
        if (d.empty()) return "0";
        string s = to_string(d.back());
        for (int i = sz(d) - 2; i >= 0; i--) {
            string t = to_string(d[i]);
            s += string(9 - sz(t), '0') + t;
        }
        return s;
    }
    bool operator<(const Big &o) const {
        if (sz(d) != sz(o.d)) return sz(d) < sz(o.d);
        for (int i = sz(d) - 1; i >= 0; i--)
            if (d[i] != o.d[i]) return d[i] < o.d[i];
        return false;
    }
    bool operator==(const Big &o) const { return d == o.d; }
    bool operator!=(const Big &o) const { return !(*this == o); }
    bool operator>(const Big &o) const { return o < *this; }
    bool operator<=(const Big &o) const { return !(o < *this); }
    bool operator>=(const Big &o) const { return !(*this < o); }
    Big operator+(const Big &o) const {
        Big r;
        for (int i = 0, c = 0; i < max(sz(d), sz(o.d)) || c; i++) {
            ll s = c + (i < sz(d) ? d[i] : 0) + (i < sz(o.d) ? o.d[i] : 0);
            r.d.pb(s % B), c = s >= B;
        }
        return r;
    }
    Big operator-(const Big &o) const {  // *this >= o
        Big r = *this;
        for (int i = 0, c = 0; i < sz(r.d); i++) {
            ll s = (ll)r.d[i] - c - (i < sz(o.d) ? o.d[i] : 0);
            c = s < 0, r.d[i] = s + (c ? B : 0);
        }
        r.trim();
        return r;
    }
    Big operator*(const Big &o) const {
        vector<ull> t(sz(d) + sz(o.d) + 1, 0);
        for (int i = 0; i < sz(d); i++) {
            ull c = 0;
            for (int j = 0; j < sz(o.d) || c; j++) {
                ull cur = t[i + j] + c + (j < sz(o.d) ? (ull)d[i] * o.d[j] : 0);
                t[i + j] = cur % B, c = cur / B;
            }
        }
        Big r;
        for (ull x : t) r.d.pb((int)x);
        r.trim();
        return r;
    }
    pair<Big, ll> divmod(ll m) const {  // m >= 1, m <= ~9e9
        Big q;
        q.d.resize(sz(d));
        i128 rem = 0;
        for (int i = sz(d) - 1; i >= 0; i--) {
            rem = rem * B + d[i];
            q.d[i] = (int)(rem / m), rem %= m;
        }
        q.trim();
        return {q, (ll)rem};
    }
    Big operator/(ll m) const { return divmod(m).F; }
    ll operator%(ll m) const { return divmod(m).S; }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_bigint() {
    Big f(1);
    for (int i = 1; i <= 25; i++) f = f * Big(i);  // 25!
    cout << f.str() << '\n';  // 15511210043330985984000000
    Big a("99999999999999999999"), b(1);
    ll r = (a - b) % 7;
    cout << (a + b).str() << ' ' << r << '\n';  // 100000000000000000000 0
    cout << (f / 1000000).str() << '\n';  // 15511210043330985984
}
