import React from 'react';

const COLOR_CLASSES = {
  W: 'sticker-W', G: 'sticker-G', R: 'sticker-R',
  B: 'sticker-B', O: 'sticker-O', Y: 'sticker-Y'
};

const FACE_INDICES = {
  U: 0,
  L: 9,
  F: 18,
  R: 27,
  B: 36,
  D: 45
};

export default function CubeNet({ cubeState, onStickerClick }) {
  const renderFace = (faceKey) => {
    const startIndex = FACE_INDICES[faceKey];
    const facelets = [];
    for (let i = 0; i < 9; i++) {
      const idx = startIndex + i;
      const color = cubeState[idx];
      facelets.push(
        <div
          key={idx}
          onClick={() => onStickerClick(idx)}
          className={`cube-sticker cursor-pointer ${COLOR_CLASSES[color]}`}
        />
      );
    }
    return (
      <div className="cube-face">
        {facelets}
      </div>
    );
  };

  return (
    <div className="cube-net">
      <div className="cube-net-grid">
        {/* Row 1 */}
        <div />
        {renderFace('U')}
        <div />
        <div />
        
        {/* Row 2 */}
        {renderFace('L')}
        {renderFace('F')}
        {renderFace('R')}
        {renderFace('B')}

        {/* Row 3 */}
        <div />
        {renderFace('D')}
        <div />
        <div />
      </div>
    </div>
  );
}