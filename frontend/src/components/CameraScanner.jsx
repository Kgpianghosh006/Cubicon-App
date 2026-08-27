import React, { useCallback, useEffect, useRef, useState } from 'react';

const FACES = ['U', 'L', 'F', 'R', 'B', 'D'];
const API_BASE = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080';
const COLOR_CLASSES = { W: 'color-W', G: 'color-G', R: 'color-R', B: 'color-B', O: 'color-O', Y: 'color-Y' };
const INSTRUCTIONS = {
  U: { center: 'WHITE', top: 'ORANGE' }, L: { center: 'GREEN', top: 'WHITE' },
  F: { center: 'RED', top: 'WHITE' }, R: { center: 'BLUE', top: 'WHITE' },
  B: { center: 'ORANGE', top: 'WHITE' }, D: { center: 'YELLOW', top: 'RED' },
};

export default function CameraScanner({ onComplete }) {
  const videoRef = useRef(null);
  const canvasRef = useRef(null);
  const streamRef = useRef(null);
  const [currentFaceIdx, setCurrentFaceIdx] = useState(0);
  const [capturedImage, setCapturedImage] = useState(null);
  const [scannedColors, setScannedColors] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [assembledState, setAssembledState] = useState(Array(54).fill('W'));

  const stopCamera = useCallback(() => {
    streamRef.current?.getTracks().forEach((track) => track.stop());
    streamRef.current = null;
  }, []);

  useEffect(() => {
    let cancelled = false;
    async function startCamera() {
      try {
        const stream = await navigator.mediaDevices.getUserMedia({ video: { facingMode: 'environment' } });
        if (cancelled) { stream.getTracks().forEach((track) => track.stop()); return; }
        streamRef.current = stream;
        if (videoRef.current) videoRef.current.srcObject = stream;
      } catch (cameraError) {
        setError(`Failed to access camera: ${cameraError.message}`);
      }
    }
    startCamera();
    return () => { cancelled = true; stopCamera(); };
  }, [stopCamera]);

  // The preview video is intentionally unmounted while a captured face is
  // reviewed. Reconnect the already-open stream when it is rendered again so
  // the next face has a live frame to capture.
  useEffect(() => {
    if (capturedImage || !videoRef.current || !streamRef.current) return;
    videoRef.current.srcObject = streamRef.current;
    videoRef.current.play().catch(() => {
      // Autoplay is requested on the element; browsers may still defer it
      // briefly while the element is being mounted.
    });
  }, [capturedImage]);

  const uploadImage = useCallback((canvas) => {
    setLoading(true);
    setError(null);
    canvas.toBlob(async (blob) => {
      const formData = new FormData();
      formData.append('image', blob, 'face.jpg');
      formData.append('face', FACES[currentFaceIdx]);
      try {
        const response = await fetch(`${API_BASE}/api/scan-face`, { method: 'POST', body: formData });
        const data = await response.json();
        if (!response.ok) throw new Error(data.error || 'Failed to scan');
        setScannedColors(data.colors);
      } catch (scanError) {
        setError(scanError.message);
        setCapturedImage(null);
      } finally {
        setLoading(false);
      }
    }, 'image/jpeg');
  }, [currentFaceIdx]);

  const handleCapture = useCallback(() => {
    const video = videoRef.current;
    if (!video?.videoWidth) {
      setError('Camera preview is still starting. Please try again in a moment.');
      return;
    }
    const size = Math.min(video.videoWidth, video.videoHeight) * 0.6;
    const sx = (video.videoWidth - size) / 2;
    const sy = (video.videoHeight - size) / 2;
    const canvas = document.createElement('canvas');
    canvas.width = size;
    canvas.height = size;
    canvas.getContext('2d').drawImage(video, sx, sy, size, size, 0, 0, size, size);
    setCapturedImage(canvas.toDataURL('image/jpeg'));
    uploadImage(canvas);
  }, [uploadImage]);

  const retakeFace = useCallback(() => {
    setCapturedImage(null);
    setScannedColors(null);
  }, []);

  const confirmFace = useCallback(() => {
    if (!scannedColors) return;
    const newState = [...assembledState];
    const offset = currentFaceIdx * 9;
    scannedColors.forEach((color, index) => { newState[offset + index] = color; });
    setAssembledState(newState);
    setCapturedImage(null);
    setScannedColors(null);
    if (currentFaceIdx < FACES.length - 1) {
      setCurrentFaceIdx((index) => index + 1);
    } else {
      stopCamera();
      onComplete(newState);
    }
  }, [assembledState, currentFaceIdx, onComplete, scannedColors, stopCamera]);

  useEffect(() => {
    const onKeyDown = (event) => {
      if (event.target.tagName === 'INPUT' || event.target.tagName === 'TEXTAREA') return;
      if (event.code === 'Space') {
        event.preventDefault();
        if (capturedImage) retakeFace();
        else handleCapture();
      }
      if (event.code === 'Enter' && capturedImage && scannedColors && !loading) { event.preventDefault(); confirmFace(); }
    };
    window.addEventListener('keydown', onKeyDown);
    return () => window.removeEventListener('keydown', onKeyDown);
  }, [capturedImage, confirmFace, handleCapture, loading, retakeFace, scannedColors]);

  useEffect(() => {
    if (capturedImage) return undefined;
    const canvas = canvasRef.current;
    const video = videoRef.current;
    if (!canvas || !video) return undefined;
    let animationId;
    const drawOverlay = () => {
      const rect = canvas.getBoundingClientRect();
      canvas.width = rect.width;
      canvas.height = rect.height;
      const context = canvas.getContext('2d');
      context.clearRect(0, 0, canvas.width, canvas.height);
      const size = Math.min(canvas.width, canvas.height) * 0.6;
      const x = (canvas.width - size) / 2;
      const y = (canvas.height - size) / 2;
      context.strokeStyle = 'white';
      context.lineWidth = 2;
      for (let index = 0; index <= 3; index += 1) {
        const step = (size / 3) * index;
        context.beginPath(); context.moveTo(x + step, y); context.lineTo(x + step, y + size); context.stroke();
        context.beginPath(); context.moveTo(x, y + step); context.lineTo(x + size, y + step); context.stroke();
      }
      animationId = requestAnimationFrame(drawOverlay);
    };
    drawOverlay();
    return () => cancelAnimationFrame(animationId);
  }, [capturedImage]);

  const currentFace = FACES[currentFaceIdx];
  const instructions = INSTRUCTIONS[currentFace];

  return (
    <main className="camera-workspace">
      <div className="camera-head">
        <div><p className="eyebrow">CAMERA SCAN</p><h2>Capture each face in order</h2></div>
        <p className="camera-progress">Face {currentFaceIdx + 1} of 6</p>
      </div>
      <div className="face-stepper" aria-label={`Face ${currentFaceIdx + 1} of 6`}>
        {FACES.map((face, index) => <span key={face} className={index < currentFaceIdx ? 'done' : index === currentFaceIdx ? 'current' : ''}><b>{index < currentFaceIdx ? '✓' : index + 1}</b>{face}</span>)}
      </div>
      <div className="camera-card">
        <aside className="scan-instructions">
          <p className="section-kicker">FACE {currentFaceIdx + 1} OF 6</p>
          <h3>Scan {currentFace}</h3>
          <p>Place the <strong>{instructions.center}</strong> centre sticker in the guide. Keep the <strong>{instructions.top}</strong> face pointing up.</p>
          <kbd>Space</kbd><small>capture / retake</small><br />
          <kbd>Enter</kbd><small>confirm result</small>
        </aside>
        <div className="camera-preview">
          {error && <div className="camera-error">{error}</div>}
          {!capturedImage && <div className="camera-feed">
            <video ref={videoRef} autoPlay playsInline className="camera-video" />
            <canvas ref={canvasRef} className="camera-overlay" />
          </div>}
          {capturedImage && <div className="capture-review">
            <img src={capturedImage} alt={`Captured ${currentFace} face`} />
            <div>
              {loading && <p className="classifying">Classifying face…</p>}
              {scannedColors && <div className="capture-result"><p>Detected Face</p><div>{scannedColors.map((color, index) => <i key={index} className={COLOR_CLASSES[color] || 'bg-gray-500'} />)}</div></div>}
            </div>
          </div>}
        </div>
        <div className="camera-actions">
          {!capturedImage ? <button onClick={handleCapture} className="button primary">Capture {currentFace} <span>[Space]</span></button> : <>
            <button onClick={retakeFace} disabled={loading} className="button secondary">Retake <span>[Space]</span></button>
            <button onClick={confirmFace} disabled={loading || !scannedColors} className="button primary">Confirm <span>[Enter]</span></button>
          </>}
        </div>
      </div>
    </main>
  );
}