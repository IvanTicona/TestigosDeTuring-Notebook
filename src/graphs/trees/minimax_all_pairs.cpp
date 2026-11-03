/**
 * Usage: vvi cap = minimaxAllPairs(C, graph); cap[s][t] = smallest possible maximum edge weight on a path s -> t (graph: adjacency list of (node, weight), 0-indexed)
 * Complexity: O(C^2 + L) per the original author (actually C Dijkstras: O(C * L log C))
 * Status: untested
 */
typedef vector<vpii> vvpii;

vvi minimaxAllPairs(int C, const vvpii &graph){
    // For each source i we run a "minimax Dijkstra"
    // and fill cap[i][*]
    vvi cap(C, vi(C));
    for (int src = 0; src < C; src++){
      // vis[v] = whether cap[src][v] is already finalized
      vector<char> vis(C, 0);
      // min-heap by maximum edge weight on the path
      priority_queue<pii, vpii, greater<pii>> pq;

      // (current_weight, node)
      pq.emplace(0, src);
      while (!pq.empty()){
        auto [bestW, u] = pq.top();
        pq.pop();
        if (vis[u])
          continue;
        vis[u] = 1;
        cap[src][u] = bestW;
        for (auto &e : graph[u])
        {
          int v = e.first, w = e.second;
          if (!vis[v])
          {
            // the path weight to v is the maximum
            // of the accumulated weight and the edge (u,v)
            int newW = max(bestW, w);
            pq.emplace(newW, v);
          }
        }
      }
    }
    return cap;
}
