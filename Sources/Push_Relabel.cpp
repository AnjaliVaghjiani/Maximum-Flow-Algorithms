#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <climits> // For INT_MAX
#include <omp.h>
#include <atomic>

using namespace std;

class PushRelabel {
public:
    PushRelabel(int n) : 
    n(n), 
    capacity(n, vector<int>(n, 0)), 
    flow(n, vector<int>(n, 0)), 
    height(n, 0), 
    excess(n, 0) 
    {}

    void addEdge(int u, int v, int cap) {
        capacity[u][v] = cap;
    }

    int getMaxFlow(int source, int sink) {
        height[source] = n;
        excess[source] = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (capacity[source][i] > 0) {
                push(source, i);
            }
        }

        while (true) {
            int u = -1;
            for (int i = 0; i < n; i++) {
                if (i != source && i != sink && excess[i] > 0) {
                    u = i;
                    break;
                }
            }

            if (u == -1) break;

            bool pushed = false;
            for (int v = 0; v < n && excess[u] > 0; v++) {
                if (capacity[u][v] - flow[u][v] > 0 && height[u] == height[v] + 1) {
                    push(u, v);
                    pushed = true;
                }
            }

            if (!pushed) {
                relabel(u);
            }
        }

        int maxFlow = 0;
        for (int i = 0; i < n; i++) {
            maxFlow += flow[source][i];
        }

        return maxFlow;
    }

int getMaxFlowParallel(int source, int sink) {
    height[source] = n;
    excess[source] = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (capacity[source][i] > 0) {
            push(source, i);
        }
    }

    bool active = true;
    while (active) {
        active = false;
        std::vector<bool> local_active(n, false);

        #pragma omp parallel
        {
            bool local_found_active_node = false;

            #pragma omp for nowait
            for (int u = 0; u < n; u++) {
                if (u != source && u != sink && excess[u] > 0) {
                    local_found_active_node = true;
                    bool pushed = false;
                    for (int v = 0; v < n && excess[u] > 0; v++) {
                        if (capacity[u][v] - flow[u][v] > 0 && height[u] == height[v] + 1) {
                            #pragma omp critical
                            {
                                push(u, v); // Ensure push is thread-safe
                            }
                            pushed = true;
                        }
                    }
                    if (!pushed) {
                        #pragma omp critical
                        {
                            relabel(u); // Ensure relabel is thread-safe
                        }
                        if (excess[u] > 0) {
                            local_active[u] = true;
                        }
                    }
                }
            }

            // Update global active status
            #pragma omp atomic write
            active = active || local_found_active_node;
        }

        // Update global active status based on local_active
        #pragma omp parallel for
        for (int u = 0; u < n; u++) {
            if (local_active[u]) {
                #pragma omp atomic write
                active = true;
            }
        }
    }

    // Compute maxFlow
    int maxFlow = 0;
    for (int i = 0; i < n; i++) {
        maxFlow += flow[source][i];
    }

    return maxFlow;
}
private:
    int n;
    vector<vector<int>> capacity;
    vector<vector<int>> flow;
    vector<int> height;
    vector<int> excess;

    void push(int u, int v) {
        int delta = min(excess[u], capacity[u][v] - flow[u][v]);
        flow[u][v] += delta;
        flow[v][u] -= delta;
        excess[u] -= delta;
        excess[v] += delta;
    }

    void relabel(int u) {
        int minHeight = INT_MAX;
        for (int v = 0; v < n; v++) {
            if (capacity[u][v] - flow[u][v] > 0) {
                minHeight = min(minHeight, height[v]);
            }
        }
        if (minHeight < INT_MAX) {
            height[u] = minHeight + 1;
        }
    }
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file> [number_of_threads]" << endl;
        return 1;
    }

    string filename = argv[1];

    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Unable to open file " << filename << endl;
        return 1;
    }

    int numThreads = 8; // Default value
    if (argc == 3) {
        numThreads = atoi(argv[2]); 
        if (numThreads <= 0) {
            cerr << "Invalid number of threads. Using default value of 8." << endl;
            numThreads = 8;
        }
    }

    int n;
    inputFile >> n;
    
    PushRelabel graph(n);
    PushRelabel graph1(n);
    int u, v, cap;
    while (inputFile >> u >> v >> cap) {
        graph.addEdge(u, v, cap);
        graph1.addEdge(u, v, cap);
    }

    // Read source and sink
    int source=0, sink=0;
    
    inputFile.clear();  // Clear the EOF flag to allow reading again
    inputFile >> u >> v;
    source = u;
    sink = v;
    cout << "Source and sink are " <<source << " "<< sink << endl;
    inputFile.close();

    double start = omp_get_wtime();
    int maxFlow = graph.getMaxFlow(source, sink);
    double end = omp_get_wtime();
    double duration = end - start;
    cout << "Sequential Max Flow: " << maxFlow << endl;
    cout << "Sequential Time Taken: " << duration << " seconds" << endl;


    omp_set_num_threads(numThreads);
    
    double start1 = omp_get_wtime();
    int maxFlow1 = graph1.getMaxFlowParallel(source, sink);
    double end1 = omp_get_wtime();
    double duration1 = end1 - start1;

    cout << "Parallel Max Flow: " << maxFlow1 << endl;
    cout << "Parallel Time Taken: " << duration1 << " seconds" << endl;
    cout << "Number of threads: " << numThreads << endl;
    
    return 0;
}