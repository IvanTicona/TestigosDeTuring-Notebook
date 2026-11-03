// Shared helpers for stress tests (tests/stress/**/*.cpp). Not part of the notebook.
//
// A stress test is one translation unit, in this order:
//   #include "template/template.cpp"
//   #include "src/<snippet>.cpp"
//   #include "tests/stress/common.h"
// compiled by `python scripts/notebook.py test` with -I<repo root>.
//
// Everything lives in namespace `stress` (snippets own many global names:
// n, s, dp, solve, N, ...), except the CHECK macro.
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

namespace stress {

inline uint64_t read_seed() {
    const char *e = std::getenv("SEED");
    if (e && *e) return std::strtoull(e, nullptr, 0);
    return 0x5EED;
}

inline const uint64_t seed = read_seed();
inline std::mt19937_64 rng(seed);
// Set by the test at the top of each randomized iteration; reported by CHECK.
inline long long iter = 0;

// Uniform integer in [lo, hi] (inclusive).
inline long long rnd(long long lo, long long hi) {
    return std::uniform_int_distribution<long long>(lo, hi)(rng);
}

// Random string of length n over the characters [lo, hi].
inline std::string rnd_str(int n, char lo, char hi) {
    std::string s(n, lo);
    for (auto &c : s) c = (char)rnd(lo, hi);
    return s;
}

[[noreturn]] inline void fail(const char *file, int line, const char *cond, const std::string &input) {
    std::fprintf(stderr, "CHECK FAILED at %s:%d: %s\n  seed=%llu (rerun: SEED=%llu or --seed %llu)\n  iteration=%lld\n  input: %s\n",
                 file, line, cond, (unsigned long long)seed, (unsigned long long)seed,
                 (unsigned long long)seed, iter, input.c_str());
    std::fflush(stderr);
    std::exit(1);
}

// Make stdin read `text` (for snippets that use scanf/cin). The text goes to a
// file named after the test source (<file>.in) in the current directory, which
// the runner sets to the test's own build dir. Call before the first read.
inline void feed_stdin(const std::string &text, const char *src = __builtin_FILE()) {
    std::string name(src);
    size_t slash = name.find_last_of("/\\");
    if (slash != std::string::npos) name = name.substr(slash + 1);
    name += ".in";
    FILE *f = std::fopen(name.c_str(), "wb");
    if (!f) { std::perror("feed_stdin"); std::exit(1); }
    std::fwrite(text.data(), 1, text.size(), f);
    std::fclose(f);
    if (!std::freopen(name.c_str(), "rb", stdin)) { std::perror("feed_stdin"); std::exit(1); }
    std::cin.clear();
}

// RAII capture of std::cout (for snippets that print their results).
struct Capture {
    std::ostringstream out;
    std::streambuf *old;
    Capture() : old(std::cout.rdbuf(out.rdbuf())) {}
    ~Capture() { std::cout.rdbuf(old); }
    Capture(const Capture &) = delete;
    Capture &operator=(const Capture &) = delete;
    std::string str() const { return out.str(); }
};

}  // namespace stress

// CHECK(cond, input): on failure prints seed, iteration and `input` (anything
// convertible to std::string, built lazily) and exits 1.
#define CHECK(cond, input) \
    do { if (!(cond)) ::stress::fail(__FILE__, __LINE__, #cond, (input)); } while (0)
