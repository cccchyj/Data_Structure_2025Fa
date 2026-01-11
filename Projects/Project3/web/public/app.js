const API = '/api/route';
let points = {}; // label -> {x,y}

async function loadPoints(){
  const res = await fetch('/data/pointPixel.csv');
  const txt = await res.text();
  // csv lines: A,x,y
  txt.split(/\r?\n/).forEach(line=>{ const t=line.trim(); if(!t) return; const parts=t.split(','); const lab = parts[0].trim(); const x = parseFloat(parts[1]); const y = parseFloat(parts[2]); points[lab]= {x,y}; });
}

function mapToClientCoords(px,py,img){
  // original image resolution 2560x1437
  const origW = 2560, origH = 1437;
  const rect = img.getBoundingClientRect();
  const scaleX = rect.width / origW;
  const scaleY = rect.height / origH;
  return { cx: px * scaleX + rect.left, cy: py * scaleY + rect.top, rx: px*scaleX, ry: py*scaleY };
}

function clearOverlay(){ const svg = document.getElementById('overlay'); while(svg.firstChild) svg.removeChild(svg.firstChild); }

function drawRoute(path, steps){
  const img = document.getElementById('mapImg'); const svg = document.getElementById('overlay');
  clearOverlay();
  if (!img.complete) return;
  // original image resolution
  const origW = 2560, origH = 1437;
  const imgRect = img.getBoundingClientRect();
  const svgRect = svg.getBoundingClientRect();

  // position SVG over the image and use viewBox in original image pixel coordinates
  svg.style.position = 'absolute';
  svg.style.left = (imgRect.left - svgRect.left) + 'px';
  svg.style.top = (imgRect.top - svgRect.top) + 'px';
  svg.style.width = imgRect.width + 'px';
  svg.style.height = imgRect.height + 'px';
  svg.setAttribute('viewBox', `0 0 ${origW} ${origH}`);
  svg.setAttribute('preserveAspectRatio', 'xMidYMid meet');

  const ns = 'http://www.w3.org/2000/svg';
  const defs = document.createElementNS(ns,'defs');
  svg.appendChild(defs);
  const createdMarkers = new Set();

  for (let si=0; si<steps.length; ++si){
    const st = steps[si];
    const color = (st.mode==='bus')? getComputedStyle(document.body).getPropertyValue('--bus').trim() : getComputedStyle(document.body).getPropertyValue('--accent').trim();
    const nodes = st.path;
    for (let i=0;i+1<nodes.length;i++){
      const a = points[nodes[i]]; const b = points[nodes[i+1]];
      if (!a || !b) continue;
      // draw in original image pixel coordinates; SVG scales via viewBox
      const x1 = a.x, y1 = a.y, x2 = b.x, y2 = b.y;
      const line = document.createElementNS(ns,'line');
      line.setAttribute('x1', x1); line.setAttribute('y1', y1); line.setAttribute('x2', x2); line.setAttribute('y2', y2);
      line.setAttribute('stroke', color); line.setAttribute('stroke-width', 8); line.setAttribute('stroke-linecap', 'round');
      const colorId = 'arrow_' + color.replace(/[^a-z0-9]/ig,'');
      if (!createdMarkers.has(colorId)){
        const marker = document.createElementNS(ns,'marker');
        marker.setAttribute('id', colorId); marker.setAttribute('markerWidth','10'); marker.setAttribute('markerHeight','10'); marker.setAttribute('refX','8'); marker.setAttribute('refY','3'); marker.setAttribute('orient','auto'); marker.setAttribute('markerUnits','strokeWidth');
        const pathEl = document.createElementNS(ns,'path'); pathEl.setAttribute('d','M0,0 L0,6 L9,3 z'); pathEl.setAttribute('fill', color);
        marker.appendChild(pathEl); defs.appendChild(marker); createdMarkers.add(colorId);
      }
      line.setAttribute('marker-end','url(#' + colorId + ')');
      svg.appendChild(line);
    }
  }

  // remember last drawn so we can redraw on resize
  window.__lastDraw = { path, steps };
}

function makeCard(title, metaText, onClick){
  const el = document.createElement('div'); el.className='card';
  const titleEl = document.createElement('div'); titleEl.className='title'; titleEl.textContent = title;
  const metaEl = document.createElement('div'); metaEl.className='meta'; 
  // if metaText contains HTML tags (<br>), set as HTML, else as text
  if (/<br\s*\/>|<br>/i.test(metaText) || /<strong>/i.test(metaText)) metaEl.innerHTML = metaText; else metaEl.textContent = metaText;
  el.appendChild(titleEl); el.appendChild(metaEl);
  el.addEventListener('click', onClick);
  return el;
}

function escapeHtml(unsafe){
  return String(unsafe).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;').replace(/'/g, '&#039;');
}

