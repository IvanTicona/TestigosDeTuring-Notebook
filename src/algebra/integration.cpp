/**
 * Usage: simpson(f, a, b, n)      -> composite Simpson, n subintervals
 *        integrate(f, a, b, eps)  -> adaptive Simpson, abs. error ~ eps
 *                                    (relative 1e-14 floor for huge f)
 *        f: any callable double -> double (lambda)
 * Complexity: simpson O(n) calls; integrate adapts to f (smooth: few
 *             hundred calls), depth capped at 50
 * Source: UPB NumericalIntegration (std::function, recomputes every f
 *         value 2-3 times, depth cap 20) + KACTL; reuse of f values: own
 * Note: Simpson is exact for cubics; error O(h^4 f''''). Split the
 *       interval at kinks / discontinuities (e.g. area of union of
 *       circles: integrate the covered length between event x's).
 *       Infinite range: substitute x = t / (1 - t^2), or cut where f ~ 0.
 *       2D: integrate([&](double x) { return integrate([&](double y) {
 *       return g(x, y); }, c(x), d(x)); }, a, b).
 * Status: untested
 */
template <class F>
double simpson(F f, double a, double b, int n = 1000) {
    double h = (b - a) / (2 * n), s = f(a) + f(b);
    for (int i = 1; i < 2 * n; i++) s += f(a + i * h) * (i & 1 ? 4 : 2);
    return s * h / 3;
}

template <class F>
double simpson_rec(F &f, double a, double b, double eps, double fa,
                   double fm, double fb, double S, int d) {
    double m = (a + b) / 2, lm = f((a + m) / 2), rm = f((m + b) / 2);
    double L = (m - a) / 6 * (fa + 4 * lm + fm);
    double R = (b - m) / 6 * (fm + 4 * rm + fb), T = L + R;
    // relative floor: a halved eps below rounding error would recurse forever
    if (d <= 0 || fabs(T - S) <= 15 * max(eps, 1e-14 * fabs(T)))
        return T + (T - S) / 15;
    return simpson_rec(f, a, m, eps / 2, fa, lm, fm, L, d - 1) +
           simpson_rec(f, m, b, eps / 2, fm, rm, fb, R, d - 1);
}

template <class F>
double integrate(F f, double a, double b, double eps = 1e-9) {
    double fa = f(a), fm = f((a + b) / 2), fb = f(b);
    double S = (b - a) / 6 * (fa + 4 * fm + fb);
    return simpson_rec(f, a, b, eps, fa, fm, fb, S, 50);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_integration() {  // to_string(double) prints 6 decimals
    auto cube = [](double x) { return x * x * x; };
    auto half = [](double x) { return 2 * sqrt(max(0.0, 1 - x * x)); };
    auto sn = [](double x) { return sin(x); };
    auto row = [](double x) {  // inner integral of x*y over y in [0, 1]
        return integrate([&](double y) { return x * y; }, 0, 1);
    };
    cout << to_string(integrate(sn, 0, PI)) << '\n';  // 2.000000
    cout << to_string(simpson(cube, 0, 2, 1)) << '\n';  // 4.000000
    // area of the unit circle by integrating its height 2 sqrt(1 - x^2)
    cout << to_string(integrate(half, -1, 1)) << '\n';  // 3.141593
    cout << to_string(integrate(row, 0, 1)) << '\n';    // 0.250000
}
