// Stress test for src/strings/lyndon.cpp
// Oracles: each factor is a Lyndon word (strictly smaller than all its
// proper rotations), factors are non-increasing and cover s in order
// (this characterizes the unique Lyndon factorization); min_rotation by
// comparing all rotations and taking the smallest index among the minima.
#include "tests/stress/template_lib.h"
#include "src/strings/lyndon.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_lyndon, "src/strings/lyndon.cpp", "example_lyndon");
    for (stress::iter = 0; stress::iter < stress::scaled(4000); stress::iter++) {
        string s = stress::rnd_str(stress::rnd(0, 30), 'a', stress::rnd(0, 1) ? 'b' : 'd');
        if (stress::rnd(0, 4) == 0 && !s.empty()) s = s + s + s;  // periodic
        int n = sz(s);
        vi f = duval(s);
        CHECK(n == 0 ? f.empty() : (!f.empty() && f[0] == 0), "starts at 0");
        vector<string> parts;
        for (int i = 0; i < sz(f); i++) parts.pb(s.substr(f[i], (i + 1 < sz(f) ? f[i + 1] : n) - f[i]));
        for (int i = 0; i < sz(parts); i++) {
            const string &w = parts[i];
            CHECK(!w.empty(), "non-empty factor");
            for (int r = 1; r < sz(w); r++) CHECK(w < w.substr(r) + w.substr(0, r), "lyndon word " + w);
            if (i) CHECK(parts[i - 1] >= w, "non-increasing");
        }
        if (n) {
            int best = 0;
            for (int k = 1; k < n; k++)
                if (s.substr(k) + s.substr(0, k) < s.substr(best) + s.substr(0, best)) best = k;
            CHECK(min_rotation(s) == best, "min_rotation s=" + s);
        }
    }
    puts("OK");
}
