// app.js — PAVITRAX UI logic
const API_BASE = location.origin.includes('3000')
  ? location.origin.replace('3000', '4000')
  : location.origin;

let map, marker, heatLayer, stabilityChart, wasteChart, tdsTrend, tempTrend, aqiTrend, correlationChart;
let socket;
const stabilityData = { labels: [], x: [], y: [], z: [] };

// Navigation
function setActiveSection(target) {
  document.querySelectorAll('nav button').forEach((btn) => {
    btn.classList.toggle('active', btn.dataset.target === target);
  });
  document.querySelectorAll('main section').forEach((sec) => {
    sec.classList.toggle('active', sec.id === target);
  });
}

function initNav() {
  document.getElementById('nav').addEventListener('click', (e) => {
    if (e.target.tagName === 'BUTTON') setActiveSection(e.target.dataset.target);
  });
}

// Telemetry handling
function updateCards(data) {
  const loc = data.gps?.lat ? `${data.gps.lat.toFixed(5)}, ${data.gps.lon?.toFixed(5)}` : '--';
  setText('boat-location', loc);
  setText('boat-heading', `Heading: ${data.heading ?? '--'}°`);
  setText('boat-speed', `${data.speed ?? '--'} m/s`);
  setText('ultrasonic', `${data.ultrasonic_cm ?? '--'} cm`);
  setText('battery', `${data.battery_volt ?? '--'} V`);
  setText('tds-value', `${data.tds_ppm ?? data.tds ?? '--'} ppm`);
  setText('temp', `${data.temperature_c ?? '--'} °C`);
  setText('mq135-value', `${data.mq135_ppm ?? data.gas?.mq135 ?? '--'} ppm`);
  setText('mq2-value', `${data.mq2_ppm ?? data.gas?.mq2 ?? '--'} ppm`);
  setText('moisture-dry', `${data.moisture?.dry_bin ?? '--'}%`);
  setText('moisture-wet', `${data.moisture?.wet_bin ?? '--'}%`);
  setText('weight', `${data.loadcell_grams ?? '--'} g`);
  setText('conveyors', `Wet: ${data.conveyor?.wet_active ? 'ON' : 'OFF'} | Dry: ${data.conveyor?.dry_active ? 'ON' : 'OFF'}`);
  setText('propellers', `Status: ${data.propeller?.active ? 'Active' : 'Idle'}`);
  setText('mode', data.mode?.autonomous ? 'Autonomous' : 'Manual');
  setText('image-status', data.waste?.type ? 'Processing Images' : 'Idle');
  setText('last-update', `Last update: ${new Date().toLocaleTimeString()}`);

  // Pills
  setPill('battery-pill', (data.battery_volt || 0) < 10.8 ? 'Low battery' : 'Healthy', (data.battery_volt || 0) < 10.8 ? 'danger' : 'success');
  setPill('bin-pill', (data.loadcell_grams || 0) > 4500 ? 'Bin full' : 'Capacity ok', (data.loadcell_grams || 0) > 4500 ? 'danger' : 'success');
  setPill('obstacle-pill', (data.ultrasonic_cm || 999) < 25 ? 'Obstacle' : 'Clear', (data.ultrasonic_cm || 999) < 25 ? 'danger' : 'success');

  // Hazard detection
  if (data.waste?.type && String(data.waste.type).toLowerCase().includes('hazard')) {
    setText('latest-hazard', `Hazardous: ${data.waste.type} (${(data.waste.confidence || 0).toFixed(2)})`);
    setPill('hazard-flag', 'Hazard detected', 'danger');
  } else if (data.waste?.type) {
    setText('latest-hazard', `${data.waste.type} (${(data.waste.confidence || 0).toFixed(2)})`);
    setPill('hazard-flag', 'Monitoring', 'warn');
  } else {
    setText('latest-hazard', 'No detection');
    setPill('hazard-flag', 'Monitoring', 'warn');
  }

  // Water health logic
  const tds = data.tds_ppm ?? data.tds;
  if (tds !== undefined) {
    const status = waterStatus(tds, data.temperature_c);
    setText('water-health-status', status.label);
    setText('water-health-detail', status.detail);
    setText('pollution-zone', status.zone);
    setPill('tds-pill', status.label, status.pill);
    setText('algal-risk', status.algal);
    setText('sewage-risk', status.sewage);
    setText('plastic-zone', status.plastic);
    setText('chemical-zone', status.chemical);
  }

  // Stability chart
  updateStability(data.accelerometer || {});

  // Map
  if (data.gps?.lat && marker) {
    marker.setLatLng([data.gps.lat, data.gps.lon]);
    map.setView([data.gps.lat, data.gps.lon]);
  }
}

