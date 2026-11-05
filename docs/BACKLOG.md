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
| 1 | Number Theory | done: 17 topics (pushed effab9a) |
| 2 | Combinatorics | done: 6 snippets + formulas sheet (review) |
| 3 | Algebra & Polynomials | done (review) |
| 4 | Dynamic Programming (incl. optimizations) | done (review) |
| 5 | Data Structures | done (review) |
| 6 | Graphs | done (review) |
| 7 | Flows & Matching | done (review) |
| 8 | Trees | done (review) |
| 9 | Strings | done (review) |
| 10 | Geometry | done (review) |
| 11 | Misc | done (review) |

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
| 11 | Floor sum | U Floor Sums (AtCoder) | done (review) + mod_sum |
| 12 | floor(n/d) blocks | U SQRT Mod (fragment) | done (review) + 2-var blocks, sum d/sigma/mod |
| 13 | Divisors from factorization | U Get All Divisors (fragment) | done (review) + divisor_lists(N) |
| 14 | Legendre: exponent of p in n! | T highest exponent factorial | done (review): vp_fact + trailing zeros base b + Kummer vp_binom |
| 15 | Mobius problem patterns (pairs gcd = k, sum n/gcd) | U x3 (one counts i<=j instead of i<j) | done (review) + squarefree fixed, pillai |

Topic 1 extras added on request: inverse table O(n), batch inverses, run-time modulus `dmint`.

| 16 | Modular square root (Tonelli-Shanks) | proposed by Claude, approved 2026-11-03 | done (review) |
| 17 | Primitive root / multiplicative order | proposed by Claude, approved 2026-11-03 | done (review) |

## 2. Combinatorics (topics chosen by Claude on user delegation, 2026-11-04)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | binomial.cpp: Comb (fact/ifact, C, P, multinomial, stars and bars), C for huge n small k, exact C (i128), Pascal mod any m | P CombinatoricsMod, U Modular Combinatorics (no bounds check), U Combinatorics (overflow), P BinomialDP_Pascal, T counting (perm_rep) | done (review) |
| 2 | lucas.cpp: Lucas (prime p), extra: C(n, k) mod any m (prime powers + CRT) | T Lucas theorem; extra: own | done (review) |
| 3 | catalan.cpp: Catalan, ballot / Catalan convolution, parentheses with prefix | U Catalan (OOB bug), U Catalan Convolution | done (review) + Narayana |
| 4 | counting_sequences.cpp: derangements, Stirling 1st/2nd, Bell, partitions (pentagonal) | U SubFactorial, P isc partition (Java syntax), extras: own | done (review) + stirling2_single |
| 5 | inclusion_exclusion.cpp: count not divisible by any, coprime count, exactly-k formula | P InclusionExclusion | done (review) |
| 6 | burnside.cpp: necklaces / bracelets (Burnside, Polya) | extra: own (missing in all references) | done (review) |
| 7 | tex/theory/combinatorics.tex: formulas sheet (identities, Catalan, Stirling, Fibonacci, Burnside) | U .tex style, T fibonacci properties; every formula verified numerically | done (review) |

