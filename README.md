# Cubicon - Rubik's Cube Solver

Cubicon is a full-stack web application that solves any Rubik's Cube state. Built with a highly responsive React/Vite frontend and powered by a highly optimized C++ backend REST API. Cubicon combines computer vision for automatic face scanning with advanced pathfinding algorithms to calculate the shortest solution, then visualizes it via an interactive 3D WebGL player.

### [Live Demo](YOUR_VERCEL_LINK_HERE) - Experience the solver in action.

---

## ✨ Features

- **Algorithmic Solver Engine:** Implements multiple search algorithms including DFS, BFS, IDDFS, and a lightning-fast IDA* (Iterative Deepening A*) solver powered by pre-computed Pattern Databases (PDBs).
- **Computer Vision Extraction:** Built-in scanner leverages OpenCV to automatically detect and extract sticker colors from live camera feeds using HSV thresholding and median spatial sampling.
- **Interactive 3D Playback:** Integrates `cubing.js` (`twisty-player`) for a fluid WebGL-based animated 3D playback of the generated solution.
- **Flexible Inputs:** Support for 3 input modes: webcam scanning, manual 2D facelet painting, and standard WCA scramble notation.
- **Responsive UI:** Custom CSS Grid/Flexbox architecture with a locked native-app feel on desktop and seamless vertical stacking on mobile.

---

## 🛠️ Tech Stack

### Frontend
- **Framework:** React 19, Vite
- **Styling:** TailwindCSS (utilities) + Custom App CSS (responsive layouts)
- **3D Rendering:** `cubing.js`

### Backend
- **Language:** C++17 (MSYS2/MinGW on Windows)
- **API Framework:** `cpp-httplib` (REST API routing)
- **Computer Vision:** OpenCV (`cv::cvtColor`, `cv::imdecode`)
- **Data Parsing:** `nlohmann/json`

---

## 📂 File Structure

```text
Cubicon/
├── backend/
│   ├── CMakeLists.txt
│   ├── Databases/
│   │   └── cornerDepth5V1.txt        # Pattern Database (PDB)
│   ├── Model/
│   │   ├── RubiksCube.cpp/.h         # Base cube representation
│   │   ├── RubiksCube1dArray.cpp     # 1D Array implementation
│   │   ├── RubiksCube3dArray.cpp     # 3D Array implementation
│   │   ├── RubiksCubeBitboard.cpp    # Bitboard implementation
│   │   └── PatternDatabase/          # Heuristic tables
│   ├── Solver/
│   │   ├── BFSSolver.h               # Breadth-First Search
│   │   ├── DFSSolver.h               # Depth-First Search
│   │   ├── IDDFSSolver.h             # Iterative Deepening DFS
│   │   └── IDAstarSolver.h           # IDA* with PDB heuristic
│   └── server/
│       ├── main.cpp                  # REST API & OpenCV logic
│       ├── httplib.h                 # HTTP server library
│       └── json.hpp                  # JSON parsing
├── frontend/
│   ├── index.html
│   ├── package.json
│   ├── vite.config.js
│   ├── src/
│   │   ├── App.jsx / App.css         # Main application & routing
│   │   ├── main.jsx / index.css      # Entry point & Tailwind imports
│   │   ├── components/               # React components
│   │   │   ├── CameraScanner.jsx     # Webcam capture UI
│   │   │   ├── CubeNet.jsx           # 2D flat cube editor
│   │   │   ├── CubePlayer.jsx        # 3D WebGL playback
│   │   │   └── ScrambleInput.jsx     # Manual WCA notation input
│   │   └── lib/
│   │       ├── apiClient.js          # Backend fetch wrappers
│   │       └── cubeStateUtils.js     # State translation logic
└── nixpacks.toml                     # Deployment configuration
```

---

## 🚀 Local Setup & Installation

### 1. Backend Setup

Ensure you have CMake, a C++17 compiler (like GCC via MSYS2), and OpenCV installed on your system.

```bash
cd backend

# Configure CMake
cmake -G "MinGW Makefiles" -B build

# Build the server executable
cmake --build build --target rubiks_server

# Run the server (starts on http://localhost:8080)
./build/rubiks_server.exe
```

### 2. Frontend Setup

Ensure you have Node.js installed.

```bash
cd frontend

# Install dependencies
npm install

# Start the Vite development server
npm run dev
```

The frontend will be available at `http://localhost:5173`. Make sure the backend server is running simultaneously so the frontend can successfully communicate with the API!
