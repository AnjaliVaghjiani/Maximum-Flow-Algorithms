SDP-Project_2024

-----------------------------------------------------------------------------------------------------------

Maximum Flow Algorithm Project

-----------------------------------------------------------------------------------------------------------

Project Description

This project implements three key algorithms to solve the Maximum Flow Problem in a flow network:
- Edmonds-Karp Algorithm
- Dinic's Algorithm
- Push-Relabel Algorithm
Each algorithm is implemented in both sequential and parallel versions, with the parallel versions optimized using OpenMP.
Additionally, a graph generation code is included, allowing you to create test graphs easily if you just want to test the algorithms without providing your own input.

-----------------------------------------------------------------------------------------------------------

Implementation Details:

- Programming Language: 
    The project is implemented in C++.
- Operating Systems compatibility:
    Windows
    Ubuntu
    iOS
- Parallelization: 
    Parallel versions leverage OpenMP for enhanced performance on multi-core systems.
- Further Details: 
    Detailed descriptions of each algorithm, including their implementation specifics and theoretical background, can be found in the accompanying documentation file `DOCUMENTATION.md`. This file provides in-depth explanations of the algorithms, their parallelization strategies, and any additional considerations related to their implementation.

-----------------------------------------------------------------------------------------------------------

Features

- Multiple Maximum Flow Algorithms:
  - Implements three different algorithms to solve the Maximum Flow Problem: Edmonds-Karp, Dinitz's, and Push-Relabel.

- Sequential and Parallel Implementations:
  - Provides both sequential and parallel implementations for each algorithm.

- Performance Measurement:
  - Measures execution time for both the sequential and parallel implementations of each algorithm.

- Graph Generation Utility:
  - Includes a tool for generating test graphs to facilitate algorithm testing.

- Supported Graph File Format:
  - The input graph should be provided in a `.txt` file with the following structure:
    - First Line: Number of vertices (n) and number of edges (m).
    - Next m Lines: Each line represents an edge with three values: the starting vertex, the ending vertex, and the capacity of the edge.
    - Last Line: Contains two values representing the source and sink vertices.
  - An example input file is provided in the project folder under `graph.txt`.

-----------------------------------------------------------------------------------------------------------

Compilation and Execution

1. Ensure you have a C++ compiler installed:
   - Make sure you have a C++ compiler like `g++` installed with OpenMP support.

2. Compile the code using the following commands:

   For Windows and Ubuntu
   - To compile the Edmonds-Karp algorithm:
     g++ -fopenmp -o Edmonds_Karp.exe Edmonds_Karp.cpp

   - To compile Dinitz's algorithm:
     g++ -fopenmp -o Dinitz.exe Dinitz.cpp

   - To compile the Push-Relabel algorithm:
     g++ -fopenmp -o Push_Relabel.exe Push_Relabel.cpp

   - To compile the graph generation utility:
     g++ -o generate_graph.exe generate_graph.cpp

   The iOS version requires clang++ instead of g++

3. Run the programs:

   - To run the Edmonds-Karp algorithm:
     ./Edmonds_Karp.exe <graph_file.txt> [number_of_threads]

   - To run Dinitz's algorithm:
     ./Dinitz.exe <graph_file.txt> [number_of_threads]

   - To run the Push-Relabel algorithm:
     ./Push_Relabel.exe <graph_file.txt> [number_of_threads]

   - To run the graph generation utility:
     ./generate_graph.exe [output_file.txt]

     - Note: Replace `[output_file.txt]` with the desired name for the generated graph file. If omitted, the default filename `graph.txt` will be used.

     - Interactive Prompts: After execution, you will be prompted to enter the following details via standard input:
       - Number of vertices: The total number of vertices in the graph.
       - Edge probability: The probability (in percentage) that an edge exists between any pair of vertices.
       - Maximum capacity: The maximum capacity for any edge in the graph.

   - Note: For all algorithms, replace `<graph_file.txt>` with the name of your graph file. The `[number_of_threads]` argument is optional and specifies the number of threads to be used in parallel execution. If omitted, the program defaults to 8 threads.

-----------------------------------------------------------------------------------------------------------

Code Structure

- Edmonds_Karp.cpp: Main code file implementing the Edmond-Karp algorithm.
- EK2.cpp: Main code file implementing a parallelized version of the Edmond-Karp algorithm.
- Dinitz.cpp: Main code file implementing the Dinic's algorithm.
- Dinitz2.cpp: Main code file implementing a parallelized version of the Dinic's algorithm.
- Push_Relabel.cpp: Main code file implementing the Push-Relabel algorithm.
- generate_graph.cpp: Utility for generating a random graph and saving it to a file (default: `graph.txt`).

