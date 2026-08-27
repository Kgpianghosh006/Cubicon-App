import React, { useEffect, useState } from 'react';

const API_BASE = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080';

export default function ScrambleInput({ onScramble, resetVersion }) {
  const [movesStr, setMovesStr] = useState('');
  const [error, setError] = useState(null);
  const [loading, setLoading] = useState(false);

  const handleApply = async () => {
    if (!movesStr.trim()) return;
    setLoading(true);
    setError(null);
    try {
      const res = await fetch(`${API_BASE}/api/scramble`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ moves: movesStr })
      });
      const data = await res.json();
      if (!res.ok) throw new Error(data.error || 'Failed to scramble');
      onScramble(data.cubeState);
    } catch (err) {
      setError(err.message);
    } finally {
      setLoading(false);
    }
  };

  const handleKeyDown = (e) => {
    if (e.key === 'Enter') handleApply();
  };

  useEffect(() => {
    const addMove = (event) => setMovesStr((current) => `${current}${current ? ' ' : ''}${event.detail}`);
    window.addEventListener('cubicon:add-move', addMove);
    return () => window.removeEventListener('cubicon:add-move', addMove);
  }, []);

  useEffect(() => {
    setMovesStr('');
    setError(null);
  }, [resetVersion]);

  return (
    <div className="scramble-input">
      <label htmlFor="scramble">Scramble notation</label>
      <div className="scramble-row">
        <input 
          id="scramble"
          type="text" 
          value={movesStr}
          onChange={(e) => setMovesStr(e.target.value)}
          onKeyDown={handleKeyDown}
          placeholder="Enter your scramble input by clicking on moves"
        />
        <button 
          onClick={handleApply}
          disabled={loading || !movesStr.trim()}
          className="button primary apply-button"
        >
          {loading ? 'Applying...' : 'Apply'}
        </button>
      </div>
      {error && <p className="input-error">{error}</p>}
    </div>
  );
}