/**
 * Convert our app's move labels to WCA/twisty-player notation.
 * Our server returns: "R", "U'", "F2", "fPrime", "b2", etc.
 * twisty-player expects standard WCA: "R", "U'", "F2"
 */
export function movesToAlgString(moves) {
  if (!moves || moves.length === 0) return '';
  return moves.join(' ');
}

/**
 * Invert a list of moves to get the setup alg that represents the scramble.
 * We also prepend "y" because twisty-player's default WCA orientation is Green-Front, 
 * but our app uses Red-Front. "y" rotates the starting orientation so Red is Front,
 * which makes all subsequent moves (F, R, B, L) perfectly align with our solver!
 */
export function invertMoves(moves) {
  const ySetup = "y "; // Align WCA orientation to our Red-Front orientation
  if (!moves || moves.length === 0) return ySetup;
  const invertMove = (m) => {
    if (m.endsWith("'")) return m.slice(0, -1);
    if (m.endsWith('2')) return m; // double moves are self-inverse
    return m + "'";
  };
  return ySetup + [...moves].reverse().map(invertMove).join(' ');
}