## 3. Algebra & Polynomials (topics chosen by Claude on user delegation, 2026-11-04)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | fft.cpp: FFT (precise roots), exact multiply, conv mod any m < 2^31 (split), correlate (cyclic shifts, wildcard) | U FFT, U Interesting FFT MOD (KACTL), P FFT_new (cyclic only), T fft; U shifts trick doc said cyclic | done (review) |
| 2 | ntt.cpp: NTT templated on prime/root (4 primes listed), conv_any (3 primes + Garner) | P NTT, P isc math_ntt, U NTT (61-bit, global arrays), T ntt | done (review) |
| 3 | poly.cpp: power series inv, divmod, log, exp, pow (leading zeros, huge k), multipoint eval | own (refs only define polynomials: U Poly Definitions, T polynomials) | done (review) |
| 4 | lagrange.cpp: P(x) from P(0..n) in O(n), interpolation O(n^2), power sums | own (missing in all references) | done (review) |
| 5 | karatsuba.cpp: any modulus | U Karatsuba (new[] released with free()) | done (review) |
| 6 | fwht.cpp: XOR/AND/OR convolutions (mod p or exact), subset convolution | U Fast Hadamard Transform, P InverseSOS_SubsetConvolution | done (review) |
| 7 | matrix.cpp: mat mul/pow (m < 2^31, lazy reduction), lin_rec, (min,+) powers | P MatrixExponentiation, P FastExpo, U Linear Recurrence Matrix | done (review) |
| 8 | linear_recurrence.cpp: Berlekamp-Massey, Kitamasa, guess_nth | U Cayley-Hamilton (compile bug), P Kitamasa/BM (float pow inverse, k = 1 bug) | done (review) |
| 9 | fibonacci.cpp: doubling mod m <= 1e18, Pisano period, identities | U Fast Fibonacci; pisano: own | done (review) |
| 10 | gauss.cpp: doubles (0/1/inf), mod p solve/rank/det/inverse, det mod any m | P GaussJordan (exit on singular) | done (review) |
| 11 | xor_basis.cpp: insert/contains/max/min/kth/merge, GF(2) bitset elimination | U Xor Basis, P XOR_Basis_Bitset | done (review) |
| 12 | simplex.cpp: two-phase, Bland | P Simplex (double inverse on long double table), P isc math_simplex | done (review) |
| 13 | integration.cpp: composite + adaptive Simpson (relative floor) | P NumericalIntegration | done (review) |

SOS DP (zeta/Mobius over subsets as a DP) is covered by fwht.cpp (OR/AND transforms); a plain SOS DP snippet can still go to section 4 if wanted.

## 4. Dynamic Programming (topics chosen by Claude on user delegation, 2026-11-04)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | lis.cpp: strict / non-strict with reconstruction, count of LIS (Fenwick) | U LIS, P LongestIncreasingSubsequence | done (review) |
| 2 | knapsack.cpp: 0/1 (+ items), unbounded, bounded (binary grouping), by value | P Knapsack01, old knapsack_01, U Multiple Knapsack (value/weight swapped by name) | done (review) |
| 3 | subset_sum.cpp: bitset sums with grouping, count subsets, coin ways / sequences / min | P SubsetSum, P CoinChange, U Coin Change, old subset_sum | done (review) |
| 4 | lcs.cpp: LCS with reconstruction (strings / vectors), LCS of 3, weighted edit distance | P LCS, U LCS / LCS 3 (globals 100^3), U Edit Distance, old lcs / edit_distance | done (review) |
| 5 | digit_dp.cpp: automaton template (any state machine on digits) | P DigitDP, old digit_dp (int overflow) | done (review) |
| 6 | bitmask_dp.cpp: Held-Karp cycle / path with order, SOS (+ inverse, supersets), partitions O(3^n), min rides | P BitmaskDP_Templates, P TSP_HeldKarp, U TSP, U Mask Over Submasks, U SOS (+ reverse), U Elevator | done (review) |
| 7 | max_subarray.cpp: Kadane with indices, 2D O(R^2 C), 3D O(X^2 Y^2 Z) | P Kadane, U Max Sum 1D / 2D / 3D (3D wrong index) | done (review) |
| 8 | cht.cpp: LineContainer (KACTL), MonoCHT (sorted slopes, binary search / pointer) | U Convex Hull Trick, P ConvexHullTrick (equal slopes reversed) | done (review) |
| 9 | li_chao.cpp: lines and segments, dynamic nodes, min | P LiChaoTree | done (review) |
| 10 | dp_opt.cpp: divide and conquer (layer + k-partition), Knuth, Aliens (WQS) | P DP_Optimizations, U Knuth (D&C was a TODO); aliens: own | done (review) |
| 11 | rerooting.cpp: generic lambda interface, iterative | P RerootingDP (recursive) | done (review) |
| 12 | probability.cpp: dice pmf / counts, Markov powers, absorbing chains (Gauss) | P ProbabilityDP_Dice, P MarkovSmall, P ExpectedValueDP_Rolls | done (review) |
| 13 | meet_middle.cpp: sorted subset sums by merging, exact count, best <= W | P MeetInTheMiddle | done (review) |
| 14 | tex/theory/dp.tex: when each optimization applies (QI, Knuth, CHT, Aliens, monotone queue, separation, bitset, expected value) | U Separation Optimization, U pragmas for bitset, P DP_Optimizations notes | done (review) |

