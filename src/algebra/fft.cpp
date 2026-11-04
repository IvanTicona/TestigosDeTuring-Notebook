/**
 * Usage: conv(a, b)         -> real convolution, c[k] = sum a[i] b[k-i] (vd)
 *        multiply(a, b)     -> exact integer convolution (vl)
 *        conv_mod(a, b, m)  -> convolution mod ANY m < 2^31 (no NTT prime
 *                              needed), inputs in [0, m)
 *        correlate(a, b)    -> r[i] = sum_j a[i+j] b[j], i = 0..|a|-|b|
 * Complexity: O(n log n), n = |a| + |b|
 * Source: UMSS FFT (cp-algorithms, 2 FFTs per product, plain double roots) +
 *         UMSS Interesting FFT MOD (KACTL conv / convMod) + UPB FFT_new
 *         (recursive, cyclic only) + UTP fft; roots in long double (KACTL)
 * Note: result size |a| + |b| - 1 (LINEAR, not cyclic; UMSS "shifts trick"
 *       doc called it cyclic). multiply rounds: safe while
 *       (sum a_i^2 + sum b_i^2) * log2(n) < ~9e14; otherwise use conv_mod
 *       or NTT. Cyclic shift scores of a (n items): correlate(a + a, a)
 *       gives sum_j a[(i+j) % n] a[j] for i = 0..n.
 *       Wildcard matching: sum p_j t_{i+j} (p_j - t_{i+j})^2 with 0 = '?'
 *       = 3 correlations.
 * Status: untested
 */
typedef complex<double> cpx;
typedef vector<double> vd;

void fft(vector<cpx> &a) {
    int n = sz(a), L = 31 - __builtin_clz(n);
    static vector<complex<long double>> R(2, 1);
    static vector<cpx> rt(2, 1);  // rt[k + j] = e^(i pi j / k), precise
    for (static int k = 2; k < n; k *= 2) {
        R.resize(n), rt.resize(n);
        auto x = polar(1.0L, acosl(-1) / k);
        for (int i = k; i < 2 * k; i++)
            rt[i] = R[i] = i & 1 ? R[i / 2] * x : R[i / 2];
    }
    vi rev(n);
    for (int i = 0; i < n; i++) rev[i] = (rev[i / 2] | (i & 1) << L) / 2;
    for (int i = 0; i < n; i++) if (i < rev[i]) swap(a[i], a[rev[i]]);
    for (int k = 1; k < n; k *= 2)
        for (int i = 0; i < n; i += 2 * k)
            for (int j = 0; j < k; j++) {
                cpx z = rt[j + k] * a[i + j + k];
                a[i + j + k] = a[i + j] - z;
                a[i + j] += z;
            }
}

vd conv(const vd &a, const vd &b) {  // 1 product = 2 FFTs (a real, b imag)
    if (a.empty() || b.empty()) return {};
    vd res(sz(a) + sz(b) - 1);
    int L = 32 - __builtin_clz(sz(res)), n = 1 << L;
    vector<cpx> in(n), out(n);
    copy(all(a), begin(in));
    for (int i = 0; i < sz(b); i++) in[i].imag(b[i]);
    fft(in);
    for (cpx &x : in) x *= x;
    for (int i = 0; i < n; i++) out[i] = in[-i & (n - 1)] - conj(in[i]);
    fft(out);
    for (int i = 0; i < sz(res); i++) res[i] = imag(out[i]) / (4 * n);
    return res;
}

vl multiply(const vl &a, const vl &b) {
    vd c = conv(vd(all(a)), vd(all(b)));
    vl r(sz(c));
    for (int i = 0; i < sz(c); i++) r[i] = llround(c[i]);
    return r;
}

vl conv_mod(const vl &a, const vl &b, ll m) {  // split x = hi * cut + lo
    if (a.empty() || b.empty()) return {};
    vl res(sz(a) + sz(b) - 1);
    int B = 32 - __builtin_clz(sz(res)), n = 1 << B;
    ll cut = (ll)sqrtl(m) + 1;
    vector<cpx> L(n), R(n), outs(n), outl(n);
    for (int i = 0; i < sz(a); i++) L[i] = cpx(a[i] / cut, a[i] % cut);
    for (int i = 0; i < sz(b); i++) R[i] = cpx(b[i] / cut, b[i] % cut);
    fft(L), fft(R);
    for (int i = 0; i < n; i++) {
        int j = -i & (n - 1);
        outl[j] = (L[i] + conj(L[j])) * R[i] / (2.0 * n);
        outs[j] = (L[i] - conj(L[j])) * R[i] / (2.0 * n) * cpx(0, -1);
    }
    fft(outl), fft(outs);
    for (int i = 0; i < sz(res); i++) {
        ll av = llround(real(outl[i])) % m, cv = llround(imag(outs[i])) % m;
        ll bv = (llround(imag(outl[i])) + llround(real(outs[i]))) % m;
        res[i] = ((av * cut % m + bv) % m * cut % m + cv) % m;
    }
    return res;
}

vl correlate(const vl &a, const vl &b) {  // |b| <= |a|
    vl rb(rall(b)), p = multiply(a, rb), r(sz(a) - sz(b) + 1);
    for (int i = 0; i < sz(r); i++) r[i] = p[i + sz(b) - 1];
    return r;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_fft() {
    vl c = multiply({1, 2, 3}, {4, 5});  // (1+2x+3x^2)(4+5x)
    cout << c[0] << ' ' << c[1] << ' ' << c[2] << ' ' << c[3]  // 4 13 22 15
         << '\n';
    vl d = conv_mod({MOD - 1, 2}, {MOD - 1, 3}, MOD);  // (-1+2x)(-1+3x)
    cout << d[0] << ' ' << d[1] << ' ' << d[2] << '\n';  // 1 1000000002 6
    vl r = correlate({1, 2, 3, 1, 2, 3}, {1, 2, 3});  // cyclic shifts of 123
    cout << r[0] << ' ' << r[1] << ' ' << r[2] << ' ' << r[3]  // 14 11 11 14
         << '\n';
}
