/**
 * Usage: next_comb(x)  -> next larger number with the same popcount
 *                         (Gosper's hack): iterate all k-subsets of n bits
 *        gray(i), gray_inv(g) -> i-th Gray code (consecutive codes differ
 *                         in 1 bit) and its inverse
 *        for_submasks(m, f) -> f(s) for every s subset of m, decreasing
 *                         (s = 0 included)
 * Complexity: O(1) / O(log) per call; submasks O(2^popcount(m))
 * Source: UMSS Bit Tricks + UPB BitmaskTricks (popcount wrappers, SOS);
 *         Gosper / Gray: own
 * Note: x & -x lowest set bit; x & (x - 1) clears it; __builtin_popcountll,
 *       __builtin_ctzll / clzll (undefined for 0), __lg(x) = floor(log2 x),
 *       __builtin_parityll. Iterate set bits: for (y = x; y; y &= y - 1)
 *       bit = __builtin_ctzll(y). Mask of n ones: (1LL << n) - 1 (n < 63).
 *       bitset<N>: _Find_first(), _Find_next(i), count(), shifts for DP.
 *       All submasks of all masks: 3^n total.
 * Status: ok
 */
ull next_comb(ull x) {  // x != 0
    ull c = x & -x, r = x + c;
    return (((r ^ x) >> 2) / c) | r;
}
ull gray(ull i) { return i ^ (i >> 1); }
ull gray_inv(ull g) {
    for (int s = 1; s < 64; s <<= 1) g ^= g >> s;
    return g;
}
template <class F>
void for_submasks(ull m, F f) {
    for (ull s = m;; s = (s - 1) & m) {
        f(s);
        if (!s) break;
    }
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_bit_tricks() {
    ull x = 0b0111;  // 3-subsets of 5 bits: 00111, 01011, 01101, ...
    x = next_comb(x);
    cout << x << ' ' << next_comb(x) << ' '  // 11 13 7 5
         << gray(5) << ' ' << gray_inv(7) << '\n';
    int cnt = 0;
    for_submasks(0b1011, [&](ull) { cnt++; });
    cout << cnt << ' ' << __builtin_popcountll(0b1011) << '\n';  // 8 3
}
