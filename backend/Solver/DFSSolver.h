#include <vector>
#include <cassert>
#include <stdexcept>
#include "../Model/RubiksCube.h"

#ifndef RUBIKS_CUBE_SOLVER_DFSSOLVER_H
#define RUBIKS_CUBE_SOLVER_DFSSOLVER_H

// Typename T: RubiksCube Representation used (3d, 1d, Bitboard)
// Typename H: Corresponding Hash function

template<typename T, typename H>
class DFSSolver {
private:
    vector<RubiksCube::MOVE> moves;
    int max_search_depth;

    // DFS helper; increments nodesExpanded on each call.
    bool dfs(int dep) {
        nodesExpanded++;
        if (nodesExpanded > maxNodes) throw std::runtime_error("Search timeout: expanded too many nodes");
        if (rubiksCube.isSolved()) return true;
        if (dep > max_search_depth) return false;
        for (int i = 0; i < 18; i++) {
            if (!moves.empty()) {
                int prev = (int)moves.back() / 3;
                int curr = i / 3;
                if (prev == curr) continue;
                if (prev % 2 == 1 && curr == prev - 1) continue;
            }
            rubiksCube.move(RubiksCube::MOVE(i));
            moves.push_back(RubiksCube::MOVE(i));
            if (dfs(dep + 1)) return true;
            moves.pop_back();
            rubiksCube.invert(RubiksCube::MOVE(i));
        }
        return false;
    }

public:
    T rubiksCube;
    long long nodesExpanded = 0;
    long long maxNodes = 1000000000;

    DFSSolver(T _rubiksCube, int _max_search_depth = 8) {
        rubiksCube = _rubiksCube;
        max_search_depth = _max_search_depth;
    }

    vector<RubiksCube::MOVE> solve() {
        dfs(1);
        return moves;
    }
};

#endif //RUBIKS_CUBE_SOLVER_DFSSOLVER_H