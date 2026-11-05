/**
 * Usage: digit_dp(N, S, s0, step, acc) -> #x in [0, N] accepted by a digit
 *        automaton: states 0..S-1, start s0, step(s, d) -> next state or -1
 *        (reject), acc(s) -> accept the final state. Digits are read from
 *        the most significant one, WITHOUT leading zeros (0 is read as "0").
 *        Range [L, R]: digit_dp(R, ...) - digit_dp(L - 1, ...).
 * Complexity: O(len(N) * S * 10)
 * Source: UPB DigitDP (recursive, std::function) + old digit_dp (int
 *         overflow, fixed dp[20][200]); automaton formulation: own
 * Note: encode several features in one state (s = sum * 10 + last, ...).
 *       Digit sum % k: S = k, step = (s + d) % k, acc = s == 0.
 *       No two equal adjacent digits: S = 11, s0 = 10, step = d == s ? -1 : d.
 *       Sum of a function over [0, N] (not a count): add a second array
 *       carrying sums next to the counts.
 * Status: ok
 */
template <class Step, class Acc>
ll digit_dp(ll N, int S, int s0, Step step, Acc acc) {
    if (N < 0) return 0;
    string t = to_string(N);
    vl f(S, 0);  // started numbers whose prefix is already < N's prefix
    int ts = s0;  // state of N's own prefix (-1 = rejected)
    for (int i = 0; i < sz(t); i++) {
        int lim = t[i] - '0';
        vl g(S, 0);
        for (int s = 0; s < S; s++)
            if (f[s])
                for (int d = 0; d < 10; d++)
                    if (int ns = step(s, d); ns >= 0) g[ns] += f[s];
        if (ts >= 0)  // leave N's prefix with a smaller digit
            for (int d = i == 0 ? 1 : 0; d < lim; d++)
                if (int ns = step(ts, d); ns >= 0) g[ns]++;
        if (i > 0)  // shorter numbers start here
            for (int d = 1; d <= 9; d++)
                if (int ns = step(s0, d); ns >= 0) g[ns]++;
        if (ts >= 0) ts = step(ts, lim);
        f = g;
    }
    ll r = 0;
    for (int s = 0; s < S; s++) if (acc(s)) r += f[s];
    if (N > 0 && ts >= 0 && acc(ts)) r++;           // N itself
    if (int z = step(s0, 0); z >= 0 && acc(z)) r++;  // the number 0
    return r;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_digit_dp() {
    auto add3 = [](int s, int d) { return (s + d) % 3; };
    auto zero = [](int s) { return s == 0; };
    // multiples of 3 in [0, 100]: digit sum divisible by 3 -> 0, 3, ..., 99
    cout << digit_dp(100, 3, 0, add3, zero) << '\n';  // 34
    auto diff = [](int s, int d) { return d == s ? -1 : d; };  // s = last
    auto any = [](int) { return true; };
    // CSES Counting Numbers: [123, 321] with no equal adjacent digits
    ll c = digit_dp(321, 11, 10, diff, any) - digit_dp(122, 11, 10, diff, any);
    cout << c << '\n';  // 171
}
