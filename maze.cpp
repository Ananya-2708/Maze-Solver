#include "maze.h"
#include "queue.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

// Direction vectors: TOP, RIGHT, BOTTOM, LEFT
static const int dx[4] = { 0,  1,  0, -1 };
static const int dy[4] = { -1, 0,  1,  0 };

static bool inBounds(const Maze* m, int x, int y) {
    return x >= 0 && x < m->size && y >= 0 && y < m->size;
}

static void resetVisited(Maze* m) {
    for (int y = 0; y < m->size; ++y) {
        for (int x = 0; x < m->size; ++x) {
            m->cells[y][x].visited = 0;
        }
    }
}

static void resetPath(Maze* m) {
    for (int y = 0; y < m->size; ++y) {
        for (int x = 0; x < m->size; ++x) {
            m->cells[y][x].inPath = 0;
        }
    }
}

// Fisher-Yates shuffle for small arrays
static void shuffle(int* arr, int n) {
    for (int i = n - 1; i > 0; --i) {
        int j = std::rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void initMaze(Maze* m, int n) {
    if (n <= 1 || n > MAX_SIZE) {
        std::fprintf(stderr, "Maze size must be between 2 and %d\n", MAX_SIZE);
        std::exit(EXIT_FAILURE);
    }

    m->size = n;
    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            m->cells[y][x].visited = 0;
            m->cells[y][x].inPath  = 0;
            for (int w = 0; w < 4; ++w) {
                m->cells[y][x].walls[w] = 1; // all walls present
            }
        }
    }

    // default entry/exit; will be overwritten by chooseRandomEntryExit()
    m->entry = {0, 0};
    m->exit  = {n - 1, n - 1};
}

// Pick random cells on the outer boundary for entry and exit
void chooseRandomEntryExit(Maze* m) {
    int n = m->size;

    auto randomBoundaryCell = [n]() -> Position {
        int side = std::rand() % 4; // 0=TOP,1=RIGHT,2=BOTTOM,3=LEFT
        Position p;
        switch (side) {
            case 0:  // TOP row
                p.y = 0;
                p.x = std::rand() % n;
                break;
            case 1:  // RIGHT column
                p.x = n - 1;
                p.y = std::rand() % n;
                break;
            case 2:  // BOTTOM row
                p.y = n - 1;
                p.x = std::rand() % n;
                break;
            case 3:  // LEFT column
            default:
                p.x = 0;
                p.y = std::rand() % n;
                break;
        }
        return p;
    };

    // ensure entry and exit are distinct
    Position a = randomBoundaryCell();
    Position b = randomBoundaryCell();
    while (a.x == b.x && a.y == b.y) {
        b = randomBoundaryCell();
    }

    m->entry = a;
    m->exit  = b;
}

// BFS-based maze generation: random spanning tree rooted at entry
void generateMaze(Maze* m) {
    int n = m->size;

    // visited for generation
    int genVisited[MAX_SIZE][MAX_SIZE] = {0};

    Queue q;
    initQueue(&q);

    Position root = m->entry;

    genVisited[root.y][root.x] = 1;
    enqueue(&q, root);

    while (!isQueueEmpty(&q)) {
        Position cur = dequeue(&q);
        int x = cur.x;
        int y = cur.y;

        int dirs[4] = { TOP, RIGHT, BOTTOM, LEFT };
        shuffle(dirs, 4);

        // For each neighbor in random order, if unvisited, connect and enqueue
        for (int i = 0; i < 4; ++i) {
            int d  = dirs[i];
            int nx = x + dx[d];
            int ny = y + dy[d];

            if (inBounds(m, nx, ny) && !genVisited[ny][nx]) {
                genVisited[ny][nx] = 1;

                // knock down wall between (x,y) and (nx,ny)
                m->cells[y][x].walls[d] = 0;
                m->cells[ny][nx].walls[(d + 2) % 4] = 0;

                enqueue(&q, Position{nx, ny});
            }
        }
    }

    // open entrance to the outside
    if (m->entry.y == 0) {
        m->cells[m->entry.y][m->entry.x].walls[TOP] = 0;
    } else if (m->entry.y == n - 1) {
        m->cells[m->entry.y][m->entry.x].walls[BOTTOM] = 0;
    } else if (m->entry.x == 0) {
        m->cells[m->entry.y][m->entry.x].walls[LEFT] = 0;
    } else if (m->entry.x == n - 1) {
        m->cells[m->entry.y][m->entry.x].walls[RIGHT] = 0;
    }

    // open exit to the outside
    if (m->exit.y == 0) {
        m->cells[m->exit.y][m->exit.x].walls[TOP] = 0;
    } else if (m->exit.y == n - 1) {
        m->cells[m->exit.y][m->exit.x].walls[BOTTOM] = 0;
    } else if (m->exit.x == 0) {
        m->cells[m->exit.y][m->exit.x].walls[LEFT] = 0;
    } else if (m->exit.x == n - 1) {
        m->cells[m->exit.y][m->exit.x].walls[RIGHT] = 0;
    }

    resetVisited(m);
}

