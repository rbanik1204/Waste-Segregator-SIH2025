# How to Enable GitHub Pages for This Repository

Follow these steps to deploy your PAVITRAX frontend to GitHub Pages:

## Step-by-Step Instructions

### 1. Navigate to Repository Settings
1. Go to https://github.com/rbanik1204/Waste-Segregator-SIH2025
2. Click on the **Settings** tab (top navigation bar)

### 2. Access GitHub Pages Settings
1. In the left sidebar, scroll down to **Code and automation** section
2. Click on **Pages**

### 3. Configure GitHub Pages
1. Under **Build and deployment**:
   - **Source**: Select **GitHub Actions** from the dropdown
   - (This tells GitHub to use the workflow file at `.github/workflows/deploy.yml`)

2. Click **Save** (if a button appears)

### 4. Trigger Deployment

The deployment will automatically trigger when:
- You push changes to the `main` branch, OR
- You manually trigger the workflow

**To manually trigger the workflow now:**
1. Go to the **Actions** tab in your repository
2. Click on **Deploy to GitHub Pages** in the left sidebar
3. Click **Run workflow** button (on the right)
4. Select `main` branch
5. Click the green **Run workflow** button

### 5. Monitor Deployment
1. Go to the **Actions** tab
2. You'll see the workflow running (yellow dot)
3. Click on the workflow to see progress
4. Wait for it to complete (green checkmark)
5. This usually takes 1-3 minutes

### 6. Access Your Site
Once deployed, your site will be available at:

**https://rbanik1204.github.io/Waste-Segregator-SIH2025/**

The URL will also be shown in:
- Settings → Pages → "Your site is live at..."
- The Actions workflow output

## Verification Steps

After deployment completes:

1. ✅ Visit your GitHub Pages URL
2. ✅ Verify the dashboard loads
3. ✅ Check browser console for errors (F12)
4. ✅ Test navigation between sections

## Troubleshooting

### Deployment Failed
- Check the Actions tab for error messages
- Ensure the workflow file exists at `.github/workflows/deploy.yml`
- Verify you have proper permissions on the repository

### Page Shows 404
- Wait a few minutes - it can take 2-5 minutes for changes to appear
- Clear your browser cache (Ctrl+Shift+R or Cmd+Shift+R)
- Verify the deployment completed successfully in Actions tab

### Assets Not Loading (CSS/JS)
- Ensure `.nojekyll` file exists in `frontend/` directory
- Check that all paths in HTML are relative (e.g., `./app.css` not `/app.css`)
- View browser console for specific errors

### Backend API Errors
This is normal! The frontend will try to connect to a backend API, but on GitHub Pages:
- Backend is not deployed (only frontend/static files)
- You'll need to deploy the backend separately to Heroku/Railway/etc.
- Update `frontend/config.js` with your backend URL

See [DEPLOYMENT.md](DEPLOYMENT.md) for full backend deployment options.

## What Gets Deployed

The GitHub Actions workflow deploys **only the frontend directory**:
- `frontend/index.html`
- `frontend/app.js`
- `frontend/app.css`
- `frontend/config.js`
- `frontend/.nojekyll`

The backend, AI models, and Python code are **not deployed** to GitHub Pages (they need separate hosting).

## Next Steps After Deployment

1. ✅ Share your live URL with team members
2. 📝 Update `config.js` if you deploy a backend API
3. 🎨 Customize the site further if needed
4. 🔄 Changes to `main` branch will auto-deploy

## Need Help?

- 📖 Full deployment guide: [DEPLOYMENT.md](DEPLOYMENT.md)
- 🏠 Main README: [README.md](README.md)
- 👥 Team contact: [team_info.txt](team_info.txt)
- 🐛 GitHub Actions logs: [Actions tab](../../actions)

---

**Note:** GitHub Pages is free for public repositories and perfect for hosting the frontend UI. For the full system (backend + AI + database), you'll need additional hosting services.