function waterStatus(tds, temp) {
  let label = 'Good', detail = 'Acceptable water quality', pill = 'success', zone = 'Normal Zone';
  let algal = 'Low', sewage = 'Low', plastic = 'No', chemical = 'No';
  if (tds < 50) { label = 'Excellent'; detail = 'Very clean; possible plastic accumulation'; zone = 'Plastic Zone (Low TDS)'; plastic = 'Likely'; }
  else if (tds < 200) { label = 'Good'; }
  else if (tds < 500) { label = 'Fair'; detail = 'Moderate dissolved solids'; pill = 'warn'; zone = 'Moderate Pollution'; }
  else if (tds < 1000) { label = 'Poor'; detail = 'High TDS - possible sewage/industrial'; pill = 'danger'; zone = 'Chemical Zone'; sewage = 'High'; chemical = 'Detected'; }
  else { label = 'Critical'; detail = 'Severe pollution'; pill = 'danger'; zone = 'Critical'; sewage = 'Critical'; chemical = 'Critical'; }
  if (tds > 500 && (temp || 0) > 25) algal = 'High';
  else if (tds > 300 && (temp || 0) > 22) algal = 'Moderate';
  return { label, detail, pill, zone, algal, sewage, plastic, chemical };
}

// UI helpers
function setText(id, text) {
  const el = document.getElementById(id);
  if (el) el.innerText = text;
}

function setPill(id, text, style) {
  const el = document.getElementById(id);
  if (!el) return;
  el.innerText = text;
  el.className = `pill ${style || ''}`;
}

// Charts
function initCharts() {
  stabilityChart = new Chart(document.getElementById('stabilityChart'), {
    type: 'line',
    data: { labels: [], datasets: [
      { label: 'X', data: [], borderColor: '#2563eb', tension: 0.3 },
      { label: 'Y', data: [], borderColor: '#10b981', tension: 0.3 },
      { label: 'Z', data: [], borderColor: '#f59e0b', tension: 0.3 }
    ]},
    options: { plugins: { legend: { display: true } }, scales: { y: { suggestedMin: -2, suggestedMax: 2 } } }
  });

  wasteChart = new Chart(document.getElementById('wasteChart'), {
    type: 'bar',
    data: { labels: [], datasets: [
      { label: 'Dry', data: [], backgroundColor: '#2563eb' },
      { label: 'Wet', data: [], backgroundColor: '#10b981' }
    ]},
    options: { responsive: true, plugins: { legend: { position: 'bottom' } } }
  });

  tdsTrend = new Chart(document.getElementById('tdsTrend'), {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'TDS (ppm)', data: [], borderColor: '#2563eb', tension: 0.3 }]},
    options: { plugins: { legend: { display: false } } }
  });

  tempTrend = new Chart(document.getElementById('tempTrend'), {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'Water Temp (°C)', data: [], borderColor: '#f97316', tension: 0.3 }]},
    options: { plugins: { legend: { display: false } } }
  });

  aqiTrend = new Chart(document.getElementById('aqiTrend'), {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'Air Quality (MQ135 ppm)', data: [], borderColor: '#22c55e', tension: 0.3 }]},
    options: { plugins: { legend: { display: false } } }
  });

  correlationChart = new Chart(document.getElementById('correlationChart'), {
    type: 'bar',
    data: { labels: [], datasets: [
      { label: 'TDS Level', data: [], backgroundColor: '#2563eb' },
      { label: 'Waste Collected', data: [], backgroundColor: '#10b981' }
    ]},
    options: { responsive: true, scales: { y: { beginAtZero: true } } }
  });
}

function updateStability(acc) {
  const ts = new Date().toLocaleTimeString();
  stabilityData.labels.push(ts);
  stabilityData.x.push(acc?.x ?? 0);
  stabilityData.y.push(acc?.y ?? 0);
  stabilityData.z.push(acc?.z ?? 0);
  if (stabilityData.labels.length > 30) {
    stabilityData.labels.shift(); stabilityData.x.shift(); stabilityData.y.shift(); stabilityData.z.shift();
  }
  stabilityChart.data.labels = stabilityData.labels;
  stabilityChart.data.datasets[0].data = stabilityData.x;
  stabilityChart.data.datasets[1].data = stabilityData.y;
  stabilityChart.data.datasets[2].data = stabilityData.z;
  stabilityChart.update('none');
}

// Data loading
async function fetchJson(path) {
  const res = await fetch(`${API_BASE}${path}`);
  return res.json();
}

