import React, { useState, useEffect, useRef } from 'react';
import './App.css';
import CubeNet from './components/CubeNet';
import CameraScanner from './components/CameraScanner';
import CubePlayer from './components/CubePlayer';
import ScrambleInput from './components/ScrambleInput';
import { solveCube, validateCube } from './lib/apiClient';
import { movesToAlgString, invertMoves } from './lib/cubeStateUtils';

function UpwardSelect({ label, value, options, onChange }) {
  const [open, setOpen] = useState(false);
  const containerRef = useRef(null);
  const selected = options.find((option) => option.value === value);

  useEffect(() => {
    const closeOnOutsideClick = (event) => {
      if (!containerRef.current?.contains(event.target)) setOpen(false);
    };
    document.addEventListener('mousedown', closeOnOutsideClick);
    return () => document.removeEventListener('mousedown', closeOnOutsideClick);
  }, []);

  return (
    <div className="upward-select" ref={containerRef}>
      <span className="select-label">{label}</span>
      <button
        type="button"
        className="select-trigger"
        aria-haspopup="listbox"
        aria-expanded={open}
        onClick={() => setOpen((isOpen) => !isOpen)}
      >
        <span>{selected?.label}</span><span className="select-arrow"></span>
      </button>
      {open && (
        <div className="select-options" role="listbox" aria-label={label}>
          {options.map((option) => (
            <button
              key={option.value}
              type="button"
              role="option"
              aria-selected={option.value === value}
              className={option.value === value ? 'selected' : ''}
              onClick={() => { onChange(option.value); setOpen(false); }}
            >
              {option.label}
            </button>
          ))}
        </div>
      )}
    </div>
  );
}

const INITIAL_STATE = [
  ...Array(9).fill('W'), ...Array(9).fill('G'), ...Array(9).fill('R'),
  ...Array(9).fill('B'), ...Array(9).fill('O'), ...Array(9).fill('Y'),
];
const COLORS = ['W', 'G', 'R', 'B', 'O', 'Y'];
const MOVES = ['L', "L'", 'L2', 'R', "R'", 'R2', 'U', "U'", 'U2', 'D', "D'", 'D2', 'F', "F'", 'F2', 'B', "B'", 'B2'];
const COLOR_NAMES = { W: 'White / Up', G: 'Green / Left', R: 'Red / Front', B: 'Blue / Right', O: 'Orange / Back', Y: 'Yellow / Down' };
const MOVE_IMAGES = {
  L: new URL('../moves_img/L.png', import.meta.url).href,
  "L'": new URL("../moves_img/L'.png", import.meta.url).href,
  L2: new URL('../moves_img/L2.png', import.meta.url).href,
  R: new URL('../moves_img/R.png', import.meta.url).href,
  "R'": new URL("../moves_img/R'.png", import.meta.url).href,
  R2: new URL('../moves_img/R2.png', import.meta.url).href,
  U: new URL('../moves_img/U.png', import.meta.url).href,
  "U'": new URL("../moves_img/U'.png", import.meta.url).href,
  U2: new URL('../moves_img/U2.png', import.meta.url).href,
  D: new URL('../moves_img/D.png', import.meta.url).href,
  "D'": new URL("../moves_img/D'.png", import.meta.url).href,
  D2: new URL('../moves_img/D2.png', import.meta.url).href,
  F: new URL('../moves_img/F.png', import.meta.url).href,
  "F'": new URL("../moves_img/F'.png", import.meta.url).href,
  F2: new URL('../moves_img/F2.png', import.meta.url).href,
  B: new URL('../moves_img/B.png', import.meta.url).href,
  "B'": new URL("../moves_img/B'.png", import.meta.url).href,
  B2: new URL('../moves_img/B2.png', import.meta.url).href,
};

