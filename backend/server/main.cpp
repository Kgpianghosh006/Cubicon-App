#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <opencv2/opencv.hpp>

#include "httplib.h"
#include "json.hpp"

// Cube and Solvers
#include "RubiksCube3dArray.cpp"
#include "RubiksCube1dArray.cpp"
#include "RubiksCubeBitboard.cpp"

#include "DFSSolver.h"
#include "BFSSolver.h"
#include "IDDFSSolver.h"
#include "IDAstarSolver.h"

#include "CornerPatternDatabase.h"
#include "Validator.h"

using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

// Global PDB instance loaded once on startup
CornerPatternDatabase g_pdb;
bool g_pdbLoaded = false;

// Helpers to parse request
RubiksCube::COLOR charToColor(char c) {
    switch (c) {
        case 'W': return RubiksCube::COLOR::WHITE;
        case 'G': return RubiksCube::COLOR::GREEN;
        case 'R': return RubiksCube::COLOR::RED;
        case 'B': return RubiksCube::COLOR::BLUE;
        case 'O': return RubiksCube::COLOR::ORANGE;
        case 'Y': return RubiksCube::COLOR::YELLOW;
    }
    return RubiksCube::COLOR::UNKNOWN;
}

// Convert JSON array of 54 single-character strings into a Cube
template<typename T>
T populateCube(const json& stateArr) {
    T cube;
    RubiksCube::FACE faces[] = {
        RubiksCube::FACE::UP, RubiksCube::FACE::LEFT, RubiksCube::FACE::FRONT,
        RubiksCube::FACE::RIGHT, RubiksCube::FACE::BACK, RubiksCube::FACE::DOWN
    };
    int i = 0;
    for (int f = 0; f < 6; f++) {
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                string s = stateArr[i].get<string>();
                char ch = s.empty() ? 'W' : s[0];
                cube.setColor(faces[f], r, c, charToColor(ch));
                i++;
            }
        }
    }
    return cube;
}

// Execute the requested solver
template<typename T, typename H>
void runSolver(const string& algo, T cube, httplib::Response& res) {
    long long nodes = 0;
    double ms = 0;
    vector<RubiksCube::MOVE> moves;

    auto t0 = steady_clock::now();

    if (algo == "dfs") {
        // Warning: DFS with depth=8 can be slow! Hardcoding max depth to 8 for safety
        DFSSolver<T, H> solver(cube, 8);
        moves = solver.solve();
        nodes = solver.nodesExpanded;
    }
    else if (algo == "bfs") {
        BFSSolver<T, H> solver(cube);
        moves = solver.solve();
        nodes = solver.nodesExpanded;
    }
    else if (algo == "iddfs") {
        IDDFSSolver<T, H> solver(cube, 8);
        moves = solver.solve();
        nodes = solver.nodesExpanded;
    }
    else if (algo == "idastar") {
        if (!g_pdbLoaded) {
            res.status = 500;
            res.set_content(R"({"error": "PDB not loaded"})", "application/json");
            return;
        }
        IDAstarSolver<T, H> solver(cube, g_pdb);
        moves = solver.solve();
        nodes = solver.nodesExpanded;
    }
    else {
        res.status = 400;
        res.set_content(R"({"error": "Unknown algorithm"})", "application/json");
        return;
    }

    ms = duration<double, std::milli>(steady_clock::now() - t0).count();

    // Map moves to strings
    vector<string> moveStrs;
    for (auto m : moves) {
        moveStrs.push_back(RubiksCube::getMove(m));
    }

    json out;
    out["moves"] = moveStrs;
    out["timeMs"] = ms;
    out["nodesExpanded"] = nodes;
    res.set_content(out.dump(), "application/json");
}