async function loadClassification() {
  const data = await fetchJson('/api/analytics/classification');
  const tbody = document.querySelector('#classification-table tbody');
  if (tbody) tbody.innerHTML = '';
  const counts = { dry: 0, wet: 0, hazardous: 0, metal: 0 };
  (data.data || []).forEach(row => {
    const tr = document.createElement('tr');
    tr.innerHTML = `<td>${row._id || 'unknown'}</td><td>${row.count}</td><td>${(row.avgConfidence||0).toFixed(2)}</td>`;
    tbody?.appendChild(tr);
    const t = (row._id || '').toLowerCase();
    if (t.includes('wet')) counts.wet = row.count;
    else if (t.includes('hazard')) counts.hazardous = row.count;
    else if (t.includes('metal') || t.includes('plastic')) counts.metal = row.count;
    else counts.dry += row.count;
  });
  setText('stat-dry', counts.dry);
  setText('stat-wet', counts.wet);
  setText('stat-hazard', counts.hazardous);
  setText('stat-metal', counts.metal);
}

async function loadWeekly() {
  const data = await fetchJson('/api/analytics/weekly');
  const labels = data.data.map(d => d._id.day);
  const wet = data.data.map(d => d.wet || 0);
  const dry = data.data.map(d => d.dry || 0);
  const haz = data.data.map(d => d.hazardous || 0);
  wasteChart.data.labels = labels;
  wasteChart.data.datasets[0].data = dry;
  wasteChart.data.datasets[1].data = wet;
  wasteChart.update('none');
  tdsTrend.data.labels = labels;
  tdsTrend.data.datasets[0].data = data.data.map(d => d.avgGas || 0);
  tdsTrend.update('none');
  tempTrend.data.labels = labels;
  tempTrend.data.datasets[0].data = data.data.map(d => d.avgTemp || 0);
  tempTrend.update('none');
}

async function loadHeatmap() {
  const data = await fetchJson('/api/analytics/heatmap');
  if (!map) return;
  if (!heatLayer) {
    heatLayer = L.heatLayer(data.data || [], { radius: 25 }).addTo(map);
  } else {
    heatLayer.setLatLngs(data.data || []);
  }
}

async function loadAlerts() {
  const data = await fetchJson('/api/alerts');
  const container = document.getElementById('alerts');
  if (!container) return;
  container.innerHTML = '';
  (data.data || []).forEach(al => {
    const div = document.createElement('div');
    div.className = 'alert';
    div.innerHTML = `<strong>${al.type}</strong> • ${al.message} <div class="subtitle">${new Date(al.createdAt).toLocaleString()}</div>`;
    container.appendChild(div);
  });
}

async function loadCorrelation() {
  // reuse weekly data for simple correlation
  const data = await fetchJson('/api/analytics/weekly');
  correlationChart.data.labels = data.data.map(d => d._id.day);
  correlationChart.data.datasets[0].data = data.data.map(d => d.avgGas || 0);
  correlationChart.data.datasets[1].data = data.data.map(d => (d.totalWeight || 0) / 1000);
  correlationChart.update('none');
}

// Map
function initMap() {
  map = L.map('map').setView([22.5726, 88.3639], 13);
  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', { maxZoom: 19 }).addTo(map);
  marker = L.marker([22.5726, 88.3639]).addTo(map);
}

// Commands
function sendCommand(cmd) {
  fetch(`${API_BASE}/api/control/send`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(cmd)
  }).catch(console.error);
  if (socket) socket.emit('command', cmd);
}

function bindControls() {
  document.querySelectorAll('[data-cmd]').forEach(btn => {
    btn.addEventListener('click', () => {
      const payload = btn.dataset.cmd;
      if (payload === 'forward' || payload === 'left' || payload === 'right' || payload === 'stop') sendCommand({ action: payload });
      else if (payload === 'wet_on' || payload === 'wet_off') sendCommand({ belt: payload.replace('_', '') });
      else if (payload === 'dry_on' || payload === 'dry_off') sendCommand({ belt: payload.replace('_', '') });
      else if (payload === 'auto_toggle') sendCommand({ mode: 'autonomous_toggle' });
      else if (payload === 'nav_toggle') sendCommand({ mode: 'navigation_toggle' });
    });
  });
}

function downloadFile(path) {
  window.open(`${API_BASE}${path}`, '_blank');
}

// Bootstrap
async function bootstrap() {
  initNav();
  initCharts();
  initMap();
  bindControls();

  await loadClassification();
  await loadWeekly();
  await loadHeatmap();
  await loadAlerts();
  await loadCorrelation();

  socket = io(API_BASE, { transports: ['websocket'] });
  socket.on('telemetry', (payload) => {
    const d = payload.data || {};
    updateCards(d);
  });

  // initial pull
  const latest = await fetchJson('/api/sensors/latest?n=1');
  if (latest.data && latest.data[0]) updateCards(latest.data[0].data || {});

  setInterval(loadClassification, 15000);
  setInterval(loadWeekly, 60000);
  setInterval(loadHeatmap, 20000);
  setInterval(loadAlerts, 30000);
}

document.addEventListener('DOMContentLoaded', bootstrap);

