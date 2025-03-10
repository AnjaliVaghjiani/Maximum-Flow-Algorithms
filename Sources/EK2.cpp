#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <omp.h>
#include <fstream>

using namespace std;

// Sequential BFS function to find the shortest augmenting path using adjacency matrix
bool bfsSequential(vector<vector<int>>& rGraph, int s, int t, vector<int>& parent) {
    int V = rGraph.size();
    vector<bool> visited(V, false);
    queue<int> q;

    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        // Explore all vertices adjacent to u
        for (int v = 0; v < V; ++v) {
            if (!visited[v] && rGraph[u][v] > 0) {
                parent[v] = u;
                visited[v] = true;

                if (v == t) {
                    return true;  // Found the sink
                }

                q.push(v);
            }
        }
    }

    return false;
}

// Parallel BFS function to find the shortest augmenting path using adjacency matrix
bool bfsParallel(vector<vector<int>>& rGraph, int s, int t, vector<int>& parent) {
    int V = rGraph.size();
    vector<bool> visited(V, false);
    queue<int> q;

    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    bool found_path = false;
    int local_found = 0;

    while (!q.empty() && !found_path) {
        int u = q.front();
        q.pop();

        #pragma omp parallel for shared(found_path) firstprivate(local_found)
        for (int v = 0; v < V; ++v) {
            if (!found_path && !visited[v] && rGraph[u][v] > 0) {
                parent[v] = u;

                if (v == t) {
                    local_found = 1;
                    #pragma omp atomic write
                    found_path = true;
                } else {
                    #pragma omp critical
                    {
                        q.push(v);
                        visited[v] = true;
                    }
                }
            }
        }

        if (local_found) {
            #pragma omp flush(found_path)
            break;
        }
    }

    return found_path;
}

// Edmonds-Karp algorithm using adjacency matrix (sequential)
int edmondsKarpSequential(vector<vector<int>>& graph, int s, int t) {
    int V = graph.size();

    // Create the residual graph as an adjacency matrix
    vector<vector<int>> rGraph = graph;

    vector<int> parent(V);
    int max_flow = 0;

    // Augment the flow while there is a path from source to sink
    while (bfsSequential(rGraph, s, t, parent)) {
        int path_flow = INT_MAX;

        // Find the maximum flow through the path found by BFS
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            path_flow = min(path_flow, rGraph[u][v]);
        }

        // Update residual capacities of the edges and reverse edges along the path
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            rGraph[u][v] -= path_flow;
            rGraph[v][u] += path_flow;
        }

        max_flow += path_flow;
    }

    return max_flow;
}

// Edmonds-Karp algorithm using adjacency matrix (parallel)
int edmondsKarpParallel(vector<vector<int>>& graph, int s, int t) {
    int V = graph.size();

    // Create the residual graph as an adjacency matrix
    vector<vector<int>> rGraph = graph;

    vector<int> parent(V);
    int max_flow = 0;

    // Augment the flow while there is a path from source to sink
    while (bfsParallel(rGraph, s, t, parent)) {
        int path_flow = INT_MAX;

        // Find the maximum flow through the path found by BFS
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            path_flow = min(path_flow, rGraph[u][v]);
        }

        // Update residual capacities of the edges and reverse edges along the path
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

    int u, v, capacity;
    while (infile >> u >> v >> capacity) {
        graph[u][v] = capacity;
    }

    infile.clear(); // Clear the EOF flag
    infile >> u >> v; // Reading the source and sink values

    int s = u; // source
    int t = v; // sink
    infile.close();

    cout << "Source (s) and Sink (t): " << s << " " << t << endl;

    double start_time, end_time;

    // Sequential version timing
    start_time = omp_get_wtime();
    int max_flow_sequential = edmondsKarpSequential(graph, s, t);
    end_time = omp_get_wtime();
    double total_time_sequential = end_time - start_time;
    cout << "Sequential: The maximum possible flow is " << max_flow_sequential << endl;
    cout << "Time taken (Sequential): " << total_time_sequential << " seconds" << endl;

    // Parallel version timing
    omp_set_num_threads(desired_threads);  // Set the number of threads

    start_time = omp_get_wtime();
    int max_flow_parallel = edmondsKarpParallel(graph, s, t);
    end_time = omp_get_wtime();
    double total_time_parallel = end_time - start_time;

    cout << "Parallel: The maximum possible flow is " << max_flow_parallel << endl;
    cout << "Time taken (Parallel): " << total_time_parallel << " seconds" << endl;

    return 0;
}