async function planRoute(){
  const s = document.getElementById('start').value.trim().toUpperCase();
  const t = document.getElementById('end').value.trim().toUpperCase();
  const identity = parseInt(document.getElementById('identity').value,10);
  if (!s || !t) return alert('Enter start and end');
  // validate nodes exist in point list
  if (!points[s]) return alert('Start point not found on map: ' + s);
  if (!points[t]) return alert('End point not found on map: ' + t);
  const res = await fetch(API, { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({start:s,end:t,identity}) });
  const data = await res.json();
  if (data.error) return alert('Error: '+JSON.stringify(data));
  document.getElementById('permText').innerText = (identity==0)? 'Visitor (no campus)' : (identity==1? 'FDU' : (identity==2? 'SHUTCM' : 'Both'));
  
  const routesContainer = document.getElementById('routesContainer'); routesContainer.innerHTML='';
  const showWalk = document.getElementById('mode_walk').checked;
  const showBike = document.getElementById('mode_bike').checked;
  const showCar = document.getElementById('mode_car').checked;
  const showBus = document.getElementById('mode_bus').checked;
  
  // Filter routes by selected modes
  let filteredRoutes = [];
  let pureWalkRoutes = [];
  data.routes.forEach(r => {
    const hasWalk = r.times && r.times.walk && r.times.walk > 0.0001;
    const hasBike = r.times && r.times.bike && r.times.bike > 0.0001;
    const hasCar = r.times && r.times.car && r.times.car > 0.0001;
    const showRoute = (showWalk && hasWalk) || (showBike && hasBike) || (showCar && hasCar);
    if (showRoute) {
      filteredRoutes.push(r);
    }
    // Collect pure walk routes for optimal distance reference
    if (hasWalk && (!hasBike || r.times.bike === 0) && (!hasCar || r.times.car === 0)) {
      pureWalkRoutes.push(r);
    }
  });
  
  // Sort routes by distance ascending
  filteredRoutes.sort((a, b) => a.distance - b.distance);
  
  // Get optimal walking distance (shortest pure walk route)
  pureWalkRoutes.sort((a, b) => a.distance - b.distance);
  const optimalWalkingDistance = pureWalkRoutes.length > 0 ? pureWalkRoutes[0].distance : 0;
  
  // Get overall optimal distance (shortest route among all modes)
  const optimalDistance = filteredRoutes.length > 0 ? filteredRoutes[0].distance : 0;
  
  // Filter out routes that are more than 3x longer than optimal
  filteredRoutes = filteredRoutes.filter(r => r.distance <= optimalDistance * 3);
  
  // Keep only top 3 optimal routes
  filteredRoutes = filteredRoutes.slice(0, 3);
  
  // Display filtered routes
  filteredRoutes.forEach((r, idx) => {
    const hasWalk = r.times && r.times.walk && r.times.walk > 0.0001;
    const hasBike = r.times && r.times.bike && r.times.bike > 0.0001;
    const hasCar = r.times && r.times.car && r.times.car > 0 && r.times.car > 0.0001;
    
    const title = `Route ${idx + 1} (${r.distance} m)`;
    // build meta with segments on separate lines and times
    let metaLines = [];
    if (r.segments && r.segments.length) metaLines = metaLines.concat(r.segments);
    const times = r.times || {};
    // build HTML with bold 'Times:'
    let metaHtml = '';
    if (metaLines.length) metaHtml += metaLines.map(x => escapeHtml(x)).join('<br>') + '<br>';
    metaHtml += '<strong>Times:</strong><br>';
    metaHtml += 'Walk: ' + (times.walk !== undefined ? times.walk.toFixed(2) + ' min' : '-') + '<br>';
    metaHtml += 'Bike: ' + (times.bike !== undefined ? times.bike.toFixed(2) + ' min' : '-') + '<br>';
    metaHtml += 'Car: ' + (times.car !== undefined ? (times.car < 0 ? '-' : times.car.toFixed(2) + ' min') : '-');
    const card = makeCard(title, metaHtml, () => {
      // clear previous selection highlighting
      document.querySelectorAll('.card').forEach(c => c.classList.remove('selected'));
      card.classList.add('selected');
      // draw route; choose color by preference: if bike selected show bike color, else walk
      const mode = (showBike && hasBike) ? 'bike' : (showCar && hasCar) ? 'car' : 'walk';
      drawRoute(r.nodes, [{ mode: mode, path: r.nodes }]);
    });
    routesContainer.appendChild(card);
  });
  
  // if no routes shown, display placeholder
  if (!routesContainer.children.length){
    const pc = makeCard('No routes','No routes match the selected transport modes or none are available for this start/end.');
    routesContainer.appendChild(pc);
  } else {
    // auto-select and draw the first route shown (optimal)
    const firstCard = routesContainer.querySelector('.card');
    if (firstCard){ firstCard.classList.add('selected'); firstCard.click(); }
  }
  
  const busesContainer = document.getElementById('busesContainer'); busesContainer.innerHTML='';
  if (showBus){
    // Check if buses data is available
    if (!data.buses || !Array.isArray(data.buses)) {
      console.warn('No buses data available:', data.buses);
    } else {
      // First filter buses by distance (max 3x optimal distance, use optimalDistance if no pure walk routes)
      const busMaxDistance = optimalWalkingDistance > 0 ? optimalWalkingDistance * 3 : optimalDistance * 3;
      const validBuses = data.buses.filter(b => b.totalDistance <= busMaxDistance);
      
      // Sort buses by total time ascending
      validBuses.sort((a, b) => a.totalTime - b.totalTime);
      
      // Keep only top 3 bus options
      const topBuses = validBuses.slice(0, 3);
      
      // Display each bus option as a separate card with detailed steps
      topBuses.forEach((bus, busOptionIndex) => {
        const title = `Bus option ${busOptionIndex + 1}: Distance=${bus.totalDistance} m, Time=${bus.totalTime.toFixed(2)} min`;
        
        // build meta with each step on its own line including duration and note
        const metaLines = [];
        
            // Filter out zero-distance walk/bike steps for display (they represent immediate transfers)
            const displayedSteps = bus.steps.filter(st => {
              if ((st.mode === 'walk' || st.mode === 'bike') && st.distance <= 1e-6) return false;
              return true;
            });
            // Add each displayed step with detailed information
            displayedSteps.forEach((step, stepIndex) => {
              const stepHeader = `Step ${stepIndex + 1}: ${step.mode} (${step.distance.toFixed(2)} m, ${step.time.toFixed(2)} min)`;
              const stepNote = step.note ? ` - ${step.note}` : '';
              const routeInfo = ` Route: ${step.path.join('->')}`;
              metaLines.push(`${stepHeader}${stepNote}${routeInfo}`);
              if (step.segments && step.segments.length > 0) {
                step.segments.forEach(segment => {
                  metaLines.push(`  - ${escapeHtml(segment)}`);
                });
              }
              if (stepIndex < displayedSteps.length - 1) metaLines.push('');
            });
        
        const meta = metaLines.join('<br>');
        const card = makeCard(title, meta, () => {
          document.querySelectorAll('.card').forEach(c => c.classList.remove('selected'));
          card.classList.add('selected');
          // draw using displayed steps (filter out zero-distance walk/bike)
          const displayedStepsForDraw = bus.steps.filter(st => { if ((st.mode==='walk' || st.mode==='bike') && st.distance <= 1e-6) return false; return true; });
          // flatten node list but avoid duplicating consecutive endpoints
          let flat = [];
          displayedStepsForDraw.forEach((s,i)=>{ if (i===0) flat = flat.concat(s.path); else { // avoid duplicate transfer node
              if (flat.length && s.path.length && flat[flat.length-1] === s.path[0]) flat = flat.concat(s.path.slice(1)); else flat = flat.concat(s.path);
          }});
          drawRoute(flat, displayedStepsForDraw);
        });
        busesContainer.appendChild(card);
      });
    }
  }
  
  if (!busesContainer.children.length){
    const pc = makeCard('No bus options','No bus options available for the selected identity or transport filters.');
    busesContainer.appendChild(pc);
  }
  
  // add to history
  const h = document.getElementById('historyList'); 
  const li = document.createElement('li'); 
  li.textContent = `${s} -> ${t} (${new Date().toLocaleTimeString()})`;
  li.style.cursor = 'pointer'; 
  li.title = `${s} -> ${t}`;
  li.addEventListener('click', () => {
    // on click populate inputs for quick query
    document.getElementById('start').value = s;
    document.getElementById('end').value = t;
    // automatically trigger planning for this history entry
    setTimeout(() => { planRoute(); }, 50);
  });
  h.prepend(li);
  
  // Limit history to 10 items
  if (h.children.length > 10) {
    h.removeChild(h.lastChild);
  }
}

function clearInputs(){ document.getElementById('start').value=''; document.getElementById('end').value=''; document.getElementById('routesContainer').innerHTML=''; document.getElementById('busesContainer').innerHTML=''; clearOverlay(); }

window.addEventListener('load', async ()=>{
  await loadPoints();
  document.getElementById('plan').addEventListener('click', planRoute);
  document.getElementById('clear').addEventListener('click', clearInputs);
  const tbtn = document.getElementById('toggleTheme'); tbtn.addEventListener('click', ()=>{
    document.body.classList.toggle('dark'); tbtn.textContent = document.body.classList.contains('dark')? 'Light' : 'Dark';
  });
  // redraw overlay when window resizes or when image finishes loading (keeps SVG aligned)
  window.addEventListener('resize', () => { if (window.__lastDraw) { setTimeout(()=> drawRoute(window.__lastDraw.path, window.__lastDraw.steps), 50); } });
  const img = document.getElementById('mapImg'); img.addEventListener('load', () => { if (window.__lastDraw) drawRoute(window.__lastDraw.path, window.__lastDraw.steps); });
});