## 5. Data Structures (topics chosen by Claude on user delegation, 2026-11-04)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | dsu.cpp: DSU (size, halving), rollback DSU (undo to a time), weighted / potential DSU | U DSU, P UnionFind, P DSU_Rollback (wrong rank undo), U Dynamic Connectivity, old union_find | done (review) |
| 2 | fenwick.cpp: BIT (O(n) build, lower_bound), range add + range sum, 2D | U Fenwick 1D / 2D, P BIT (std name clash), P Fenwick2D, old fenwick_tree (RURQ overflow) | done (review) |
| 3 | segtree.cpp: generic iterative monoid tree, max_right / min_left | U General Iterative Segment Tree, AtCoder; old segment_tree (size ctor, -1 sentinel) | done (review) |
| 4 | lazy_segtree.cpp: assign + add, sum + min (editable hooks) | U General Lazy Tree (stale sibling), U Iterative Lazytree, P SegmentTreeLazy (flip) | done (review) |
| 5 | sparse_table.cpp: idempotent O(1), disjoint sparse table (any associative op) | U Min Sparse Table, P SparseTable, P DisjointSparseTable (std::function) | done (review) |
| 6 | persistent_segtree.cpp: point add, range sum, range k-th by versions | U PersistantSegmentTree (pointers), T persistent seg tree | done (review) |
| 7 | treap.cpp: implicit treap with split / join, reverse, range add, sum | P isc ds_treap (erase rotation breaks heap), T STL treap | done (review) |
| 8 | mo.cpp: generic add / rem / get, Hilbert or zig-zag order | U Mo's, U Mo's Hilbert, P MosAlgorithm, T Mo's | done (review) |
| 9 | monotonic_queue.cpp: window min / max, MonoQueue, two-stack SWAG (any associative op) | P MonotonicQueue, T sliding window | done (review) |
| 10 | xor_trie.cpp: multiset trie, max / min xor, count_less, kth | P BinaryTrieXOR, U Xor Trie, T trie | done (review) |
| 11 | wavelet_tree.cpp: range kth and count <= x, any value range | T wavelet tree | done (review) |
| 12 | dynamic_connectivity.cpp: offline segment tree over time + rollback DSU | U Dynamic Connectivity | done (review) |
| 13 | segtree_beats.cpp: chmin / chmax / add, sum / max / min | own (gap in all references) | done (review) |

Not added (covered elsewhere or rarely needed): ordered set / multiset (template `oset`, use pairs
(value, id) for a multiset), custom hash (template), merge sort tree (wavelet / PST cover it),
2D segment tree (U, for non-sum 2D queries), block tree / sqrt decomposition (problem-specific),
rope (U: slower than the treap), KD tree (P: geometry), link-cut / HLD / centroid (trees section).

