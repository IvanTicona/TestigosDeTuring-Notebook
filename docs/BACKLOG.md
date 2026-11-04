# Notebook backlog

Physical record of what is pending, so nothing depends on chat memory.
**Rule:** before starting a section, read its block here and pull in anything pending.
Sources: **P** = UPB isc-teambook, **U** = UMSS competitive, **T** = UTP Pereira PDF
(`isc-teambook/notebook (1).pdf`), **old** = our own previous snippets in `delete/`.
Catalog of every reference file: `D:/Active Projects/teambook-refs/analysis/`
(artifact: https://claude.ai/artifact/71SWWq3urKbsy2wBqrYmLy).

## Skeleton (approved 2026-11-03)

| # | Section | Status |
|---|---|---|
| 0 | Template | done (85a0fdc) |
| 1 | Number Theory | in progress: 17 topics approved |
| 2 | Combinatorics | pending |
| 3 | Algebra & Polynomials | pending |
| 4 | Dynamic Programming (incl. optimizations) | pending |
| 5 | Data Structures | pending |
| 6 | Graphs | pending |
| 7 | Flows & Matching | pending |
| 8 | Trees | pending |
| 9 | Strings | pending |
| 10 | Geometry | pending |
| 11 | Misc | pending |

## 1. Number Theory (approved topics)

| # | Topic | Candidate sources | Done |
|---|---|---|---|
| 1 | Modular arithmetic (mulmod, powmod, inverse) | P ModularArithmetic, U Fraction Modular, T mod mul/pow/inv/mod integer | done |
| 2 | Extended Euclid | P Euclid.cpp (U Extended Euclides is BROKEN) | done (review) |
| 3 | Linear Diophantine | P Euclid.cpp (U broken: b=0 crash, bad euclid) | done (review) |
| 4 | CRT (non-coprime) + Garner | U Chinese Remainder (overflow risk), P CRT_Garner | done (review) |
| 5 | Sieves (linear/SPF, segmented, prime table) | P PrimesSieve, U Linear Sieve, P SegmentedSieve, P Primes | done (review) |
| 6 | Multiplicative functions (phi, mu, d, sigma) | P DivisorsTotientMobius, U Mobius (+ .tex theory), T totient/sigma | done (review) |
| 7 | Primality + factorization 64-bit | U Optimized Pollard Rho (only correct one); P versions BROKEN | done (review) |
| 8 | Discrete log (BSGS, gcd != 1) | U Discrete Log (m=1 edge); P BSGS BROKEN | done (review) + discrete_root |
| 9 | Huge exponent a^(b^c) | U Big Exponent (edge bug when a % MOD == 0) | done (review): powmod_str + tower, any m |
| 10 | Prime counting pi(n) | U Count Primes | done (review): Lucy (+ prime sums) + UMSS PrimeCounter |
| 11 | Floor sum | U Floor Sums (AtCoder) | |
| 12 | floor(n/d) blocks | U SQRT Mod (fragment) | |
| 13 | Divisors from factorization | U Get All Divisors (fragment) | |
| 14 | Legendre: exponent of p in n! | T highest exponent factorial | |
| 15 | Mobius problem patterns (pairs gcd = k, sum n/gcd) | U x3 (one counts i<=j instead of i<j) | |

Topic 1 extras added on request: inverse table O(n), batch inverses, run-time modulus `dmint`.

| 16 | Modular square root (Tonelli-Shanks) | proposed by Claude, approved 2026-11-03 | done (review) |
| 17 | Primitive root / multiplicative order | proposed by Claude, approved 2026-11-03 | done (review) |

## Deferred to later sections (do not lose)

### 2. Combinatorics
- nCr mod p with factorials: U Modular Combinatorics, P CombinatoricsMod
- Exact nCk (no modulus): U Combinatorics
- Lucas theorem: T
- Catalan numbers: U Catalan (BUG: fact written out of bounds), U Catalan Convolution
- Derangements / subfactorial: U SubFactorial
- Inclusion-exclusion patterns: P InclusionExclusion
- Pascal triangle: P BinomialDP_Pascal
- Counting formulas: T counting, T fibonacci properties

### 3. Algebra & Polynomials
- NTT: U NTT (61-bit prime), P NTT (998244353), P isc-notebook math_ntt (1004535809), T
- FFT: U FFT, P FFT_new, T fft; U FFT shifts trick (doc: cyclic, not linear)
- Karatsuba: U; Fast Walsh-Hadamard: U
- Matrix exponentiation: P MatrixExponentiation, P FastExpo, U Linear Recurrence Matrix
- Linear recurrences: U Cayley-Hamilton (latent compile bug in pow), P Kitamasa/BM (BROKEN)
- Fast Fibonacci (doubling): U
- Gauss / RREF / determinant / rank: P
- Simplex: P; Simpson integration: P
- XOR basis: U Xor Basis, P XOR_Basis_Bitset
- Subset convolution / inverse SOS: P InverseSOS_SubsetConvolution (maybe DP)
- Polynomials: T polynomials, U Poly Definitions

### 4. Dynamic Programming
- old (delete/src/dp): edit_distance, lcs, knapsack_01(+1d), subset_sum(+1d), digit_dp (BUG: int overflow)
- old stress tests in delete/tests/stress/dp

### 5. Data Structures
- old: fenwick_tree (BUG: RURQ int overflow), segment_tree (BUGS: size ctor, -1 sentinel), union_find

### 8. Trees
- old: lca_euler_segtree, lca_binary_lifting (quarantined), minimax_all_pairs (BUG unreachable=0), minimax_mst_path (unused mst_cost)

### 9. Strings
- old: z_function, kmp (BUG: empty pattern), rabin_karp (BUGS: empty pattern, bytes >= 128)

### 11. Misc
- old: backtracking_template, n_queens
- Template extras not used: possible()/impossible(), max3/min4, Python decimal precision (U)

### 7. Flows
- old: dinic (quarantined, Benq macros)

## Snippet conventions
- Each snippet ends with `void example_<name>()`: short usage demo; every `cout` line
  carries a trailing `// expected output`. The stress test calls
  `stress::check_example(example_<name>, "src/...", "example_<name>")`, which runs it and
  fails if any comment does not match what is printed (examples can never lie).
- Header: Usage / Complexity / Source / Note / Depends / Status (`untested` until the user approves).
- `Depends: src/a.cpp, src/b.cpp` (or one `Depends:` line per file): snippets this one needs
  (copy them too in a contest).
  `check` compiles template + dependencies + snippet and requires each dependency to be
  listed earlier in contents.txt.

## Tooling notes
- Template has main()/init()/solve(): stress tests include it via
  `tests/stress/template_lib.h` (renames the template's main). The check gate is unaffected
  (template + snippet has a single main).
- The runner passes `-DNB_ROOT="<repo>"` so tests can read snippet sources.
