#ifndef MAZE_H
#define MAZE_H

// Max maze dimension (N x N)
const int MAX_SIZE = 50;

// Simple position in the grid
struct Position {
    int x;
    int y;
};

// Wall indices
enum { TOP = 0, RIGHT = 1, BOTTOM = 2, LEFT = 3 };

// A single maze cell
struct Cell {
    int visited;   // used during generation/solving
    int walls[4];  // 1 = wall present, 0 = open
    int inPath;    // marked for BFS shortest path
};

// The maze itself
struct Maze {
    int size;                        // N
    Cell cells[MAX_SIZE][MAX_SIZE];
    Position entry;                  // random entry on border
    Position exit;                   // random exit on border
};

// Initialization: all walls intact, flags cleared
void initMaze(Maze* m, int n);

// Choose random entry & exit on outer boundary
void chooseRandomEntryExit(Maze* m);

// Maze generation using BFS (spanning tree)
void generateMaze(Maze* m);

// ASCII printing; shows '*' for cells on shortest path
void printMaze(const Maze* m);

// BFS solver: shortest path from entry to exit
// Marks cells along the shortest path with inPath = 1
// Returns true if path exists
bool solveMazeBFS(Maze* m);

#endif