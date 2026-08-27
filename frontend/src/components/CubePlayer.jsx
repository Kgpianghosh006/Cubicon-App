import React, { useEffect, useRef, useState } from 'react';
import 'cubing/twisty'; // Registers the <twisty-player> custom element

// CubePlayer - thin React wrapper around <twisty-player>.

export default function CubePlayer({ setupAlg, solutionAlg }) {
  const playerRef = useRef(null);
  const [speed, setSpeed] = useState(1);
  const [isPlaying, setIsPlaying] = useState(false);

  // Push new alg/setupAlg to the web component whenever props change.
  useEffect(() => {
    const player = playerRef.current;
    if (!player) return;

    // experimentalSetupAlg: moves that transform a solved cube INTO the
    // scrambled state shown as the start position.
    player.experimentalSetupAlg = setupAlg || '';

    // alg: the solution moves. Playing through them goes solved → solved
    // from the perspective of the player (start=scrambled, end=solved).
    player.alg = solutionAlg || '';

    // Always reset to start frame when a new puzzle is loaded.
    if (typeof player.jumpToStart === 'function') {
      player.jumpToStart();
    }
    setIsPlaying(false);
  }, [setupAlg, solutionAlg]);

  // The player exposes its timeline through its supported experimental model.
  // Keeping React in sync with that source resets the pause icon once playback
  // naturally reaches the final move.
  useEffect(() => {
    const player = playerRef.current;
    if (!player?.experimentalModel) return undefined;
    let active = true;
    player.experimentalModel.coarseTimelineInfo.addFreshListener((timeline) => {
      if (active) setIsPlaying(timeline.playing && !timeline.atEnd);
    });
    return () => { active = false; };
  }, [setupAlg, solutionAlg]);

  // Keep the tempoScale property in sync with the speed slider.
  useEffect(() => {
    const player = playerRef.current;
    if (!player) return;
    player.tempoScale = speed;
  }, [speed]);

  const handlePlay = async () => {
    const player = playerRef.current;
    if (!player) return;
    const timeline = await player.experimentalModel?.detailedTimelineInfo.get();
    if (timeline?.atEnd) player.jumpToStart();
    player.play();
    setIsPlaying(true);
  };

  const handlePause = () => {
    playerRef.current?.pause();
    setIsPlaying(false);
  };

  const handleJumpStart = () => {
    playerRef.current?.jumpToStart();
    setIsPlaying(false);
  };

  const handleJumpEnd = () => {
    playerRef.current?.jumpToEnd();
    setIsPlaying(false);
  };

  const step = async (direction) => {
    const player = playerRef.current;
    if (!player?.experimentalModel) return;
    player.pause();
    const [timeline, indexer] = await Promise.all([
      player.experimentalModel.detailedTimelineInfo.get(),
      player.experimentalModel.indexer.get(),
    ]);
    const currentIndex = indexer.timestampToIndex(timeline.timestamp);
    let targetTimestamp;
    if (direction < 0) {
      targetTimestamp = timeline.atStart ? timeline.timeRange.start : indexer.indexToMoveStartTimestamp(currentIndex);
    } else if (timeline.atEnd) {
      targetTimestamp = timeline.timeRange.end;
    } else {
      targetTimestamp = indexer.indexToMoveStartTimestamp(currentIndex) + indexer.moveDuration(currentIndex);
    }
    player.experimentalModel.timestampRequest.set(targetTimestamp);
    setIsPlaying(false);
  };

  const handleStepForward = () => step(1);
  const handleStepBack = () => step(-1);

  return (
    <div className="cube-player">
      {/* The web component itself. Initial attributes set here for SSR safety;
          all dynamic updates go through the useEffect above. */}
      <twisty-player
        ref={playerRef}
        puzzle="3x3x3"
        hint-facelets="none"
        background="none"
        control-panel="none"
        style={{ width: '100%', maxWidth: '400px', aspectRatio: '4/3' }}
      />

      {/* Custom controls */}
      <div className="player-controls">
        <button
          onClick={handleJumpStart}
          title="Jump to start"
          className="player-button"
        >
          ⏮
        </button>
        <button
          onClick={handleStepBack}
          title="Step back"
          className="player-button"
        >
          ◀
        </button>
        {isPlaying ? (
          <button
            onClick={handlePause}
            title="Pause"
            className="player-button player-button-main"
          >
            ⏸
          </button>
        ) : (
          <button
            onClick={handlePlay}
            title="Play"
            className="player-button player-button-main"
          >
            ▶
          </button>
        )}
        <button
          onClick={handleStepForward}
          title="Step forward"
          className="player-button"
        >
          ▶
        </button>
        <button
          onClick={handleJumpEnd}
          title="Jump to end"
          className="player-button"
        >
          ⏭
        </button>
      </div>

      {/* Speed control */}
      <div className="speed-control">
        <label>Speed</label>
        <input
          type="range"
          min="0.25"
          max="4"
          step="0.25"
          value={speed}
          onChange={(e) => setSpeed(parseFloat(e.target.value))}
          className="speed-slider"
        />
        <span>{speed}×</span>
      </div>
    </div>
  );
}