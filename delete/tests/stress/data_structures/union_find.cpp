// Stress test for src/data_structures/union_find.cpp
// Oracle: a label array; union relabels every element of one set (O(N) per union).
#include "template/template.cpp"
#include "src/data_structures/union_find.cpp"
#include "tests/stress/common.h"

static string t_ops(int N, const vpii &ops) {
    string s = "N=" + to_string(N) + " unions:";
    for (auto &o : ops) s += " (" + to_string(o.first) + "," + to_string(o.second) + ")";
    return s;
}

struct TOracle {
    vi lab;
    int sets;
    TOracle(int N) : lab(N), sets(N) { iota(all(lab), 0); }
    void unite(int i, int j) {
        if (lab[i] == lab[j]) return;
        int from = lab[j], to = lab[i];
        for (auto &l : lab) if (l == from) l = to;
        sets--;
    }
    int size(int i) const { return (int)count(all(lab), lab[i]); }
};

static void t_check_all(UnionFind &uf, TOracle &o, const string &tag) {
    int N = sz(o.lab);
    CHECK(uf.numDisjointSets() == o.sets, tag + " numDisjointSets");
    for (int i = 0; i < N; i++) {
        CHECK(uf.sizeOfSet(i) == o.size(i), tag + " sizeOfSet(" + to_string(i) + ")");
        for (int j = 0; j < N; j++)
            CHECK(uf.isSameSet(i, j) == (o.lab[i] == o.lab[j]), tag + " isSameSet(" + to_string(i) + "," + to_string(j) + ")");
    }
}

int main() {
    // Deterministic edge cases
    {
        UnionFind z(0);
        CHECK(z.numDisjointSets() == 0, "N=0");
        UnionFind one(1);
        one.unionSet(0, 0);
        CHECK(one.numDisjointSets() == 1 && one.sizeOfSet(0) == 1 && one.isSameSet(0, 0), "N=1 self union");
        UnionFind uf(5);
        CHECK(uf.numDisjointSets() == 5 && !uf.isSameSet(0, 1), "fresh");
        uf.unionSet(0, 1); uf.unionSet(1, 0); uf.unionSet(0, 1); // repeats
        CHECK(uf.numDisjointSets() == 4 && uf.sizeOfSet(0) == 2 && uf.sizeOfSet(1) == 2, "repeat union");
        uf.unionSet(2, 2);
        CHECK(uf.numDisjointSets() == 4 && uf.sizeOfSet(2) == 1, "self union");
        uf.unionSet(3, 4); uf.unionSet(1, 4);
        CHECK(uf.numDisjointSets() == 2 && uf.sizeOfSet(4) == 4 && uf.isSameSet(0, 3) && !uf.isSameSet(0, 2), "merge sets");
    }

    // Random small N, full comparison after every union
    for (int it = 0; it < 500; it++) {
        stress::iter = it;
        int N = (int)stress::rnd(1, 16);
        UnionFind uf(N);
        TOracle o(N);
        vpii ops;
        int steps = (int)stress::rnd(0, 3 * N);
        for (int k = 0; k < steps; k++) {
            int i = (int)stress::rnd(0, N - 1), j = (int)stress::rnd(0, N - 1); // includes i == j and repeats
            ops.push_back({i, j});
            uf.unionSet(i, j); o.unite(i, j);
            t_check_all(uf, o, t_ops(N, ops));
        }
    }

    // Random large N, sampled queries
    for (int it = 0; it < 20; it++) {
        stress::iter = it;
        int N = (int)stress::rnd(500, 3000);
        UnionFind uf(N);
        TOracle o(N);
        int steps = (int)stress::rnd(N / 2, 2 * N);
        for (int k = 0; k < steps; k++) {
            int i = (int)stress::rnd(0, N - 1), j = (int)stress::rnd(0, N - 1);
            uf.unionSet(i, j); o.unite(i, j);
        }
        string tag = "large N=" + to_string(N) + " it=" + to_string(it);
        CHECK(uf.numDisjointSets() == o.sets, tag);
        for (int q = 0; q < 2000; q++) {
            int i = (int)stress::rnd(0, N - 1), j = (int)stress::rnd(0, N - 1);
            CHECK(uf.isSameSet(i, j) == (o.lab[i] == o.lab[j]), tag + " isSameSet");
            CHECK(uf.sizeOfSet(i) == o.size(i), tag + " sizeOfSet");
        }
    }
    puts("OK");
}
