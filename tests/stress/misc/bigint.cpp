// Stress test for src/misc/bigint.cpp
// Oracles: unsigned __int128 arithmetic for values below 2^126 (parse,
// print, +, -, *, / and % by small, comparisons); for huge numbers
// (up to ~400 digits) algebraic identities: (a + b) - b == a,
// (a * m) / m == a with remainder 0, (a * b) % m == (a % m)(b % m) % m,
// and decimal strings with leading zeros / carries (999..9 + 1).
#include "tests/stress/template_lib.h"
#include "src/misc/bigint.cpp"
#include "tests/stress/common.h"

string u128str(unsigned __int128 x) {
    if (!x) return "0";
    string s;
    while (x) s += char('0' + (int)(x % 10)), x /= 10;
    return string(rall(s));
}

int main() {
    stress::check_example(example_bigint, "src/misc/bigint.cpp", "example_bigint");
    for (stress::iter = 0; stress::iter < stress::scaled(20000); stress::iter++) {
        auto r63 = [] { return (unsigned __int128)(stress::rng() >> stress::rnd(0, 63)); };
        unsigned __int128 x = r63(), y = r63();
        Big a(u128str(x)), b(u128str(y));
        CHECK(a.str() == u128str(x), "parse / print");
        CHECK((a + b).str() == u128str(x + y), "add");
        if (x >= y) CHECK((a - b).str() == u128str(x - y), "sub");
        CHECK((a * b).str() == u128str(x * y), "mul");
        ll m = stress::rnd(1, stress::rnd(0, 1) ? 10 : 9000000000LL);
        CHECK((a / m).str() == u128str(x / m) && a % m == (ll)(x % m), "divmod small");
        CHECK((a < b) == (x < y) && (a == b) == (x == y), "compare");
        ll v = stress::rnd(0, (ll)1e18);
        CHECK(Big(v).str() == to_string(v), "from ll");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        auto rbig = [] {
            string s(stress::rnd(1, 400), '0');
            for (char &c : s) c = '0' + stress::rnd(0, 9);
            if (stress::rnd(0, 4) == 0) s = string(sz(s), '9');
            return s;
        };
        string sa = rbig(), sb = rbig();
        Big a(sa), b(sb);
        size_t nz = sa.find_first_not_of('0');
        string na = nz == string::npos ? "0" : sa.substr(nz);
        CHECK(a.str() == na, "leading zeros");
        CHECK(((a + b) - b) == a, "add / sub");
        ll m = stress::rnd(1, 9000000000LL);
        Big am = a * Big(m);
        CHECK(am / m == a && am % m == 0, "mul / div");
        CHECK((a * b) % m == (ll)((i128)(a % m) * (b % m) % m), "mod of product");
        CHECK((a < b) != (b < a) || a == b, "total order");
    }
    puts("OK");
}