function App() {
  const [cubeState, setCubeState] = useState(INITIAL_STATE);
  const [representation, setRepresentation] = useState('bitboard');
  const [algorithm, setAlgorithm] = useState('idastar');
  const [loading, setLoading] = useState(false);
  const [result, setResult] = useState(null);
  const [error, setError] = useState(null);
  const [mode, setMode] = useState('manual');
  const [scanComplete, setScanComplete] = useState(false);
  const [resetVersion, setResetVersion] = useState(0);

  const handleStickerClick = (index) => {
    setCubeState((previous) => {
      const next = [...previous];
      next[index] = COLORS[(COLORS.indexOf(next[index]) + 1) % COLORS.length];
      return next;
    });
    setScanComplete(false);
  };

  const handleCameraComplete = (scannedState) => {
    setCubeState(scannedState);
    setScanComplete(true);
    setMode('manual');
  };

  const handleScramble = (newState) => {
    setCubeState(newState);
    setScanComplete(false);
  };

  const handleSolve = async () => {
    setLoading(true);
    setError(null);
    setResult(null);
    try {
      const snapshotState = [...cubeState];
      const validation = await validateCube(snapshotState);
      if (!validation.valid) throw new Error(validation.reason || 'Invalid cube state');
      const solveResult = await solveCube(snapshotState, representation, algorithm);
      setResult(solveResult);
    } catch (err) {
      setError(err.message || 'Failed to connect to the solver API');
    } finally {
      setLoading(false);
    }
  };

  const resetCube = () => {
    setCubeState(INITIAL_STATE);
    setResult(null);
    setError(null);
    setScanComplete(false);
    setResetVersion((version) => version + 1);
  };

  const solutionAlgStr = result ? movesToAlgString(result.moves) : '';
  const setupAlgStr = result ? invertMoves(result.moves) : '';

  return (
    <div className="app-shell">
      <header className="app-header">
        <div>
          <h1>Cubicon <span> — Rubik's Cube Solver</span></h1>
        </div>
        <div className="header-actions">

          <div className="mode-switch" aria-label="Input mode">
            <button onClick={() => setMode('manual')} className={mode === 'manual' ? 'active' : ''}>⌨ Manual</button>
            <button onClick={() => setMode('camera')} className={mode === 'camera' ? 'active' : ''}>◉ Camera</button>
          </div>
        </div>
      </header>

      {mode === 'camera' ? <CameraScanner onComplete={handleCameraComplete} /> : (
        <main className="solver-layout">
          <section className="input-column">
            <div className="card input-card">
              <div className="section-heading">
                <div><p className="section-kicker">INPUT</p><span>Set up your cube</span></div>
                {scanComplete && <span className="status-badge">Scan imported</span>}
              </div>
              <ScrambleInput onScramble={handleScramble} resetVersion={resetVersion} />
              <div className="move-grid" aria-label="Quick move controls">
                {MOVES.map((move) => (
                  <button key={move} className="move-tile" onClick={() => window.dispatchEvent(new CustomEvent('cubicon:add-move', { detail: move }))} title={`Add ${move} to scramble`} aria-label={`Add ${move} to scramble`}>
                    <img src={MOVE_IMAGES[move]} alt="" />
                    <span>{move}</span>
                  </button>
                ))}
              </div>
              <div className="entry-area">
                <div className="net-wrap">
                  <p className="mini-label">Click on a cubie to change its colour</p>
                  <CubeNet cubeState={cubeState} onStickerClick={handleStickerClick} />
                </div>
                <div className="palette" aria-label="Cube colour guide">
                  <p className="mini-label">Colour reference</p>
                  {COLORS.map((color) => <span className={`palette-chip color-${color}`} key={color} title={COLOR_NAMES[color]}>{color}</span>)}
                </div>
              </div>
            </div>

            <div className="card solve-card">
              <div className="config-grid">
                <UpwardSelect label="Representation" value={representation} onChange={setRepresentation} options={[{ value: 'bitboard', label: 'Bitboard' }, { value: '1d', label: '1D Array' }, { value: '3d', label: '3D Array' }]} />
                <UpwardSelect label="Algorithm" value={algorithm} onChange={setAlgorithm} options={[{ value: 'idastar', label: 'IDA*' }, { value: 'dfs', label: 'DFS' }, { value: 'bfs', label: 'BFS' }, { value: 'iddfs', label: 'IDDFS' }]} />
              </div>
              <div className="solve-actions">
                <button className="button secondary" onClick={resetCube}>Reset</button>
                <button className="button primary" onClick={handleSolve} disabled={loading}>
                  {loading && <span className="spinner" />}{loading ? 'Solving cube…' : 'Solve cube'}
                </button>
              </div>
              {error && <div className="error-inline"><strong>Couldn’t solve:</strong> {error}</div>}
            </div>
          </section>

          <section className="visual-column">
            {result ? (
              <>
                <div className="card playback-card">
                  <div className="section-heading"><div><p className="section-kicker">PLAYBACK</p><h2>See the solution</h2></div><span className="player-status">3D cube</span></div>
                  <CubePlayer setupAlg={setupAlgStr} solutionAlg={solutionAlgStr} />
                </div>
                <div className="result-strip">
                  <div><span>Moves Required</span><strong>{result.moves.length ? `${result.moves.length} moves` : 'Already solved'}</strong></div>
                  <div><span>Time</span><strong>{result.timeMs.toFixed(2)} ms</strong></div>
                  <div><span>Nodes Visited</span><strong>{result.nodesExpanded.toLocaleString()}</strong></div>
                </div>
                <div className="solution-string"><span>Solution</span><code>{result.moves.length ? result.moves.join(' ') : 'Cube is already solved — no moves required.'}</code></div>
              </>
            ) : (
              <div className="card visual-empty">
                <div className="cube-mark">{Array.from({ length: 9 }, (_, index) => <i key={index} />)}</div>
                <p className="section-kicker">READY TO SOLVE</p>
                <div className="instructions">
                  <h4>How to get your solution:</h4>
                  <ol>
                    <li>
                      <strong>Enter your cube:</strong> Scan the six faces, or input the scramble moves, or manually color the layout.
                    </li>
                    <li>
                      <strong>Adjust settings:</strong> Select your preferred solver configurations.
                    </li>
                    <li>
                      <strong>Solve:</strong> Click on <strong>Solve Cube</strong> to view the step-by-step path to complete your cube.
                    </li>
                  </ol>
                </div>
                <div className="empty-steps"><span>Input</span><span>Solve</span><span>Play</span></div>
              </div>
            )}
          </section>
        </main>
      )}
    </div>
  );
}

export default App;