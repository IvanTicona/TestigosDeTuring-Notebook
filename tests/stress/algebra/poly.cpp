// Stress test for src/algebra/poly.cpp
// Oracles (all naive O(n^2), mod 998244353): series inverse by the
// triangular recurrence, schoolbook long division, ln checked through
// a * (ln a)' == a', exp by n b_n = sum k a_k b_{n-k}, power by binary
// exponentiation with truncated schoolbook products, Horner evaluation.
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/algebra/ntt.cpp"
#include "src/algebra/poly.cpp"
#include "tests/stress/common.h"

const ll P = MOD2;
vl rvec(int n) {
    vl v(n);
    for (ll &x : v) x = stress::rnd(0, P - 1);
    return v;
}
vl mul_trunc(const vl &a, const vl &b, int n) {
    vl c(n, 0);
    for (int i = 0; i < min(n, sz(a)); i++)
        for (int j = 0; j < sz(b) && i + j < n; j++) c[i + j] = (c[i + j] + a[i] * b[j]) % P;
    return c;
}
ll ev(const vl &a, ll x) {
    ll r = 0;
    for (int i = sz(a) - 1; i >= 0; i--) r = (r * x + a[i]) % P;
    return r;
}

int main() {
    stress::check_example(example_poly, "src/algebra/poly.cpp", "example_poly");
    for (stress::iter = 0; stress::iter < stress::scaled(150); stress::iter++) {
        int n = stress::rnd(1, 120), la = stress::rnd(1, 130);
        vl a = rvec(la);
        if (a[0] == 0) a[0] = 1;
        // inverse
        vl b = poly_inv(a, n), w(n, 0);
        ll i0 = invmod_prime(a[0], P);
        for (int k = 0; k < n; k++) {
            ll s = k == 0 ? 1 : 0;
            for (int j = 1; j <= k && j < la; j++) s = (s - a[j] * w[k - j]) % P;
            w[k] = (s + P) % P * i0 % P;
        }
        CHECK(b == w, "inv n=" + to_string(n));
        // log: a[0] = 1, check a * L' == a' mod x^(n-1)
        vl A = a;
        A[0] = 1;
        vl L = poly_log(A, n);
        CHECK(sz(L) == n && L[0] == 0, "log size/const");
        vl dL(max(0, n - 1)), dA(max(0, n - 1), 0);
        for (int i = 1; i < n; i++) dL[i - 1] = L[i] * i % P;
        for (int i = 1; i < min(n, la); i++) dA[i - 1] = A[i] * i % P;
        CHECK(mul_trunc(A, dL, n - 1) == dA, "log derivative n=" + to_string(n));
        // exp
        vl E = rvec(stress::rnd(1, 130));
        E[0] = 0;
        vl ex = poly_exp(E, n), nb(n, 0), inv = inv_table(n, P);
        nb[0] = 1;
        for (int k = 1; k < n; k++) {
            ll s = 0;
            for (int j = 1; j <= k && j < sz(E); j++) s = (s + j * E[j] % P * nb[k - j]) % P;
            nb[k] = s * inv[k] % P;
        }
        CHECK(ex == nb, "exp n=" + to_string(n));
        CHECK(poly_log(poly_exp(E, n), n) == mul_trunc(E, {1}, n), "log(exp)");
        // pow with leading zeros and huge k
        vl pa = rvec(stress::rnd(1, 40));
        int z = stress::rnd(0, 4);
        for (int i = 0; i < min(z, sz(pa)); i++) pa[i] = 0;
        if (stress::rnd(0, 5) == 0) pa.assign(sz(pa), 0);
        ll k = stress::rnd(0, 2) ? stress::rnd(0, 12) : stress::rnd(0, (ll)1e18);
        vl want(n, 0), base = mul_trunc(pa, {1}, n);
        if (n) want[0] = 1;
        for (ll e = k; e; e >>= 1) {
            if (e & 1) want = mul_trunc(want, base, n);
            base = mul_trunc(base, base, n);
        }
        CHECK(poly_pow(pa, k, n) == want, "pow k=" + to_string(k) + " n=" + to_string(n));
        // divmod
        vl pb = rvec(stress::rnd(1, 60));
        if (pb.back() == 0) pb.back() = 1;
        vl pd = rvec(stress::rnd(1, 150));
        auto [q, r] = poly_divmod(pd, pb);
        vl rem = pd, quo(max(0, sz(pd) - sz(pb) + 1), 0);
        ll ib = invmod_prime(pb.back(), P);
        for (int i = sz(pd) - 1; i >= sz(pb) - 1; i--) {
            ll c = rem[i] * ib % P;
            quo[i - sz(pb) + 1] = c;
            for (int j = 0; j < sz(pb); j++) rem[i - sz(pb) + 1 + j] = (rem[i - sz(pb) + 1 + j] - c * pb[j] % P + P) % P;
        }
        if (sz(pd) >= sz(pb)) rem.resize(sz(pb) - 1);
        CHECK(q == quo && r == rem, "divmod |a|=" + to_string(sz(pd)) + " |b|=" + to_string(sz(pb)));
        // multipoint evaluation
        vl xs = rvec(stress::rnd(1, 70));
        if (stress::rnd(0, 1)) xs[0] = 0;
        vl ys = poly_eval(pd, xs);
        for (int i = 0; i < sz(xs); i++) CHECK(ys[i] == ev(pd, xs[i]), "eval x=" + to_string(xs[i]));
    }
    // big: inverse and exp round trip at n = 2^17
    int n = stress::SLOW ? 20000 : 131072;
    vl a = rvec(n);
    a[0] = 0;
    vl e = poly_exp(a, n), l = poly_log(e, n);
    CHECK(l == a, "big log(exp(a)) == a");
    vl one = ntt_mul(e, poly_inv(e, n));
    one.resize(n);
    vl want(n, 0);
    want[0] = 1;
    CHECK(one == want, "big e * inv(e) == 1");
    vl xs = rvec(stress::SLOW ? 2000 : 20000), cf = rvec(stress::SLOW ? 2000 : 20000);
    vl ys = poly_eval(cf, xs);
    for (int t = 0; t < 30; t++) {
        int i = stress::rnd(0, sz(xs) - 1);
        CHECK(ys[i] == ev(cf, xs[i]), "big eval");
    }
    puts("OK");
}
