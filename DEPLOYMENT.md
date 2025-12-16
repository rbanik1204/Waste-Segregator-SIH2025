# GitHub Pages Deployment Guide

This guide explains how to deploy the PAVITRAX frontend to GitHub Pages.

## Quick Start

The repository is already configured for GitHub Pages deployment! The workflow will automatically deploy the frontend when changes are pushed to the `main` branch.

## How It Works

The repository includes:
- **GitHub Actions Workflow** (`.github/workflows/deploy.yml`) - Automatically deploys frontend to GitHub Pages
- **`.nojekyll` file** (in `frontend/`) - Ensures GitHub Pages serves all files correctly
- **Configuration System** (`frontend/config.js`) - Manages environment-specific settings

## Enabling GitHub Pages

1. Go to your repository on GitHub
2. Click **Settings** → **Pages**
3. Under **Source**, select **GitHub Actions**
4. The workflow will automatically deploy on the next push to `main`

Your site will be available at: `https://rbanik1204.github.io/Waste-Segregator-SIH2025/`

## Configuration for Production

### Backend API Configuration

By default, the frontend will try to connect to a backend API at the same origin. For GitHub Pages, you have two options:

**Option 1: Deploy Backend Separately**
Deploy your backend to a service like:
- Heroku
- Railway
- Render
- AWS/Azure/GCP

Then update `frontend/config.js`:
```javascript
API_BASE: isGitHubPages 
  ? 'https://your-backend-api.herokuapp.com'
  : location.origin,
```

**Option 2: Use Relative Paths (if backend is on same domain)**
If you set up a reverse proxy or custom domain, the default configuration will work.

### YOLO Stream Configuration

The YOLO video stream requires a separate server. Update `frontend/config.js`:

```javascript
YOLO_STREAM_URL: isGitHubPages 
  ? 'https://your-yolo-server.example.com/stream'
  : 'http://127.0.0.1:8090/stream',
```

### Hardware/ESP8266 Configuration

Hardware control (ESP8266/ESP32) will only work when the frontend is accessed on the same local network as the hardware. This is automatically disabled on GitHub Pages but can be enabled by updating `frontend/config.js`:

```javascript
FEATURES: {
  ENABLE_HARDWARE_CONTROL: true, // Enable even on GitHub Pages
  // ... other features
}
```

## Manual Deployment

If you need to deploy manually:

```bash
# Build is not needed - frontend is static HTML/CSS/JS
# Just ensure all files are in the frontend/ directory

# The GitHub Actions workflow handles deployment automatically
# But you can trigger it manually:
# Go to Actions → Deploy to GitHub Pages → Run workflow
```

## Deployment Checklist

- [x] GitHub Actions workflow exists (`.github/workflows/deploy.yml`)
- [x] `.nojekyll` file in frontend directory
- [x] Configuration file created (`frontend/config.js`)
- [x] All paths in HTML/CSS/JS are relative (no absolute localhost paths)
- [ ] Backend API deployed and URL configured (if needed)
- [ ] YOLO stream server deployed and URL configured (if needed)
- [ ] Repository Settings → Pages → Source set to "GitHub Actions"

## Testing Locally

Before deploying, test the frontend locally:

```bash
cd frontend
python -m http.server 8080
# or
npx serve
```

Then visit `http://localhost:8080` in your browser.

## Troubleshooting

### Site Not Deploying
- Check the Actions tab for workflow errors
- Ensure GitHub Pages is enabled in repository settings
- Verify the workflow has proper permissions

### 404 Errors
- Ensure all file paths are relative (e.g., `./app.js` not `/app.js`)
- Check that `.nojekyll` file exists in frontend directory

### API Errors
- Update `frontend/config.js` with your backend API URL
- Ensure CORS is configured on your backend to allow requests from GitHub Pages domain
- Check browser console for specific error messages

### Backend CORS Configuration

Your Node.js backend needs to allow requests from GitHub Pages. In `backend/server.js`, ensure CORS is configured:

```javascript
const cors = require('cors');

app.use(cors({
  origin: [
    'http://localhost:8080',
    'http://localhost:3000',
    'https://rbanik1204.github.io'
  ],
  credentials: true
}));
```

## Architecture for Production

```
┌─────────────────────────────────────┐
│   GitHub Pages (Frontend)           │
│   https://rbanik1204.github.io/     │
│   - Static HTML/CSS/JS               │
│   - No backend required for display  │
└──────────────┬──────────────────────┘
               │
               │ API Calls (if backend deployed)
               ▼
┌─────────────────────────────────────┐
│   Backend API (Optional)             │
│   - Heroku/Railway/etc.              │
│   - Node.js + Express                │
│   - WebSocket support                │
└──────────────┬──────────────────────┘
               │
               │ Data Storage
               ▼
┌─────────────────────────────────────┐
│   MongoDB Atlas (Optional)           │
│   - Cloud database                   │
│   - Telemetry storage                │
└─────────────────────────────────────┘
```

## Demo Mode

The frontend can run in "demo mode" without a backend:
- Dashboard displays sample data
- Charts show placeholder information
- Hardware controls are disabled on GitHub Pages

This is perfect for showcasing the UI/UX without infrastructure requirements.

## Next Steps

1. Enable GitHub Pages in repository settings
2. Push changes to `main` branch
3. Wait for GitHub Actions to deploy (2-3 minutes)
4. Visit your GitHub Pages URL
5. (Optional) Configure backend API endpoint in `config.js`
6. (Optional) Set up custom domain in repository settings

## Custom Domain (Optional)

To use a custom domain:

1. Add a `CNAME` file in `frontend/` directory with your domain:
   ```
   waste-segregator.example.com
   ```

2. Configure DNS records at your domain provider:
   ```
   Type: CNAME
   Name: waste-segregator (or @)
   Value: rbanik1204.github.io
   ```

3. Enable HTTPS in repository settings → Pages

## Support

For issues or questions:
- Check [GitHub Actions logs](../../actions) for deployment errors
- Review browser console for frontend errors
- See main [README.md](../README.md) for general setup
- Contact team members listed in [team_info.txt](../team_info.txt)
