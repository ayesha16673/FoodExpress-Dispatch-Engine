

#include <iostream>
#include <string>
using namespace std;

const int MAX_NODES = 8;
const int INF = 9999;

// ==================== GRAPH MODULE ====================
// constructor initialis the garph
class Graph {
    int mat[MAX_NODES][MAX_NODES];
    bool block[MAX_NODES][MAX_NODES];
    string name[MAX_NODES];

public:
    Graph() {
        for (int i = 0; i < MAX_NODES; i++) {
            name[i] = "Node" + to_string(i);
            for (int j = 0; j < MAX_NODES; j++) {
                if (i == j) mat[i][j] = 0;
                else mat[i][j] = INF;
                block[i][j] = false; // mark all roads open
            }
        }
    }

    void setName(int i, string n) { name[i] = n; }

    // Road add karo (bidirectional)
    void road(int u, int v, int w) {
        mat[u][v] = w;
        mat[v][u] = w;
    }

    // FOR  Road block 

    void blockRoad(int u, int v) {
        block[u][v] = true;
        block[v][u] = true;
        cout << "Road between " << name[u] << " and " << name[v] << " blocked!" << endl;
    }

    // FOR ROAD OPEN
    void openRoad(int u, int v) {
        block[u][v] = false;
        block[v][u] = false;
        cout << "Road between " << name[u] << " and " << name[v] << " opened!" << endl;
    }
    // USE FOR BOOLEAN 
    bool open(int u, int v) {
        if (block[u][v]) return false;
        if (mat[u][v] == INF) return false;
        return true;
    }

    // ==================== DIJKSTRA ====================
    void dijkstra(int src, int dest) {
        int dist[MAX_NODES], parent[MAX_NODES];
        bool vis[MAX_NODES];

        for (int i = 0; i < MAX_NODES; i++) {
            dist[i] = INF;
            parent[i] = -1;
            vis[i] = false;
        }
        dist[src] = 0;

        for (int k = 0; k < MAX_NODES; k++) {
            int u = -1;
            for (int i = 0; i < MAX_NODES; i++) {
                if (!vis[i] && (u == -1 || dist[i] < dist[u])) u = i;
            }
            if (u == -1 || dist[u] == INF) break;
            vis[u] = true;

            for (int v = 0; v < MAX_NODES; v++) {
                if (!open(u, v)) continue;
                if (dist[u] + mat[u][v] < dist[v]) {
                    dist[v] = dist[u] + mat[u][v];
                    parent[v] = u;
                }
            }
        }

        // Check nodes distances
        if (dest == -1) {
            cout << "\nDistances from " << name[src] << ":" << endl;
            for (int i = 0; i < MAX_NODES; i++) {
                if (i == src) continue;
                cout << name[src] << " -> " << name[i] << " : ";
                if (dist[i] == INF) cout << "No path";
                else cout << dist[i] << " km";  // maesure in km
                cout << endl;
            }
            return;
        }

        if (dist[dest] == INF) {
            cout << "No path available!" << endl;
            return;
        }

        // for  Path reconstruct 
        int path[20], len = 0, cur = dest;
        while (cur != -1) {
            path[len] = cur;
            len++;
            cur = parent[cur];
        }

        cout << "\nShortest Route: " << name[src] << " to " << name[dest];
        cout << " = " << dist[dest] << " km" << endl;
        cout << "Path: ";
        for (int i = len - 1; i >= 0; i--) {
            cout << name[path[i]];
            if (i > 0) cout << " -> ";
        }
        cout << endl;
        // calculate the delivery paymnet
        cout << "Delivery Cost: Rs." << dist[dest] * 10 << endl;
    }

    // ==================== COMPARE ROUTES ====================

    void compareRoute(int s, int d) {
        cout << "\n--- Main Route ---" << endl;
        dijkstra(s, d);
        if (mat[s][d] != INF) {
            int save = mat[s][d];
            mat[s][d] = INF;
            mat[d][s] = INF;
            cout << "\n--- Alternate Route ---" << endl;
            dijkstra(s, d);
            mat[s][d] = save;
            mat[d][s] = save;
        }
    }

    // ==================== BFS ====================
    void bfs(int src) {
        bool vis[MAX_NODES];
        int q[20], f = 0, r = 0;
        for (int i = 0; i < MAX_NODES; i++) vis[i] = false;

        vis[src] = true;
        q[r] = src;
        r++;

        cout << "\nBFS Traversal from " << name[src] << ": ";
        while (f < r) {
            int u = q[f];
            f++;
            cout << name[u] << " ";
            for (int v = 0; v < MAX_NODES; v++) {
                if (!vis[v] && open(u, v)) {
                    vis[v] = true;
                    q[r] = v;
                    r++;
                }
            }
        }
        cout << endl;
    }

    // ==================== DFS ====================
    void dfsRec(int u, bool vis[]) {
        vis[u] = true;
        cout << name[u] << " ";
        for (int v = 0; v < MAX_NODES; v++) {
            if (!vis[v] && open(u, v))
                dfsRec(v, vis);
        }
    }

    void dfs(int src) {
        bool vis[MAX_NODES];
        for (int i = 0; i < MAX_NODES; i++) vis[i] = false;
        cout << "\nDFS Traversal from " << name[src] << ": ";
        dfsRec(src, vis);
        cout << endl;
    }

    // ==================== REROUTE ====================
    // 
    // create new route after old route block

    void reroute(int s, int d, int bu, int bv) {
        cout << "\n--- Reroute Demo ---" << endl;
        cout << "Original route:" << endl;
        dijkstra(s, d);

        blockRoad(bu, bv);
        cout << "\nAfter road block, new route:" << endl;
        dijkstra(s, d);

        openRoad(bu, bv);
        cout << "Road restored." << endl;
    }
};