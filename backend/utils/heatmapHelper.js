// heatmapHelper.js
// small helper to aggregate telemetry into lat/lon heatmap points.
// For demo: expects telemetry entries with data.latitude and data.longitude and a weight property.
function aggregateForHeatmap(store, options = {}) {
    // naive: return array [ [lat, lon, weight], ... ]
    return store
      .filter(s => s.data && s.data.latitude && s.data.longitude)
      .map(s => [ parseFloat(s.data.latitude), parseFloat(s.data.longitude), s.data.weight || 1 ]);
  }
  
  module.exports = { aggregateForHeatmap };
  