## 6. Graphs (topics chosen by Claude on user delegation, 2026-11-05)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | bfs.cpp: multi-source BFS + paths, 0-1 BFS, bipartite check | U BFS, P BFS01, U DFS (never left the start node) | done (review) |
| 2 | dijkstra.cpp: multi-source with parents, k shortest walks | P FastDijkstra, P MultiSourceDijkstra, U Dijkstra (std::distance clash), U K-Shortest | done (review) |
| 3 | bellman_ford.cpp: -inf propagation, negative cycle extraction | U BellmanFord (+ negative cycle), P BellmanFord_SPFA (SPFA hangs) | done (review) |
| 4 | floyd.cpp: negative weights with -inf, path reconstruction, bitset closure, minimax | U Floyd (no d[i][i] = 0), U Floyd negative, U Transitive Closure, P DijkstraFloyd, old minimax_all_pairs | done (review) |
| 5 | toposort.cpp: Kahn (+ lexicographic), directed cycle (iterative), DAG longest path | U Topological Sort, P TopologicalSort, P TopoSort_DAG_LongestPath, U Cycle Detection | done (review) |
| 6 | scc.cpp: iterative Tarjan, topological component ids, condensation | U SCC (Tarjan), P SCC, P isc graph_scc, T tarjan / kosaraju | done (review) |
| 7 | two_sat.cpp: either / imply / force / at_most_one | U Two Sat, P TwoSAT, T two sat | done (review) |
| 8 | bridges.cpp: iterative lowlink, bridges, cut vertices, 2-edge comps, vertex BCCs | P BridgesArticulation_BCC (comp never filled), P isc graph_bcc, T bridges | done (review) |
| 9 | euler.cpp: directed + undirected trail / circuit, optional start, edge order | P EulerianPath (isolated vertex 0), T eulerian path | done (review) |
| 10 | mst.cpp: Kruskal (forest, chosen ids), dense Prim O(n^2) | U Kruskal (always max), P Kruskal, P MST_Kruskal_Prim, P Prim, old minimax_mst_path | done (review) |
| 11 | directed_mst.cpp: Chu-Liu / Edmonds O(nm) | T directed mst | done (review) |
| 12 | min_mean_cycle.cpp: Karp, exact fraction | P MinMeanCycle_Karp (double), T karp | done (review) |
| 13 | dominator_tree.cpp: iterative Lengauer-Tarjan | own (gap in all references) | done (review) |

Not added: Yen's k shortest SIMPLE paths (P, rare), SPFA (BF is safer), Johnson (note in dijkstra).

## 7. Flows & Matching (topics chosen by Claude on user delegation, 2026-11-05)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | dinic.cpp: edge ids, edge flows, min cut side, incremental | P Dinic (code/), U Max Flow, old dinic (did not compile), P isc graph_dinic (wrong return) | done (review) |
| 2 | push_relabel.cpp: highest label + gap | P PushRelabel, KACTL | done (review) |
| 3 | mcmf.cpp: potentials (negative costs via Bellman-Ford), flow limit, edge flows | P MinCostMaxFlowPotentials, P MinCostMaxFlow, P isc mincost, U MCMF (did not compile) | done (review) |
| 4 | hungarian.cpp: n <= m assignment, 0-indexed | U Hungarian, P Hungarian, P MinCostMatching | done (review) |
| 5 | bipartite_matching.cpp: Hopcroft-Karp, König cover, DAG path cover | P HopcroftKarp, P isc HK (dist reset), P MaxBipartiteMatching, T konig / path cover | done (review) |
| 6 | global_min_cut.cpp: Stoer-Wagner with side | P MinCut (int), U Min Cut (unit s-t only) | done (review) |
| 7 | gomory_hu.cpp: Gusfield flow tree + path query | P GomoryHuTree | done (review) |
| 8 | lower_bounds.cpp: max flow with lower bounds, circulations | own (gap) | done (review) |
| 9 | general_matching.cpp: Edmonds blossom O(V^3) | own (gap) | done (review) |
| 10 | tex/theory/flows.tex: modeling sheet (gadgets, Menger, closure, König, Hall, Dilworth, Tutte, lower bounds, min cost) | T konig / path cover, own | done (review) |

Not added: weighted general matching (too long), P GraphCutInference (= closure model in the sheet),
P DinicScaling (Dinic is enough).

