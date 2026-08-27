#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "RubiksCube.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

class Validator {
public:
    static bool isValid(const RubiksCube& cube, string& reason) {
        if (cube.getColor(RubiksCube::FACE::UP, 1, 1) != RubiksCube::COLOR::WHITE) { reason = "U center must be White"; return false; }
        if (cube.getColor(RubiksCube::FACE::LEFT, 1, 1) != RubiksCube::COLOR::GREEN) { reason = "L center must be Green"; return false; }
        if (cube.getColor(RubiksCube::FACE::FRONT, 1, 1) != RubiksCube::COLOR::RED) { reason = "F center must be Red"; return false; }
        if (cube.getColor(RubiksCube::FACE::RIGHT, 1, 1) != RubiksCube::COLOR::BLUE) { reason = "R center must be Blue"; return false; }
        if (cube.getColor(RubiksCube::FACE::BACK, 1, 1) != RubiksCube::COLOR::ORANGE) { reason = "B center must be Orange"; return false; }
        if (cube.getColor(RubiksCube::FACE::DOWN, 1, 1) != RubiksCube::COLOR::YELLOW) { reason = "D center must be Yellow"; return false; }

        vector<pair<char, char>> validEdges = {
            {'W','B'}, {'W','R'}, {'W','G'}, {'W','O'},
            {'Y','B'}, {'Y','R'}, {'Y','G'}, {'Y','O'},
            {'R','B'}, {'R','G'}, {'O','B'}, {'O','G'}
        };

        vector<string> validCorners = {
            "WBR", "WRG", "WGO", "WOB",
            "YRB", "YGR", "YOG", "YBO"
        };

        int edgeParity = 0;
        vector<int> edgePermutation;
        for (int i=0; i<12; i++) edgePermutation.push_back(-1);
        bool edgesPresent[12] = {false};

        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::UP, 1, 2));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::RIGHT, 0, 1));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[0] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[0] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::UP, 2, 1));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::FRONT, 0, 1));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[1] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[1] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::UP, 1, 0));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::LEFT, 0, 1));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[2] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[2] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::UP, 0, 1));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::BACK, 0, 1));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[3] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[3] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::DOWN, 1, 2));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::RIGHT, 2, 1));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[4] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[4] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::DOWN, 0, 1));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::FRONT, 2, 1));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[5] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[5] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::DOWN, 1, 0));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::LEFT, 2, 1));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[6] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[6] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::DOWN, 2, 1));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::BACK, 2, 1));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[7] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[7] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::FRONT, 1, 2));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::RIGHT, 1, 0));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[8] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[8] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::FRONT, 1, 0));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::LEFT, 1, 2));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[9] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[9] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::BACK, 1, 0));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::RIGHT, 1, 2));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[10] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[10] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::BACK, 1, 2));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::LEFT, 1, 0));
            bool found = false;
            for (int j=0; j<12; j++) {
                if (c1 == validEdges[j].first && c2 == validEdges[j].second) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[11] = j;
                    found = true; break;
                }
                if (c1 == validEdges[j].second && c2 == validEdges[j].first) {
                    if (edgesPresent[j]) { reason = "Duplicate edge piece"; return false; }
                    edgesPresent[j] = true;
                    edgePermutation[11] = j;
                    edgeParity++;
                    found = true; break;
                }
            }
            if (!found) { reason = "Invalid edge piece"; return false; }
        }
        if (edgeParity % 2 != 0) { reason = "Invalid edge orientation parity"; return false; }

        int cornerParity = 0;
        vector<int> cornerPermutation;
        for (int i=0; i<8; i++) cornerPermutation.push_back(-1);
        bool cornersPresent[8] = {false};

        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::UP, 2, 2));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::RIGHT, 0, 0));
            char c3 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::FRONT, 0, 2));
            string s = ""; s += c1; s += c2; s += c3;
            bool found = false;
            for (int j=0; j<8; j++) {
                string v = validCorners[j];
                if (s[0] == v[0] && s[1] == v[1] && s[2] == v[2]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[0] = j; found = true; break;
                }
                if (s[0] == v[1] && s[1] == v[2] && s[2] == v[0]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[0] = j; cornerParity += 1; found = true; break;
                }
                if (s[0] == v[2] && s[1] == v[0] && s[2] == v[1]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[0] = j; cornerParity += 2; found = true; break;
                }
            }
            if (!found) { reason = "Invalid corner piece (incorrect colors or twisted in place)"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::UP, 2, 0));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::FRONT, 0, 0));
            char c3 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::LEFT, 0, 2));
            string s = ""; s += c1; s += c2; s += c3;
            bool found = false;
            for (int j=0; j<8; j++) {
                string v = validCorners[j];
                if (s[0] == v[0] && s[1] == v[1] && s[2] == v[2]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[1] = j; found = true; break;
                }
                if (s[0] == v[1] && s[1] == v[2] && s[2] == v[0]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[1] = j; cornerParity += 1; found = true; break;
                }
                if (s[0] == v[2] && s[1] == v[0] && s[2] == v[1]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[1] = j; cornerParity += 2; found = true; break;
                }
            }
            if (!found) { reason = "Invalid corner piece (incorrect colors or twisted in place)"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::UP, 0, 0));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::LEFT, 0, 0));
            char c3 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::BACK, 0, 2));
            string s = ""; s += c1; s += c2; s += c3;
            bool found = false;
            for (int j=0; j<8; j++) {
                string v = validCorners[j];
                if (s[0] == v[0] && s[1] == v[1] && s[2] == v[2]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[2] = j; found = true; break;
                }
                if (s[0] == v[1] && s[1] == v[2] && s[2] == v[0]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[2] = j; cornerParity += 1; found = true; break;
                }
                if (s[0] == v[2] && s[1] == v[0] && s[2] == v[1]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[2] = j; cornerParity += 2; found = true; break;
                }
            }
            if (!found) { reason = "Invalid corner piece (incorrect colors or twisted in place)"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::UP, 0, 2));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::BACK, 0, 0));
            char c3 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::RIGHT, 0, 2));
            string s = ""; s += c1; s += c2; s += c3;
            bool found = false;
            for (int j=0; j<8; j++) {
                string v = validCorners[j];
                if (s[0] == v[0] && s[1] == v[1] && s[2] == v[2]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[3] = j; found = true; break;
                }
                if (s[0] == v[1] && s[1] == v[2] && s[2] == v[0]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[3] = j; cornerParity += 1; found = true; break;
                }
                if (s[0] == v[2] && s[1] == v[0] && s[2] == v[1]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[3] = j; cornerParity += 2; found = true; break;
                }
            }
            if (!found) { reason = "Invalid corner piece (incorrect colors or twisted in place)"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::DOWN, 0, 2));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::FRONT, 2, 2));
            char c3 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::RIGHT, 2, 0));
            string s = ""; s += c1; s += c2; s += c3;
            bool found = false;
            for (int j=0; j<8; j++) {
                string v = validCorners[j];
                if (s[0] == v[0] && s[1] == v[1] && s[2] == v[2]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[4] = j; found = true; break;
                }
                if (s[0] == v[1] && s[1] == v[2] && s[2] == v[0]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[4] = j; cornerParity += 1; found = true; break;
                }
                if (s[0] == v[2] && s[1] == v[0] && s[2] == v[1]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[4] = j; cornerParity += 2; found = true; break;
                }
            }
            if (!found) { reason = "Invalid corner piece (incorrect colors or twisted in place)"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::DOWN, 0, 0));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::LEFT, 2, 2));
            char c3 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::FRONT, 2, 0));
            string s = ""; s += c1; s += c2; s += c3;
            bool found = false;
            for (int j=0; j<8; j++) {
                string v = validCorners[j];
                if (s[0] == v[0] && s[1] == v[1] && s[2] == v[2]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[5] = j; found = true; break;
                }
                if (s[0] == v[1] && s[1] == v[2] && s[2] == v[0]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[5] = j; cornerParity += 1; found = true; break;
                }
                if (s[0] == v[2] && s[1] == v[0] && s[2] == v[1]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[5] = j; cornerParity += 2; found = true; break;
                }
            }
            if (!found) { reason = "Invalid corner piece (incorrect colors or twisted in place)"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::DOWN, 2, 0));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::BACK, 2, 2));
            char c3 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::LEFT, 2, 0));
            string s = ""; s += c1; s += c2; s += c3;
            bool found = false;
            for (int j=0; j<8; j++) {
                string v = validCorners[j];
                if (s[0] == v[0] && s[1] == v[1] && s[2] == v[2]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[6] = j; found = true; break;
                }
                if (s[0] == v[1] && s[1] == v[2] && s[2] == v[0]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[6] = j; cornerParity += 1; found = true; break;
                }
                if (s[0] == v[2] && s[1] == v[0] && s[2] == v[1]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[6] = j; cornerParity += 2; found = true; break;
                }
            }
            if (!found) { reason = "Invalid corner piece (incorrect colors or twisted in place)"; return false; }
        }
        {
            char c1 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::DOWN, 2, 2));
            char c2 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::RIGHT, 2, 2));
            char c3 = RubiksCube::getColorLetter(cube.getColor(RubiksCube::FACE::BACK, 2, 0));
            string s = ""; s += c1; s += c2; s += c3;
            bool found = false;
            for (int j=0; j<8; j++) {
                string v = validCorners[j];
                if (s[0] == v[0] && s[1] == v[1] && s[2] == v[2]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[7] = j; found = true; break;
                }
                if (s[0] == v[1] && s[1] == v[2] && s[2] == v[0]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[7] = j; cornerParity += 1; found = true; break;
                }
                if (s[0] == v[2] && s[1] == v[0] && s[2] == v[1]) {
                    if (cornersPresent[j]) { reason = "Duplicate corner piece"; return false; }
                    cornersPresent[j] = true; cornerPermutation[7] = j; cornerParity += 2; found = true; break;
                }
            }
            if (!found) { reason = "Invalid corner piece (incorrect colors or twisted in place)"; return false; }
        }
        if (cornerParity % 3 != 0) { reason = "Invalid corner orientation parity"; return false; }

        int edgeSwaps = 0;
        for (int i=0; i<12; i++) {
            if (edgePermutation[i] != i) {
                int j = i;
                while (edgePermutation[j] != i) {
                    j++;
                }
                swap(edgePermutation[i], edgePermutation[j]);
                edgeSwaps++;
            }
        }

        int cornerSwaps = 0;
        for (int i=0; i<8; i++) {
            if (cornerPermutation[i] != i) {
                int j = i;
                while (cornerPermutation[j] != i) {
                    j++;
                }
                swap(cornerPermutation[i], cornerPermutation[j]);
                cornerSwaps++;
            }
        }

        if (edgeSwaps % 2 != cornerSwaps % 2) { reason = "Invalid permutation parity (edges and corners must have same parity)"; return false; }

        return true;
    }
};
#endif
