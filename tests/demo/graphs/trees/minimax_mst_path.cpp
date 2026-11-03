#include "template/template.cpp"
#include "src/graphs/trees/minimax_mst_path.cpp"

int main(){

  int C, L, H;

  while (
  cin >> C >> L >> H
   && C != -1 && L != -1 && H != -1){
    grafo.clear();
    visited.clear();
    path.clear();
    grafo.resize(C, vector<pair<int, int>>());
    visited.resize(C, false);

    vector<iii> res = solve(C, L);
    for (auto &[w, u, v]: res){
      // cout << u+1 << " - " << v+1 << " : " << w << "\n";
      grafo[u].push_back({v, w});
      grafo[v].push_back({u, w});
    }

    for (int i = 0; i < H; i++){
      path.clear();
      visited.clear();
      visited.resize(C, false);

      int u, v; cin >> u >> v;
      --u, --v;
      dfs(u, v, -1);
      reverse(path.begin(), path.end());
      // cout << "path: ";
      // for(auto &[node, w]: path) cout << node << " - " << w << "\n";
      // cout << "===============\n";

      int max_weight = -1;
      for (int i = 1; i < path.size(); ++i) {
        int weight = path[i].second;
        if (weight > max_weight) {
          max_weight = weight;
        }
      }
      cout << max_weight << "\n";
      // cout << "\n";
    }
  }

  return 0;
}