## 8. Trees (topics chosen by Claude on user delegation, 2026-11-05)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | lca.cpp: binary lifting (BFS build), dist, k-th ancestor, k-th on path, path max | U LCA, U K-th Parent, P LCA, P BinaryLiftingExtras, old lca_binary_lifting, old minimax_mst_path | done (review) |
| 2 | euler_tour.cpp: subtree ranges, O(1) LCA (Euler + sparse table), iterative | U Euler Tour - Sum, P EulerTour, P EulerTour_Queries, old lca_euler_segtree | done (review) |
| 3 | hld.cpp: path ranges (vertex / edge mode), subtree, lca, iterative | U Heavy-Light, P HLD, P isc HLD, T heavy light | done (review) |
| 4 | centroid.cpp: centroid tree + distances to centroid ancestors, iterative | U Centroid, U Online Centroid, P CentroidDecomposition, P isc centroid | done (review) |
| 5 | sack.cpp: generic DSU on tree, explicit stack | P DSU_On_Tree_Sack (vertex 0 as "no heavy child") | done (review) |
| 6 | virtual_tree.cpp: keys + LCAs, parent edges | P isc misc_virtual_tree (globals, problem code) | done (review) |
| 7 | tree_misc.cpp: weighted diameter, centers, AHU isomorphism, Prufer encode / decode | own (gaps) | done (review) |
| 8 | link_cut.cpp: link / cut / connected / path sum / rooted lca | P LinkCutTree, P isc ds_link_cut_tree (connected wrong) | done (review) |
| 9 | mo_tree.cpp: Mo on paths with entry / exit flattening + lca | U Mo's in Trees, P MosOnTree_Hilbert (Z-order, not Hilbert), T Mo's on trees | done (review) |

Rerooting DP lives in section 4 (dp/rerooting.cpp); tree knapsack and "count paths of length k" patterns
are solved with centroid.cpp / sack.cpp.

## 9. Strings (topics chosen by Claude on user delegation, 2026-11-05)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | prefix_function.cpp: pi, KMP (empty pattern ok), period, KMP automaton | U KMP, U KMP Automaton, P KMP, P KMP_Automaton, P isc string_KMP, old kmp (empty pattern bug) | done (review) |
| 2 | z_function.cpp: Z, matching (empty pattern ok) | P ZFunction, P isc string_z_function (wrong index), old z_function | done (review) |
| 3 | hashing.cpp: mod 2^61 - 1, random base, unsigned bytes, lcp | U Fast Hashing (fixed bases), P RollingHash, old rabin_karp (empty pattern, bytes >= 128) | done (review) |
| 4 | manacher.cpp: d1 / d2, O(1) is_pal, longest, count | P Manacher | done (review) |
| 5 | suffix_array.cpp: O(n log n) with sentinel, Kasai LCP | P SuffixArrayLCP (cyclic, wrong), P isc string_SA, P isc kasai | done (review) |
| 6 | suffix_automaton.cpp: distinct, occurrences, LCS | P SuffixAutomaton (120 MB reserve), P isc SAM, T suffix automaton | done (review) |
| 7 | aho_corasick.cpp: array automaton, counts per pattern (suffix chains) | P AhoCorasick (missed suffix matches), P isc string_AC, T incremental AC | done (review) |
| 8 | eertree.cpp: distinct palindromes, occ, first position, ends[i] | P PalindromicTree_Eertree | done (review) |
| 9 | lyndon.cpp: Duval factorization, minimal rotation (smallest index) | P Booth_Duval, T minimal string rotation | done (review) |

Not added: Ukkonen suffix tree (P isc, SA / SAM cover it), trie (trivial; see xor_trie / aho),
wildcard matching (fft.cpp correlate note).

