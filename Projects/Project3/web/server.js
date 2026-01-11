const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
app.use(cors());
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, 'public')));
// serve data (map image and CSV) to frontend
app.use('/data', express.static(path.join(__dirname, '..', 'data')));

// Prefer the CMake-built binary, but fall back to bundled executables if build/ not present
let EXEC = path.join(__dirname, '..', 'build', 'project3.exe');
const fallbackCandidates = [
  path.join(__dirname, '..', 'backend.exe'),
  path.join(__dirname, '..', 'test_bus_plans.exe')
];
// If build binary doesn't exist, pick first existing fallback
const fs = require('fs');
if (!fs.existsSync(EXEC)) {
  for (const cand of fallbackCandidates) {
    if (fs.existsSync(cand)) { EXEC = cand; break; }
  }
}
if (!fs.existsSync(EXEC)) {
  console.error('No backend executable found. Tried:', EXEC, fallbackCandidates);
  EXEC = null;
} else {
  console.log('Using backend executable:', EXEC);
}

app.post('/api/route', (req, res) => {
  const { start, end, identity } = req.body;
  if (!start || !end) return res.status(400).json({error:'start/end required'});
  if (!EXEC) return res.status(500).json({error:'backend executable not found on server. Please build project3.exe or place a compatible executable in the repo root.'});
  const args = ['--api', '--start', start, '--end', end, '--identity', String(identity||0)];
  const proc = spawn(EXEC, args, { windowsHide: true });
  let stdout=''; let stderr='';
  proc.stdout.on('data', (d) => { stdout += d.toString(); });
  proc.stderr.on('data', (d) => { stderr += d.toString(); });
  proc.on('close', (code) => {
    if (code !== 0) return res.status(500).json({error: 'backend error', details: stderr, code});
    try {
      const obj = JSON.parse(stdout);
      return res.json(obj);
    } catch (e){
      return res.status(500).json({error:'invalid backend output', raw: stdout, stderr});
    }
  });
});

const PORT = process.env.PORT || 3001;
app.listen(PORT, () => console.log(`Server listening on ${PORT}`));
