#include <iostream>
#include <cstdlib>
#include <ctime>
#include "maze.h"

int main() {
    int n;
    std::cout << "Enter maze size N (2 .. " << MAX_SIZE << "): ";
    if (!(std::cin >> n)) {
        std::cerr << "Invalid input.\n";
        return EXIT_FAILURE;
    }
    if (n < 2 || n > MAX_SIZE) {
        std::cerr << "Maze size must be between 2 and " << MAX_SIZE << ".\n";
        return EXIT_FAILURE;
    }

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Maze m;
    initMaze(&m, n);

    chooseRandomEntryExit(&m);

    std::cout << "Random entry: (" << m.entry.x << "," << m.entry.y << ")\n";
    std::cout << "Random exit : (" << m.exit.x  << "," << m.exit.y  << ")\n";

    std::cout << "\nGenerating maze using BFS spanning tree...\n";
    generateMaze(&m);

    std::cout << "\nGenerated maze:\n";
    printMaze(&m);

    std::cout << "\nSolving with BFS (shortest path entry -> exit)...\n";
    bool bfsFound = solveMazeBFS(&m);
    if (bfsFound) {
        std::cout << "BFS: Shortest path found.\n";
        std::cout << "\nMaze with BFS shortest path marked as '*':\n";
        printMaze(&m);
    } else {
        std::cout << "BFS: No path found.\n";
    }

    return 0;
}