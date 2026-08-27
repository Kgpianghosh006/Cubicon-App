const API_BASE = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080';

async function request(endpoint, method = 'GET', body = null, timeoutMs = 10000) {
  const controller = new AbortController();
  const timer = setTimeout(() => controller.abort(), timeoutMs);

  const options = {
    method,
    signal: controller.signal,
    headers: { 'Content-Type': 'application/json' }
  };
  if (body) options.body = JSON.stringify(body);

  try {
    const res = await fetch(`${API_BASE}${endpoint}`, options);
    clearTimeout(timer);
    const data = await res.json();
    if (!res.ok) {
      throw new Error(data.error || data.reason || `HTTP error ${res.status}`);
    }
    return data;
  } catch (err) {
    clearTimeout(timer);
    if (err.name === 'AbortError') {
      throw new Error('Request timed out. The puzzle might be too scrambled for this heuristic to solve quickly.');
    }
    throw err;
  }
}

// IDA* on deep scrambles can take up to ~30s — use a generous timeout
export async function solveCube(cubeState, representation, algorithm) {
  const timeoutMs = algorithm === 'idastar' ? 120000 : 15000;
  return request('/api/solve', 'POST', { cubeState, representation, algorithm }, timeoutMs);
}

export async function validateCube(cubeState) {
  return request('/api/validate', 'POST', { cubeState }, 10000);
}

export async function checkHealth() {
  return request('/api/health', 'GET', null, 5000);
}