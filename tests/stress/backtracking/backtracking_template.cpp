// Stress test for src/backtracking/backtracking_template.cpp
// Oracle: odometer generation. With the default isValid() == true the template
// must produce every sequence in [1, n]^n, in lexicographic order, exactly once.
//
// Note: isValid() has an unused parameter (compiler warning in the snippet);
//   left as is, the snippet is not modified by this change.
#include "template/template.cpp"
#include "src/backtracking/backtracking_template.cpp"
#include "tests/stress/common.h"

static ll t_pow(int n) {
    ll r = 1;
    for (int i = 0; i < n; i++) r *= n;
    return r;
}

static string t_vec(const vi &v) {
    string s = "[";
    for (int x : v) s += to_string(x) + " ";
    return s + "]";
}

int main() {
    for (int n = 0; n <= 6; n++) {
        stress::iter = n;
        solutions.clear();
        current.clear();
        backtrack(0, n);
        string in = "n=" + to_string(n);
        CHECK((ll)solutions.size() == t_pow(n), in + " count " + to_string(solutions.size()));
        CHECK(current.empty(), in + " current must be restored (undo)");

        // Odometer: n=0 -> a single empty sequence; else 1..1, 1..1 2, ..., n..n
        vi seq(n, 1);
        for (size_t k = 0; k < solutions.size(); k++) {
            const vi &got = solutions[k];
            CHECK(sz(got) == n, in + " length at #" + to_string(k));
            CHECK(got == seq, in + " #" + to_string(k) + " got " + t_vec(got) + " want " + t_vec(seq));
            for (int x : got) CHECK(1 <= x && x <= n, in + " range " + t_vec(got));
            // advance the odometer (last position fastest)
            int p = n - 1;
            while (p >= 0 && seq[p] == n) seq[p--] = 1;
            if (p >= 0) seq[p]++;
        }
    }
    puts("OK");
}
