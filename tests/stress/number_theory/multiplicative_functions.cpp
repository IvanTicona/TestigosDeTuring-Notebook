// Stress test for src/number_theory/multiplicative_functions.cpp
// Oracles (by definition, different methods):
//  - phi(x) = #{1 <= i <= x : gcd(i, x) = 1}            (x <= 3000)
//  - d(x), sigma(x) from a divisor-enumeration sieve     (x <= 3e5)
//  - mu(x): from sum_{d|x} mu(d) = [x == 1] solved by the O(n log n) recurrence
//  - large n (<= 1e12): divisors enumerated up to sqrt(n), then
//    d = #divisors, sigma = sum, phi via sum_{d|n} phi(d) = n identity
#include "tests/stress/template_lib.h"
#include "src/number_theory/multiplicative_functions.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_multiplicative_functions,
                          "src/number_theory/multiplicative_functions.cpp",
                          "example_multiplicative_functions");
    const int N = 300000;
    MultSieve s(N);
    {
        MultSieve s0(0), s1(1);
        CHECK(s1.phi[1] == 1 && s1.mu[1] == 1 && s1.d[1] == 1 && s1.sigma[1] == 1, "n=1");
    }
    // divisor sieve oracle for d and sigma
    vl dd(N + 1, 0), ss(N + 1, 0), mu(N + 1, 0);
    for (int i = 1; i <= N; i++) for (int j = i; j <= N; j += i) dd[j]++, ss[j] += i;
    mu[1] = 1;  // sum_{d|n} mu(d) = [n == 1]
    for (int i = 1; i <= N; i++) for (int j = 2 * i; j <= N; j += i) mu[j] -= mu[i];
    for (int x = 1; x <= N; x++) {
        string in = "x=" + to_string(x);
        CHECK(s.d[x] == dd[x] && s.sigma[x] == ss[x], in + " sieve d/sigma");
        CHECK(s.mu[x] == mu[x], in + " sieve mu");
        CHECK(mobius(x) == mu[x], in + " mobius(x)");
        auto [c, sm] = divisor_count_sum(x);
        CHECK(c == dd[x] && sm == ss[x], in + " divisor_count_sum");
        CHECK(phi(x) == s.phi[x], in + " phi single vs sieve");
    }
    for (int x = 1; x <= 3000; x++) {
        int cnt = 0;
        for (int i = 1; i <= x; i++) cnt += std::gcd(i, x) == 1;
        CHECK(s.phi[x] == cnt, "phi by gcd count x=" + to_string(x));
    }
    // large n: enumerate divisors, check d, sigma and sum_{d|n} phi(d) == n
    for (int it = 0; it < 200; it++) {
        stress::iter = it;
        ll n = it % 4 == 0 ? stress::rnd(1, 1000) * stress::rnd(1, 1000) * 720720
                           : stress::rnd(1, 1000000000000LL);
        vl divs;
        for (ll i = 1; i * i <= n; i++)
            if (n % i == 0) { divs.pb(i); if (i != n / i) divs.pb(n / i); }
        ll sum = 0, phisum = 0;
        for (ll dv : divs) sum += dv, phisum += phi(dv);
        auto [c, sm] = divisor_count_sum(n);
        string in = "n=" + to_string(n);
        CHECK(c == sz(divs) && sm == sum, in + " d/sigma");
        CHECK(phisum == n, in + " sum phi(d) == n");
        int musum = 0;
        for (ll dv : divs) musum += mobius(dv);
        CHECK(musum == (n == 1), in + " sum mu(d) == [n==1]");
    }
    puts("OK");
}