## 10. Geometry (topics chosen by Claude on user delegation, 2026-11-05)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | point.cpp: Pt<T> (ll exact / ld), cross / dot / rot / perp, exact angular sort | P Geometry.cpp (Stanford), U Convex Hull pt, P JavaGeometry | done (review) |
| 2 | segments.cpp: on_segment, seg_intersect, seg_inter (point / overlap), line_inter, projection, distances | P Geometry.cpp, U Segment Intersection (slopes, printed, wrong), KACTL | done (review) |
| 3 | polygon.cpp: area2, in_polygon (any simple), in_convex O(log n), Pick, convex cut, centroid | U Point In General / Convex Polygon, P Geometry.cpp, U Some Formulas | done (review) |
| 4 | convex_hull.cpp: monotone chain (no collinear), rotating calipers diameter | U Convex Hull (empty when all equal), P ConvexHull, U / P Polygon Diameter | done (review) |
| 5 | closest_pair.cpp: O(n log n) sweep with set | U Closest Pair (O(n^2) on equal x, TLE), T closest pair | done (review) |
| 6 | circles.cpp: circle-line, circle-circle, tangents, circumcircle, Welzl MEC, lens area | P Geometry.cpp, T circle formulas | done (review) |
| 7 | half_plane.cpp: deque HPI with bounding box, degenerate results -> {} | P HalfPlaneIntersection (parallel decided with the origin) | done (review) |
| 8 | minkowski.cpp: convex Minkowski sum | P MinkowskiSum, P ConvexPolygonIntersection (-> HPI) | done (review) |
| 9 | point3d.cpp: P3, vol6, plane distance, line-plane, polyhedron volume, haversine | P Geom3D (Java), U glass volume | done (review) |
| 10 | tex/theory/geometry.tex: triangle / polygon / circle / solids / vectors / precision (verified) | U Some Formulas, T triangles | done (review) |

Not added: 3D convex hull, Delaunay / Voronoi (P Delaunay O(n^4)), KD tree (P), CDQ (P), union of
circles / polygons areas (sweeps), polygon triangulation.

## 11. Misc (topics chosen by Claude on user delegation, 2026-11-05)

| # | Topic / file | Sources | Done |
|---|---|---|---|
| 1 | dates.cpp: day numbers (proleptic Gregorian, any year), inverse, weekday, leap | P Dates (int Julian days), P Dates.java, T dates | done (review) |
| 2 | bit_tricks.cpp: Gosper next combination, Gray code + inverse, submask iteration, builtin notes | U Bit Tricks, P BitmaskTricks | done (review) |
| 3 | search.cpp: first_true, real search, integer / real ternary, compression, inversions | U Ternary Search | done (review) |
| 4 | fraction.cpp: exact reduced fractions (i128 middle steps), best rational approximation | T fraction, U Fraction Modular (mixed % MOD) | done (review) |
| 5 | bigint.cpp: base 1e9 non-negative + - *, / and % by small | own (gap) | done (review) |
| 6 | backtracking.cpp: N-queens with bitmasks (count + one solution) | old backtracking_template, old n_queens | done (review) |
| 7 | random_gen.cpp: permutations, distinct values, trees, connected simple graphs, strings | U Randoms, P RandomSTL | done (review) |
| 8 | tex/theory/contest.tex: stress loop (bash), debug flags, Python recipes (all run) | U Decimal Precision Python | done (review) |

Template already covers: __int128 I/O, rng / randint, pragmas, custom hash, oset, debug macro.

## Deferred to later sections (do not lose)

All deferred items were handled (or explicitly left out) in their sections.

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

- Lines <= 80 columns (`check` warns): the PDF has 2 columns of 6pt code. A long `cout` in an
  example wraps after the `// expected` comment (the comment must stay on the `cout` line).
- Type names defined by another snippet (e.g. `vd` from fft.cpp) must be re-typedef'd (same type)
  or the snippet does not compile alone.

- Never use a template macro name as an identifier (`all`, `sz`, `F`, `S`, `pb`, `SQ`, ...):
  `st.all()` expands the macro. Members named `max` / `min` hide std::max inside the class.

- CHECK is a statement (do-while): never chain it with commas.
- Explanations go on their own comment line, never after the `// expected` of a `cout` line.

- Never name a function like a std:: one: C++17 special math functions (legendre, beta,
  expint, hermite, laguerre, riemann_zeta, ...) and gcd/lcm/size/data/distance/hash win
  overload resolution silently under `using namespace std` (legendre(100,5) printed 2e98).

## Tooling notes
- Template has main()/init()/solve(): stress tests include it via
  `tests/stress/template_lib.h` (renames the template's main). The check gate is unaffected
  (template + snippet has a single main).
- The runner passes `-DNB_ROOT="<repo>"` so tests can read snippet sources.
