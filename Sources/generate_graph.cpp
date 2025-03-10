#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <chrono>  // For timing

using namespace std;
using namespace std::chrono;

void generateGraphFile(const string& filename, int numVertices, int edgeProbability, int maxCapacity) {
    ofstream outfile(filename);
    if (!outfile) {
        cerr << "Error opening file for writing." << endl;
        return;
    }

    // Write the number of vertices
    outfile << numVertices << endl;

    srand(time(nullptr)); // Seed for randomness

    // Generate and write the edges
    for (int i = 0; i < numVertices; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            if (i != j && (rand() % 100) < edgeProbability) { // Randomly decide if an edge exists
                int capacity = rand() % maxCapacity + 1; // Random capacity between 1 and maxCapacity
                outfile << i << " " << j << " " << capacity << endl;
            }
        }
    }

    // Define source and sink (for simplicity, source = 0 and sink = numVertices-1)
    int s = 0;
    int t = numVertices - 1;
    outfile << s << " " << t << endl;

    outfile.close();
}

int main(int argc, char *argv[]) {
    // Default filename
    string filename = "graph.txt";

    if (argc >= 2) {
        filename = argv[1];
    }
    int numVertices; // Number of nodes
    int edgeProbability; // Probability factor for edge creation (in percentage)
    int maxCapacity;  // Maximum capacity for an edge

    cout << "Enter the number of vertices: ";
    cin >> numVertices;
    cout << "Enter the edge probability (in %): ";
    cin >> edgeProbability;
    cout << "Enter the maximum capacity for an edge: ";
    cin >> maxCapacity;

    auto start = high_resolution_clock::now();
    generateGraphFile(filename, numVertices, edgeProbability, maxCapacity);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Graph generated and saved to " << filename << endl;
    cout << "Time taken to generate the graph: " << duration.count() << " milliseconds" << endl;

    return 0;
}
