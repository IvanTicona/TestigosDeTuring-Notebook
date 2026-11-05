// Stress test for src/misc/backtracking.cpp
// Oracles: all n! column permutations checked for diagonal attacks
// (n <= 9) and the known sequence A000170 up to n = 12; every solution
// from queens_one is verified directly.
#include "tests/stress/template_lib.h"
#include "src/misc/backtracking.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_backtracking, "src/misc/backtracking.cpp", "example_backtracking");
    auto valid = [](const vi &c) {
        for (int i = 0; i < sz(c); i++)
            for (int j = i + 1; j < sz(c); j++)
                if (c[i] == c[j] || abs(c[i] - c[j]) == j - i) return false;
        return true;
    };
    for (int n = 1; n <= 9; n++) {
        vi p(n);
        iota(all(p), 0);
        ll cnt = 0;
        do cnt += valid(p);
        while (next_permutation(all(p)));
        CHECK(queens(n) == cnt, "count n=" + to_string(n));
    }
    ll A[] = {1, 0, 0, 2, 10, 4, 40, 92, 352, 724, 2680, 14200};
    for (int n = 1; n <= 12; n++) {
        CHECK(queens(n) == A[n - 1], "A000170 n=" + to_string(n));
        vi c = queens_one(n);
        CHECK(c.empty() == (A[n - 1] == 0), "existence");
        if (!c.empty()) CHECK(sz(c) == n && valid(c), "valid solution");
    }
    puts("OK");
}
