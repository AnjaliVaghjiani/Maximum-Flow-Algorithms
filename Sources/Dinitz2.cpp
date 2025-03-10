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
#include <cfloat>  // For DBL_MAX

using namespace std;
using namespace std::chrono;

size_t estimateMemoryUsage(const vector<vector<double>>& graph) {
    size_t totalMemory = 0;

    // Memory for the outer vector structure
    totalMemory += sizeof(graph);

    for (const auto& row : graph) {
        // Memory for each inner vector structure
        totalMemory += sizeof(row);

        // Memory for the elements in each inner vector
        totalMemory += row.size() * sizeof(double);
    }

    return totalMemory;
}

// Function to perform BFS and construct level graph
bool bfs(vector<vector<double>>& rGraph, int s, int t, vector<int>& level) {
    int V = rGraph.size();
    fill(level.begin(), level.end(), -1);
    level[s] = 0;

    queue<int> q;
    q.push(s);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < V; ++v) {
            if (level[v] < 0 && rGraph[u][v] > 0) {
                level[v] = level[u] + 1;
                q.push(v);
            }
        }
    }

    return level[t] >= 0;
}

// Parallelized BFS
bool bfsParallel(vector<vector<double>>& rGraph, int s, int t, vector<int>& level) {
    int V = rGraph.size();
    fill(level.begin(), level.end(), -1);
    level[s] = 0;

    queue<int> q;
    q.push(s);

    bool found = false;

    while (!q.empty() && !found) {
        int u = q.front();
        q.pop();

        #pragma omp parallel for shared(level, rGraph, q, found) schedule(dynamic)
        for (int v = 0; v < V; ++v) {
            if (!found && level[v] < 0 && rGraph[u][v] > 0) {
                #pragma omp critical
                {
                    if (level[v] < 0) { // Check again in case another thread modified it
                        level[v] = level[u] + 1;
                        if (v == t) {
                            found = true;
                        }
                        q.push(v);
                    }
                }
            }
        }
    }

    return level[t] >= 0;
}

// Function to perform DFS and find the blocking flow
double dfs(vector<vector<double>>& rGraph, vector<int>& level, vector<int>& start, int u, int t, double flow) {
    if (u == t) return flow;

    for (int& v = start[u]; v < rGraph.size(); ++v) {
        if (level[v] == level[u] + 1 && rGraph[u][v] > 0) {
            double curr_flow = min(flow, rGraph[u][v]);
            double temp_flow = dfs(rGraph, level, start, v, t, curr_flow);

            if (temp_flow > 0) {
                rGraph[u][v] -= temp_flow;
                rGraph[v][u] += temp_flow;
                return temp_flow;
            }
        }
    }

    return 0;
}

// Parallelized DFS
double dfsParallel(vector<vector<double>>& rGraph, vector<int>& level, vector<int>& start, int u, int t, double flow) {
    if (u == t) return flow;

    double total_flow = 0;
    int V = rGraph.size();

    #pragma omp parallel for shared(total_flow) schedule(dynamic)
    for (int v = start[u]; v < V; ++v) {
        if (level[v] == level[u] + 1 && rGraph[u][v] > 0) {
            double curr_flow = min(flow, rGraph[u][v]);
            double temp_flow = dfs(rGraph, level, start, v, t, curr_flow);

            #pragma omp critical
            {
                if (temp_flow > 0) {
                    rGraph[u][v] -= temp_flow;
                    rGraph[v][u] += temp_flow;
                    total_flow += temp_flow;
                }
            }
        }
    }

    return total_flow;
}

// Dinic's algorithm for computing maximum flow
double dinic(vector<vector<double>>& graph, int s, int t) {
    int V = graph.size();
    vector<vector<double>> rGraph = graph;

    vector<int> level(V);
    vector<int> start(V);
    double max_flow = 0;

    while (bfs(rGraph, s, t, level)) {
        fill(start.begin(), start.end(), 0);

        while (double flow = dfs(rGraph, level, start, s, t, DBL_MAX)) {
            max_flow += flow;
        }
    }

    return max_flow;
}

// Dinic's algorithm for computing maximum flow
double dinicParallel(vector<vector<double>>& graph, int s, int t) {
    int V = graph.size();
    vector<vector<double>> rGraph = graph;

    vector<int> level(V);
    vector<int> start(V);
    double max_flow = 0;

    while (bfsParallel(rGraph, s, t, level)) {
        fill(start.begin(), start.end(), 0);

        while (double flow = dfs(rGraph, level, start, s, t, DBL_MAX)) {
            max_flow += flow;
        }
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
    // cout << "debus the value of V is " << V << endl;
    vector<vector<double>> graph(V, vector<double>(V, 0));
    vector<vector<double>> graph1(V, vector<double>(V, 0));


    int u, v;
    double capacity;
    while (infile >> u >> v >> capacity) {
        // cout << "u is "<< u << "and v is "<< v << endl;
        graph[u][v] = capacity;
        graph1[u][v] = capacity;
    }

    infile.clear(); // Clear the EOF flag
    infile >> u >> v; // Reading the source and sink values

    int s = u; // source
    int t = v; // sink

    infile.close();
     size_t memoryUsage = estimateMemoryUsage(graph);
    cout << "Estimated memory usage of the graph: " << memoryUsage << " bytes" << endl;

    cout << "The value for s and t are " << s << " " << t << endl;

    double start, end;

    // Sequential version timing
    start = omp_get_wtime();
    double max_flow_sequential = dinic(graph, s, t);
    end = omp_get_wtime();
    double total_time_sequential = end - start;
    cout << "Sequential: The maximum possible flow is " << max_flow_sequential << endl;
    cout << "Time taken (Sequential): " << total_time_sequential << " seconds" << endl;

    
    // Parallel version timing
    omp_set_num_threads(desired_threads);

    start = omp_get_wtime();
    double max_flow_parallel = dinicParallel(graph1, s, t); // Same function, but ensure parallel code is effective
    cout << "Parallel: The maximum possible flow is " << max_flow_parallel << endl;
 
    end = omp_get_wtime();
    double total_time_parallel = end - start;
    cout << "Time taken (Parallel): " << total_time_parallel << " seconds" << endl;

    return 0;
}
