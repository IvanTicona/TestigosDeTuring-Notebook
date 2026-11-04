// Stress test for src/dp/digit_dp.cpp
// Oracles: direct scan of every x in [0, N] (N <= 3e4) for random automata
// (digit sum mod k, digit sum exactly s, no equal adjacent digits, digit
// product parity, "contains 7"); for N up to 1e18, the multiples-of-k count
// floor(N/k) + 1 for k = 3, 9 (digit-sum rule).
#include "tests/stress/template_lib.h"
#include "src/dp/digit_dp.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_digit_dp, "src/dp/digit_dp.cpp", "example_digit_dp");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        ll N = stress::rnd(-2, stress::rnd(0, 1) ? 300 : 30000);
        int type = stress::rnd(0, 4), k = stress::rnd(1, 12), target = stress::rnd(0, 30);
        // automaton + direct predicate on the decimal string of x
        auto step = [&](int s, int d) -> int {
            if (type == 0) return (s + d) % k;
            if (type == 1) return s + d > target ? -1 : s + d;
            if (type == 2) return d == s ? -1 : d;   // s0 = 10
            if (type == 3) return s * (d % 2);       // product parity, s0 = 1
            return s == 1 || d == 7 ? 1 : 0;         // contains 7
        };
        auto acc = [&](int s) {
            if (type == 0) return s == 0;
            if (type == 1) return s == target;
            if (type == 2) return true;
            return s == 1;
        };
        int S = type == 0 ? k : type == 1 ? target + 1 : type == 2 ? 11 : 2;
        int s0 = type == 2 ? 10 : type == 3 ? 1 : 0;
        auto direct = [&](ll x) {
            string t = to_string(x);
            int sum = 0, prod = 1;
            bool adj = true, seven = false;
            for (int i = 0; i < sz(t); i++) {
                int d = t[i] - '0';
                sum += d, prod *= d % 2, seven |= d == 7;
                if (i && t[i] == t[i - 1]) adj = false;
            }
            if (type == 0) return sum % k == 0;
            if (type == 1) return sum == target;
            if (type == 2) return adj;
            if (type == 3) return prod == 1;
            return seven;
        };
        ll want = 0;
        for (ll x = 0; x <= N; x++) want += direct(x);
        CHECK(digit_dp(N, S, s0, step, acc) == want, "type=" + to_string(type) + " N=" + to_string(N));
    }
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        ll N = stress::rnd(0, (ll)1e18);
        for (int k : {3, 9}) {
            auto st = [&](int s, int d) { return (s + d) % k; };
            auto ac = [](int s) { return s == 0; };
            CHECK(digit_dp(N, k, 0, st, ac) == N / k + 1, "multiples of " + to_string(k) + " N=" + to_string(N));
        }
    }
    puts("OK");
}