// ASCII printing: shows '*' on cells that are on the shortest path
void printMaze(const Maze* m) {
    int n = m->size;

    // top boundary
    for (int x = 0; x < n; ++x) {
        std::printf("+");
        if (m->cells[0][x].walls[TOP]) std::printf("---");
        else std::printf("   ");
    }
    std::printf("+\n");

    for (int y = 0; y < n; ++y) {
        // left walls + content
        for (int x = 0; x < n; ++x) {
            if (m->cells[y][x].walls[LEFT]) std::printf("|");
            else std::printf(" ");

            if (m->cells[y][x].inPath) std::printf(" * ");
            else std::printf("   ");
        }
        // right wall of last cell
        if (m->cells[y][n - 1].walls[RIGHT]) std::printf("|");
        else std::printf(" ");
        std::printf("\n");

        // bottom walls
        for (int x = 0; x < n; ++x) {
            std::printf("+");
            if (m->cells[y][x].walls[BOTTOM]) std::printf("---");
            else std::printf("   ");
        }
        std::printf("+\n");
    }
}

// BFS shortest-path solver: entry -> exit
bool solveMazeBFS(Maze* m) {
    resetVisited(m);
    resetPath(m);

    int n = m->size;

    int parentX[MAX_SIZE][MAX_SIZE];
    int parentY[MAX_SIZE][MAX_SIZE];

    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            parentX[y][x] = -1;
            parentY[y][x] = -1;
        }
    }

    Queue q;
    initQueue(&q);

    Position start = m->entry;
    Position goal  = m->exit;

    m->cells[start.y][start.x].visited = 1;
    enqueue(&q, start);

    bool found = false;

    while (!isQueueEmpty(&q)) {
        Position cur = dequeue(&q);
        int x = cur.x;
        int y = cur.y;

        if (x == goal.x && y == goal.y) {
            found = true;
            break;
        }

        for (int d = 0; d < 4; ++d) {
            if (m->cells[y][x].walls[d] == 0) {
                int nx = x + dx[d];
                int ny = y + dy[d];

                if (inBounds(m, nx, ny) && !m->cells[ny][nx].visited) {
                    m->cells[ny][nx].visited = 1;
                    parentX[ny][nx] = x;
                    parentY[ny][nx] = y;
                    enqueue(&q, Position{nx, ny});
                }
            }
        }
    }

    if (!found) {
        return false;
    }

    // Reconstruct shortest path and mark inPath
    int x = goal.x;
    int y = goal.y;
    while (!(x == start.x && y == start.y)) {
        m->cells[y][x].inPath = 1;
        int px = parentX[y][x];
        int py = parentY[y][x];
        x = px;
        y = py;
    }
    m->cells[start.y][start.x].inPath = 1;

    return true;
}