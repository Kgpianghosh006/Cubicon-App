#include <vector>
#include <unordered_map>
#include <queue>
#include <cassert>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <chrono>
#include "../Model/RubiksCube.h"
#include "../PatternDatabases/CornerPatternDatabase.h"

#ifndef RUBIKS_CUBE_SOLVER_IDASTARSOLVER_H
#define RUBIKS_CUBE_SOLVER_IDASTARSOLVER_H

template<typename T, typename H, typename Heuristic = CornerPatternDatabase>
class IDAstarSolver {
private:
    Heuristic& heuristic;
    vector<RubiksCube::MOVE> moves;
    unordered_map<T, RubiksCube::MOVE, H> move_done;
    unordered_map<T, bool, H> visited;

    struct Node {
        T cube;
        int depth;
        int estimate;

        Node(T _cube, int _depth, int _estimate)
            : cube(_cube), depth(_depth), estimate(_estimate) {}
    };

    struct compareCube {
        bool operator()(pair<Node, int> const &p1, pair<Node, int> const &p2) {
            auto n1 = p1.first, n2 = p2.first;
            if (n1.depth + n1.estimate == n2.depth + n2.estimate) {
                return n1.estimate > n2.estimate;
            } else return n1.depth + n1.estimate > n2.depth + n2.estimate;
        }
    };

    void resetStructure() {
        moves.clear();
        move_done.clear();
        visited.clear();
    }

    pair<T, int> IDAstar(int bound) {
        priority_queue<pair<Node, int>, vector<pair<Node, int>>, compareCube> pq;
        Node start = Node(rubiksCube, 0, heuristic.getNumMoves(rubiksCube));
        pq.push(make_pair(start, 0));
        int next_bound = 100;
        
        auto start_time = std::chrono::steady_clock::now();

        while (!pq.empty()) {
            auto p = pq.top();
            Node node = p.first;
            pq.pop();

            if (visited[node.cube]) continue;

            nodesExpanded++;
            if (nodesExpanded > maxNodes) {
                throw std::runtime_error("Search timeout: expanded too many nodes");
            }
            
            // Check time limit (110 seconds to avoid browser 120s fetch timeout)
            if (nodesExpanded % 10000 == 0) {
                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() > 110) {
                    throw std::runtime_error("Search timeout: Exceeded 110 seconds. The heuristic is too weak for this scramble.");
                }
            }

            visited[node.cube] = true;
            move_done[node.cube] = RubiksCube::MOVE(p.second);

            if (node.cube.isSolved()) return make_pair(node.cube, bound);
            node.depth++;
            for (int i = 0; i < 18; i++) {
                auto curr_move = RubiksCube::MOVE(i);
                node.cube.move(curr_move);
                if (!visited[node.cube]) {
                    node.estimate = heuristic.getNumMoves(node.cube);
                    if (node.estimate + node.depth > bound) {
                        next_bound = min(next_bound, node.estimate + node.depth);
                    } else {
                        pq.push(make_pair(node, i));
                    }
                }
                node.cube.invert(curr_move);
            }
        }
        return make_pair(rubiksCube, next_bound);
    }

public:
    T rubiksCube;
    long long nodesExpanded = 0;
    long long maxNodes = 1000000000;

    IDAstarSolver(T _rubiksCube, Heuristic& _heuristic)
        : rubiksCube(_rubiksCube), heuristic(_heuristic) {}

    vector<RubiksCube::MOVE> solve() {
        int bound = 1;
        auto p = IDAstar(bound);
        while (p.second != bound) {
            resetStructure();
            bound = p.second;
            p = IDAstar(bound);
        }
        T solved_cube = p.first;
        T curr_cube = solved_cube;
        while (!(curr_cube == rubiksCube)) {
            RubiksCube::MOVE curr_move = move_done[curr_cube];
            moves.push_back(curr_move);
            curr_cube.invert(curr_move);
        }
        rubiksCube = solved_cube;
        reverse(moves.begin(), moves.end());
        return moves;
    }
};

#endif //RUBIKS_CUBE_SOLVER_IDASTARSOLVER_H
