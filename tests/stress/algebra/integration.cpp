// Stress test for src/algebra/integration.cpp
// Oracles: closed-form antiderivatives: random polynomials of degree <= 6
// (Simpson exact up to degree 3), exp(k x), sin/cos, 1/(1+x^2) (atan),
// sqrt (circle area); 2D nested integral of a separable function.
#include "tests/stress/template_lib.h"
#include "src/algebra/integration.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_integration, "src/algebra/integration.cpp", "example_integration");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int d = stress::rnd(0, 6);
        vector<double> c(d + 1);
        for (double &v : c) v = stress::rnd(-50, 50) / 7.0;
        double a = stress::rnd(-300, 300) / 100.0, b = a + stress::rnd(0, 500) / 100.0;
        auto f = [&](double x) { double r = 0; for (int i = d; i >= 0; i--) r = r * x + c[i]; return r; };
        auto Fp = [&](double x) { double r = 0; for (int i = d; i >= 0; i--) r = r * x + c[i] / (i + 1); return r * x; };
        double want = Fp(b) - Fp(a), tol = 1e-6 * max(1.0, fabs(want));
        CHECK(fabs(integrate(f, a, b) - want) < tol, "adaptive poly d=" + to_string(d));
        if (d <= 3) CHECK(fabs(simpson(f, a, b, stress::rnd(1, 5)) - want) < tol, "simpson exact on cubics");
        CHECK(fabs(simpson(f, a, b, 2000) - want) < tol, "composite poly d=" + to_string(d));
        double k = stress::rnd(-30, 30) / 10.0;
        double we = k == 0 ? b - a : (exp(k * b) - exp(k * a)) / k;
        CHECK(fabs(integrate([&](double x) { return exp(k * x); }, a, b) - we) < 1e-6 * max(1.0, fabs(we)), "exp");
        CHECK(fabs(integrate([](double x) { return cos(x); }, a, b) - (sin(b) - sin(a))) < 1e-7, "cos");
        CHECK(fabs(integrate([](double x) { return 1 / (1 + x * x); }, a, b) - (atan(b) - atan(a))) < 1e-7, "atan");
    }
    double r = stress::rnd(1, 100) / 10.0;
    double circ = integrate([&](double x) { return 2 * sqrt(max(0.0, r * r - x * x)); }, -r, r);
    CHECK(fabs(circ - PI * r * r) < 1e-5 * r * r, "circle area");
    double dbl = integrate([](double x) { return integrate([&](double y) { return exp(x) * cos(y); }, 0, 1); }, 0, 2);
    CHECK(fabs(dbl - (exp(2) - 1) * sin(1)) < 1e-7, "2D separable");
    // periodic integrands vanishing at the first 5 samples (old: returned 0)
    for (int k = 1; k <= 3; k++) {
        double v = integrate([&](double x) { return sin(4 * k * x) * sin(4 * k * x); }, 0, PI);
        CHECK(fabs(v - PI / 2) < 1e-7, "periodic k=" + to_string(k));
    }
    puts("OK");
}