int main(int argc, char** argv) {
    
    string loadedPath = "backend/Databases/cornerDepth5V1.txt";

    if (g_pdbLoaded) {
        cout << "Loaded PDB successfully from " << loadedPath << endl;
    } else {
        cout << "WARNING: Could not load PDB from any known path. idastar will fail." << endl;
    }

    httplib::Server svr;

    // CORS headers
    auto set_cors = [](httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    };

    svr.Options(R"(.*)", [&set_cors](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
        res.status = 200;
    });

    // 2. Health check
    svr.Get("/api/health", [&set_cors](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
        res.set_content(R"({"status": "ok"})", "application/json");
    });

    // 3. Validation
    svr.Post("/api/validate", [&set_cors](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            auto body = json::parse(req.body);
            if (!body.contains("cubeState") || !body["cubeState"].is_array() || body["cubeState"].size() != 54) {
                res.status = 400;
                res.set_content(R"({"valid": false, "reason": "cubeState must be an array of 54 characters"})", "application/json");
                return;
            }
            // 1. Check color counts (each should be exactly 9) from the JSON explicitly
            int counts[6] = {0};
            for (int i=0; i<54; i++) {
                string s = body["cubeState"][i].get<string>();
                char ch = s.empty() ? ' ' : s[0];
                switch (charToColor(ch)) {
                    case RubiksCube::COLOR::WHITE: counts[0]++; break;
                    case RubiksCube::COLOR::GREEN: counts[1]++; break;
                    case RubiksCube::COLOR::RED: counts[2]++; break;
                    case RubiksCube::COLOR::BLUE: counts[3]++; break;
                    case RubiksCube::COLOR::ORANGE: counts[4]++; break;
                    case RubiksCube::COLOR::YELLOW: counts[5]++; break;
                    default: 
                        res.set_content(R"({"valid": false, "reason": "Invalid color character"})", "application/json");
                        return;
                }
            }
            for (int c : counts) {
                if (c != 9) {
                    res.set_content(R"({"valid": false, "reason": "Cube must have exactly 9 facelets of each color"})", "application/json");
                    return;
                }
            }

            // 2. Check centers (must be W, G, R, B, O, Y at indices 4, 13, 22, 31, 40, 49)
            string expectedCenters = "WGRBOY";
            int centerIndices[6] = {4, 13, 22, 31, 40, 49};
            for (int i = 0; i < 6; i++) {
                string s = body["cubeState"][centerIndices[i]].get<string>();
                char ch = s.empty() ? ' ' : s[0];
                if (ch != expectedCenters[i]) {
                    res.set_content(R"({"valid": false, "reason": "Centers cannot be moved. Outer layer turns do not change center colors."})", "application/json");
                    return;
                }
            }

            // 3. Physical Validator covers edges and corners parities
            RubiksCubeBitboard cube = populateCube<RubiksCubeBitboard>(body["cubeState"]);
            
            string reason = "";
            if (!Validator::isValid(cube, reason)) {
                res.set_content(string(R"({"valid": false, "reason": ")") + reason + "\"}", "application/json");
                return;
            }
            
            res.set_content(R"({"valid": true})", "application/json");
        } catch (const exception& e) {
            res.status = 400;
            res.set_content(string(R"({"valid": false, "reason": ")") + e.what() + "\"}", "application/json");
        }
    });

    // 4. Solve
    svr.Post("/api/solve", [&set_cors](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            auto body = json::parse(req.body);
            string rep = body.value("representation", "bitboard");
            string algo = body.value("algorithm", "idastar");

            if (!body.contains("cubeState") || !body["cubeState"].is_array() || body["cubeState"].size() != 54) {
                res.status = 400;
                res.set_content(R"({"error": "cubeState must be an array of 54 characters"})", "application/json");
                return;
            }

            if (rep == "3d") {
                auto cube = populateCube<RubiksCube3dArray>(body["cubeState"]);
                runSolver<RubiksCube3dArray, Hash3d>(algo, cube, res);
            }
            else if (rep == "1d") {
                auto cube = populateCube<RubiksCube1dArray>(body["cubeState"]);
                runSolver<RubiksCube1dArray, Hash1d>(algo, cube, res);
            }
            else if (rep == "bitboard") {
                auto cube = populateCube<RubiksCubeBitboard>(body["cubeState"]);
                runSolver<RubiksCubeBitboard, HashBitboard>(algo, cube, res);
            }
            else {
                res.status = 400;
                res.set_content(R"({"error": "Unknown representation"})", "application/json");
            }

        } catch (const exception& e) {
            res.status = 400;
            res.set_content(string(R"({"error": ")") + e.what() + "\"}", "application/json");
        }
    });

    // 4. Scramble
    svr.Post("/api/scramble", [&set_cors](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            auto body = json::parse(req.body);
            if (!body.contains("moves") || !body["moves"].is_string()) {
                res.status = 400;
                res.set_content(R"({"error": "moves string is required"})", "application/json");
                return;
            }
            
            string movesStr = body["moves"].get<string>();
            RubiksCubeBitboard cube;
            
            stringstream ss(movesStr);
            string token;
            while (ss >> token) {
                while (!token.empty() && (token.back() == ',' || token.back() == ' ')) token.pop_back();
                if (token.empty()) continue;
                
                RubiksCube::MOVE m;
                if (token == "L") m = RubiksCube::MOVE::L;
                else if (token == "L'" || token == "LPRIME") m = RubiksCube::MOVE::LPRIME;
                else if (token == "L2") m = RubiksCube::MOVE::L2;
                else if (token == "R") m = RubiksCube::MOVE::R;
                else if (token == "R'" || token == "RPRIME") m = RubiksCube::MOVE::RPRIME;
                else if (token == "R2") m = RubiksCube::MOVE::R2;
                else if (token == "U") m = RubiksCube::MOVE::U;
                else if (token == "U'" || token == "UPRIME") m = RubiksCube::MOVE::UPRIME;
                else if (token == "U2") m = RubiksCube::MOVE::U2;
                else if (token == "D") m = RubiksCube::MOVE::D;
                else if (token == "D'" || token == "DPRIME") m = RubiksCube::MOVE::DPRIME;
                else if (token == "D2") m = RubiksCube::MOVE::D2;
                else if (token == "F") m = RubiksCube::MOVE::F;
                else if (token == "F'" || token == "FPRIME") m = RubiksCube::MOVE::FPRIME;
                else if (token == "F2") m = RubiksCube::MOVE::F2;
                else if (token == "B") m = RubiksCube::MOVE::B;
                else if (token == "B'" || token == "BPRIME") m = RubiksCube::MOVE::BPRIME;
                else if (token == "B2") m = RubiksCube::MOVE::B2;
                else {
                    res.status = 400;
                    res.set_content(R"({"error": "Invalid move token: )" + token + R"("})", "application/json");
                    return;
                }
                cube.move(m);
            }
            
            vector<string> cubeState;
            for (int f = 0; f < 6; f++) {
                for (int r = 0; r < 3; r++) {
                    for (int c = 0; c < 3; c++) {
                        char colorChar = RubiksCube::getColorLetter(cube.getColor((RubiksCube::FACE)f, r, c));
                        cubeState.push_back(string(1, colorChar));
                    }
                }
            }
            
            json responseJson = {
                {"cubeState", cubeState}
            };
            res.set_content(responseJson.dump(), "application/json");
        } catch (const exception& e) {
            res.status = 400;
            res.set_content(string(R"({"error": ")") + e.what() + "\"}", "application/json");
        }
    });

    // 5. Scan face
    svr.Post("/api/scan-face", [&set_cors](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            if (!req.form.has_file("image")) {
                res.status = 400;
                res.set_content(R"({"error": "Missing 'image' file"})", "application/json");
                return;
            }
            if (!req.form.has_field("face") && !req.form.has_file("face")) {
                res.status = 400;
                res.set_content(R"({"error": "Missing 'face' field"})", "application/json");
                return;
            }

            auto img_file = req.form.get_file("image");
            string face_id;
            if (req.form.has_field("face")) face_id = req.form.get_field("face");
            else face_id = req.form.get_file("face").content;
            string content = img_file.content;

            // Decode image
            std::vector<uchar> buf(content.begin(), content.end());
            cv::Mat frame = cv::imdecode(buf, cv::IMREAD_COLOR);
            if (frame.empty()) {
                res.status = 400;
                res.set_content(R"({"error": "Failed to decode image"})", "application/json");
                return;
            }

            // The frontend sends a cropped square image representing the 3x3 face
            int cellW = frame.cols / 3;
            int cellH = frame.rows / 3;

            auto classifyColor = [](const cv::Vec3b& bgr) -> string {
                cv::Mat bgrPixel(1, 1, CV_8UC3, bgr);
                cv::Mat hsvPixel;
                cv::cvtColor(bgrPixel, hsvPixel, cv::COLOR_BGR2HSV);
                cv::Vec3b hsv = hsvPixel.at<cv::Vec3b>(0, 0);
                int h = hsv[0];
                int s = hsv[1];
                int v = hsv[2];

                // White detection: Low saturation OR roughly equal B/G/R above a low threshold
                if (s < 80) return "W";
                if (bgr[2] > 100 && bgr[1] > 100 && bgr[0] > 100 &&
                    abs(bgr[2] - bgr[1]) < 40 &&
                    abs(bgr[1] - bgr[0]) < 40 &&
                    abs(bgr[0] - bgr[2]) < 40) return "W";

                // OpenCV Hue wraps at 180. Red sits at both ends (0-10 and 160-180).
                if ((h >= 160 && h <= 180) || (h >= 0 && h <= 5)) return "R";
                if (h >= 6 && h <= 19)   return "O";
                if (h >= 20 && h <= 35)  return "Y"; 
                if (h >= 45 && h <= 90)  return "G"; 
                if (h >= 100 && h <= 140)return "B"; 

                return "W";
            };

            auto medianColor = [&](int centerX, int centerY, int region) -> cv::Vec3b {
                int half = region / 2;
                std::vector<uchar> B, G, R;
                for (int dy = -half; dy <= half; ++dy) {
                    for (int dx = -half; dx <= half; ++dx) {
                        int x = centerX + dx, y = centerY + dy;
                        if (x >= 0 && x < frame.cols && y >= 0 && y < frame.rows) {
                            cv::Vec3b bgr = frame.at<cv::Vec3b>(y, x);
                            B.push_back(bgr[0]); G.push_back(bgr[1]); R.push_back(bgr[2]);
                        }
                    }
                }
                std::sort(B.begin(), B.end()); std::sort(G.begin(), G.end()); std::sort(R.begin(), R.end());
                if(B.empty()) return cv::Vec3b(0,0,0);
                int mid = B.size() / 2;
                return cv::Vec3b(B[mid], G[mid], R[mid]);
            };

            json colors = json::array();
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    int cx = c * cellW + cellW / 2;
                    int cy = r * cellH + cellH / 2;
                    cv::Vec3b bgr = medianColor(cx, cy, std::min(cellW, cellH) / 3);
                    colors.push_back(classifyColor(bgr));
                }
            }

            json out;
            out["face"] = face_id;
            out["colors"] = colors;
            res.set_content(out.dump(), "application/json");

        } catch (const exception& e) {
            res.status = 500;
            res.set_content(string(R"({"error": ")") + e.what() + "\"}", "application/json");
        }
    });

    cout << "Starting server on http://localhost:8080..." << endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}
