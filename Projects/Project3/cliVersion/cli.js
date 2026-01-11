const { spawn } = require('child_process');
const fs = require('fs');
const path = require('path');

// From cliVersion folder, project root is parent
const projectRoot = path.resolve(__dirname, '..');
const candidates = [
  path.join(projectRoot, 'build', 'project3.exe'),
  path.join(projectRoot, 'test_bus_plans.exe'),
  path.join(projectRoot, 'backend.exe')
];

let exe = null;
for (const c of candidates){ if (fs.existsSync(c)){ exe = c; break; } }
if (!exe){
  console.error('No executable found. Checked:');
  candidates.forEach(c=> console.error('  ' + c));
  console.error('\nBuild the project first (from project root):');
  console.error('  cmake -S . -B build');
  console.error('  cmake --build build --config Release');
  process.exit(1);
}

console.log('Spawning backend executable:', exe);

// Spawn interactive child and connect stdio so the backend's interactive loop works
const child = spawn(exe, { stdio: 'inherit' });
child.on('exit', (code) => { process.exit(code === null ? 0 : code); });
child.on('error', (err) => { console.error('Failed to start backend:', err); process.exit(1); });
