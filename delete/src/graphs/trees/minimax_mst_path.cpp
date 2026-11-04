/**
 * Usage: solve(V, E) reads E edges (u v w) with scanf and returns the MST (Kruskal); dfs(u, v, -1) fills path with the MST path; maximum edge weight on the path = minimax answer
 * Complexity: O(E log E) per MST + O(V) per query (suitable for one or a few queries)
 * Source: UnionFind from Competitive Programming 4 (Halim), same logic as data_structures/union_find.cpp (only the indentation differs)
 * Status: untested
 */
typedef tuple<int, int, int> iii;

// KRUSKAL
class UnionFind
{
private:
  vi p, rank, setSize;
  // OOP style
  // vi p is the key part
  int numSets;

public:
  UnionFind(int N)
  {
    p.assign(N, 0);
    for (int i = 0; i < N; ++i)
      p[i] = i;
    rank.assign(N, 0);
    // optional speedup
    setSize.assign(N, 1);
    numSets = N;
  }
  // optional feature
  // optional feature
  int findSet(int i) { return (p[i] == i) ? i : (p[i] = findSet(p[i])); }
  bool isSameSet(int i, int j) { return findSet(i) == findSet(j); }
  int numDisjointSets() { return numSets; }
  // optional
  int sizeOfSet(int i) { return setSize[findSet(i)]; } // optional
  void unionSet(int i, int j)
  {
    if (isSameSet(i, j))
      return;
    int x = findSet(i), y = findSet(j);
    if (rank[x] > rank[y])
      swap(x, y);
    p[x] = y;
    if (rank[x] == rank[y])
      ++rank[y];
    setSize[y] += setSize[x];
    --numSets;
  }
};
vector<iii> solve(int V, int E)
{

  vector<iii> res;
  vector<iii> EL(E);
  for (int i = 0; i < E; ++i)
  {
    int u, v, w;
    scanf("%d %d %d", &u, &v, &w); // read as (u, v, w)
    --u, --v;
    EL[i] = {w, u, v};
    // reorder as (w, u, v)
  }
  sort(EL.begin(), EL.end());
  // sort by w, O(E log E)
  // note: std::tuple has built-in comparison function
  int mst_cost = 0, num_taken = 0;
  UnionFind UF(V);
  // no edge has been taken
  // all V are disjoint sets
  // note: the runtime cost of UFDS is very light
  for (auto &[w, u, v] : EL)
  {
    // C++17 style
    if (UF.isSameSet(u, v))
      continue;
    mst_cost += w;
    UF.unionSet(u, v);
    res.push_back({w, u, v});
    ++num_taken;
    if (num_taken == V - 1)
      break;
    // already in the same CC
    // add w of this edge
    // link them
    // 1 more edge is taken
    // optimization
  }
  return res;
}
// returns edges of MST

// DFS to find the path(only, including weights) between nodes u and v
vector<vector<pair<int, int>>> grafo;
vector<bool> visited; 
vector<pair<int, int>> path;
bool dfs(int u, int v, int parent_weight) {
  visited[u] = true;
  if (u == v) {
    path.push_back({u, parent_weight});
    return true;
  }
  for (auto &[neighbor, weight]: grafo[u]) {
    if (!visited[neighbor]) {
      if (dfs(neighbor, v, weight)) {
        path.push_back({u, parent_weight});
        return true;
      }
    }
  }
  return false;
}
// returns path
