// =====================================================================
//  Testigos De Turing - contest template
//  Compile: g++ -std=gnu++17 -O2 -Wall -Wextra -Wshadow -o a a.cpp
//  Debug:   g++ -std=gnu++17 -g -DLOCAL -D_GLIBCXX_DEBUG
//           -fsanitize=address,undefined -o a a.cpp
//  (_GLIBCXX_DEBUG = bounds-checked STL; -DLOCAL turns deb() on)
// =====================================================================

// ---------- Optimization (optional, put BEFORE includes) -------------
// #pragma GCC optimize("O3,unroll-loops")
// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")  // may RE on old CPUs

// ---------- Includes --------------------------------------------------
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace std;
using namespace __gnu_pbds;

// ---------- Types -----------------------------------------------------
typedef long long ll;
typedef unsigned long long ull;
typedef long double ld;
typedef __int128 i128;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
typedef tuple<ll, ll, ll> tlll;
typedef vector<int> vi;
typedef vector<ll> vl;
typedef vector<bool> vb;
typedef vector<string> vs;
typedef vector<ld> vld;
typedef vector<vi> vvi;
typedef vector<vl> vvl;
typedef vector<pii> vpii;
typedef vector<pll> vpll;

// ---------- Macros ----------------------------------------------------
#define pb push_back
#define F first
#define S second
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define sz(x) (int)(x).size()
#define sortt(x) sort(all(x))
#define SQ(a) ((a) * (a))
#define FOR(i, a, b) for (int i = (a); i < (b); i++)    // [a, b)
#define ROF(i, a, b) for (int i = (a); i >= (b); i--)   // [b, a] down
#define REP(i, a, b) for (int i = (a); i <= (b); i++)   // [a, b]
#define EACH(x, v) for (auto &x : (v))

// ---------- Constants -------------------------------------------------
const int MOD = 1e9 + 7;
const int MOD2 = 998244353;     // NTT-friendly prime
const int INF = 1e9;            // INF + INF still fits in int
const ll INFL = 1e18;           // INFL + INFL still fits in ll

// ---------- Data structures -------------------------------------------
template <class T>
using pql = priority_queue<T, vector<T>, greater<T>>;  // min-heap
template <class T>
using pqg = priority_queue<T>;                          // max-heap
// ordered set: find_by_order(k) -> k-th (0-based), order_of_key(x) -> #< x
// multiset: use pair<T, int> (value, unique id)
template <class T>
using oset = tree<T, null_type, less<T>, rb_tree_tag,
                  tree_order_statistics_node_update>;
// anti-hack hash: unordered_map<ll, int, custom_hash>
struct custom_hash {
    static ull splitmix64(ull x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }
    size_t operator()(ull x) const {
        static const ull R =
            chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + R);
    }
};

// ---------- Graphs / grids --------------------------------------------
const int dx[4] = {1, 0, -1, 0}, dy[4] = {0, 1, 0, -1};
const int dx8[8] = {1, 1, 0, -1, -1, -1, 0, 1};
const int dy8[8] = {0, 1, 1, 1, 0, -1, -1, -1};

// ---------- Geometry --------------------------------------------------
const ld EPS = 1e-9;
const ld PI = acosl(-1);
int sgn(ld x) { return (x > EPS) - (x < -EPS); }  // -1, 0, 1

// ---------- Math ------------------------------------------------------
// __int128 I/O (cin/cout do not support it)
i128 read128() {
    string s; cin >> s;
    i128 x = 0; int i = (s[0] == '-');
    for (; i < sz(s); i++) x = x * 10 + (s[i] - '0');
    return s[0] == '-' ? -x : x;
}
void print128(i128 x) {
    if (x == 0) { cout << '0'; return; }
    string s; bool neg = x < 0;
    while (x != 0) {  // works for INT128_MIN too (no negation)
        int d = (int)(x % 10);
        s += char('0' + (d < 0 ? -d : d)); x /= 10;
    }
    if (neg) s += '-';
    reverse(all(s)); cout << s;
}

// ---------- Random ----------------------------------------------------
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
ll randint(ll a, ll b) { return uniform_int_distribution<ll>(a, b)(rng); }

// ---------- Output helpers --------------------------------------------
// fixed decimals: cout << fixed << setprecision(9) << x;
void yes() { cout << "YES\n"; }
void no() { cout << "NO\n"; }

// ---------- Debug (only with -DLOCAL, prints to stderr) ---------------
#ifdef LOCAL
void dbg_out(const string &s) { cerr << '"' << s << '"'; }
void dbg_out(const char *s) { cerr << '"' << s << '"'; }
void dbg_out(char c) { cerr << '\'' << c << '\''; }
void dbg_out(bool b) { cerr << (b ? "true" : "false"); }
template <class T> void dbg_out(const T &x);
template <class A, class B> void dbg_out(const pair<A, B> &p) {
    cerr << '('; dbg_out(p.F); cerr << ", "; dbg_out(p.S); cerr << ')';
}
template <class T> void dbg_out(const T &x) {
    if constexpr (is_arithmetic_v<T>) cerr << x;
    else { cerr << '{'; bool f = 1;
        for (auto &e : x) { if (!f) cerr << ", "; f = 0; dbg_out(e); }
        cerr << '}'; }
}
template <class... T> void dbg_all(const char *names, T &&...v) {
    cerr << "[" << names << "] = [";
    int k = 0; ((cerr << (k++ ? ", " : ""), dbg_out(v)), ...);
    cerr << "]\n";
}
#define deb(...) dbg_all(#__VA_ARGS__, __VA_ARGS__)
#else
#define deb(...)
#endif

// ---------- Main ------------------------------------------------------
void init() {}  // precomputation, runs once

void solve() {
    // one test case
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    init();
    int t = 1;
    cin >> t;  // single test case: comment this line
    while (t--) solve();
    return 0;
}
