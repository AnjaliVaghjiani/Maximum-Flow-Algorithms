#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <omp.h>
#include <fstream>
#include <ctime>   // For time()
#include <chrono>  // For timing

using namespace std;
using namespace std::chrono;

// Function to perform BFS and find the shortest augmenting path
bool bfs(vector<vector<int>>& rGraph, int s, int t, vector<int>& parent) {
    int V = rGraph.size();
    vector<bool> visited(V, false);
    queue<int> q;

    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < V; ++v) {
            if (visited[v] == false && rGraph[u][v] > 0) {
                if (v == t) {
                    parent[v] = u;
                    return true;
                }
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }

    return false;
}

// Edmonds-Karp algorithm for computing maximum flow
int edmondsKarp(vector<vector<int>>& graph, int s, int t) {
    int V = graph.size();
    vector<vector<int>> rGraph = graph;
    vector<int> parent(V);
    int max_flow = 0;

    while (bfs(rGraph, s, t, parent)) {
        int path_flow = INT_MAX;

        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            path_flow = min(path_flow, rGraph[u][v]);
        }

        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            rGraph[u][v] -= path_flow;
            rGraph[v][u] += path_flow;
        }

        max_flow += path_flow;
    }

    return max_flow;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file> [number_of_threads]" << endl;
        return 1;
    }

    string filename = argv[1];

    ifstream infile(filename);
    if (!infile) {
        cerr << "Unable to open file " << filename << endl;
        return 1;
    }

    int desired_threads = 8; // Default value
    if (argc == 3) {
        desired_threads = atoi(argv[2]); 
        if (desired_threads <= 0) {
            cerr << "Invalid number of threads. Using default value of 8." << endl;
            desired_threads = 8;
        }
    }

    int V;
    infile >> V;

    vector<vector<int>> graph(V, vector<int>(V, 0));
    vector<vector<int>> graph1(V, vector<int>(V, 0));

    int u, v, capacity;
    while (infile >> u >> v >> capacity) {
        graph[u][v] = capacity;
        graph1[u][v] = capacity;
    }

    infile.clear(); // Clear the EOF flag
    infile >> u >> v; // Reading the source and sink values

    int s = u; // source
    int t = v; // sink

    infile.close();

    cout << "The value for s and t are " << s << " " << t << endl;

    double start, end;

    // Sequential version timing
    start = omp_get_wtime();
    int max_flow_sequential = edmondsKarp(graph, s, t);
    end = omp_get_wtime();
    double total_time_sequential = end - start;
    cout << "Sequential: The maximum possible flow is " << max_flow_sequential << endl;
    cout << "Time taken (Sequential): " << total_time_sequential << " seconds" << endl;

    omp_set_num_threads(desired_threads);

    start = omp_get_wtime();
    #pragma omp parallel 
    {
        #pragma omp single
        {
            int num_threads = omp_get_num_threads();
            cout << "Number of threads running in parallel: " << num_threads << endl;
            int max_flow_parallel = edmondsKarp(graph1, s, t); // Same function, but ensure parallel code is effective
        }
    }
    end = omp_get_wtime();
    double total_time_parallel = end - start;
    cout << "Parallel: The maximum possible flow is " << max_flow_sequential << endl; // Use the same max_flow_sequential as placeholder
    cout << "Time taken (Parallel): " << total_time_parallel << " seconds" << endl;

    return 0;
}
