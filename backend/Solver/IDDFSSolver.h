#include <vector>
#include "../Model/RubiksCube.h"
#include "DFSSolver.h"

#ifndef RUBIKS_CUBE_SOLVER_IDDFSSOLVER_H
#define RUBIKS_CUBE_SOLVER_IDDFSSOLVER_H

template<typename T, typename H>
class IDDFSSolver {

private:
    int max_search_depth;
    long long maxNodes;
    vector<RubiksCube::MOVE> moves;

public:
    T rubiksCube;
    long long nodesExpanded = 0;

    IDDFSSolver(T _rubiksCube, int _max_search_depth = 8, long long _maxNodes = 1000000000) {
        rubiksCube = _rubiksCube;
        max_search_depth = _max_search_depth;
        maxNodes = _maxNodes;
    }

    // Used DFSSolver with increasing max_search_depth; sums nodesExpanded across iterations.
    vector<RubiksCube::MOVE> solve() {
        for (int i = 1; i <= max_search_depth; i++) {
            DFSSolver<T, H> dfsSolver(rubiksCube, i);
            moves = dfsSolver.solve();
            nodesExpanded += dfsSolver.nodesExpanded;
            if (dfsSolver.rubiksCube.isSolved()) {
                rubiksCube = dfsSolver.rubiksCube;
                break;
            }
        }
        return moves;
    }
};

#endif //RUBIKS_CUBE_SOLVER_IDDFSSOLVER_H
