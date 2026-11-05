// Stress test for src/strings/hashing.cpp
// Oracles: substring equality by std::string compare over all pairs of
// equal-length substrings (tiny alphabets -> many equal pairs), bytes
// >= 128 and '\0' included; lcp by direct comparison, across two strings.
#include "tests/stress/template_lib.h"
#include "src/strings/hashing.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_hashing, "src/strings/hashing.cpp", "example_hashing");
    for (stress::iter = 0; stress::iter < stress::scaled(500); stress::iter++) {
        int n = stress::rnd(0, 30), m = stress::rnd(0, 30);
        auto gen = [&](int len) {
            string s(len, 'a');
            int mode = stress::rnd(0, 2);
            for (char &c : s)
                c = mode == 0 ? 'a' + stress::rnd(0, 1) : mode == 1 ? (char)stress::rnd(0, 255) : (char)(stress::rnd(0, 1) ? 0 : 200);
            return s;
        };
        string a = gen(n), b = gen(m);
        Hash ha(a), hb(b);
        for (int i = 0; i < n; i++)
            for (int len = 1; i + len <= n; len++)
                for (int j = 0; j + len <= m; j++)
                    CHECK((ha.get(i, i + len - 1) == hb.get(j, j + len - 1)) == (a.compare(i, len, b, j, len) == 0), "equality");
        for (int t = 0; t < 50 && n && m; t++) {
            int i = stress::rnd(0, n), j = stress::rnd(0, m), k = 0;
            while (i + k < n && j + k < m && a[i + k] == b[j + k]) k++;
            CHECK(lcp(ha, i, hb, j) == k, "lcp");
        }
        CHECK(ha.get(3, 2) == 0, "empty range");
    }
    puts("OK");
}
