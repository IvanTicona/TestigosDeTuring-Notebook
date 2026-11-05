// Stress test for src/strings/z_function.cpp
// Oracles: z[i] by direct comparison; occurrences by comparing at every
// position (empty pattern matches everywhere, pattern longer than text).
#include "tests/stress/template_lib.h"
#include "src/strings/z_function.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_z_function, "src/strings/z_function.cpp", "example_z_function");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        char hi = stress::rnd(0, 1) ? 'b' : 'z';
        string s = stress::rnd_str(stress::rnd(0, 40), 'a', hi), p = stress::rnd_str(stress::rnd(0, 6), 'a', hi);
        vi z = z_function(s);
        for (int i = 0; i < sz(s); i++) {
            int k = 0;
            while (i + k < sz(s) && s[k] == s[i + k]) k++;
            CHECK(z[i] == k, "z i=" + to_string(i));
        }
        vi want;
        for (int i = 0; i + sz(p) <= sz(s); i++) if (s.compare(i, sz(p), p) == 0) want.pb(i);
        CHECK(z_match(s, p) == want, "z_match");
    }
    puts("OK");
}
