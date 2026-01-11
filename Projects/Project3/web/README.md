# Web UI for Project3

This folder contains a small Express backend and a static frontend that calls the C++ binary to compute routes.

Setup

1. Make sure `build/project3.exe` exists (build the C++ project with CMake).
2. Copy `data/map.png` and `data/pointPixel.csv` into the `data/` folder at the project root (they are expected at `../data/...` by the web files).
3. Install Node dependencies and run the server:

```powershell
cd web
npm install
npm start
```

4. Open `http://localhost:3001` in your browser.

Notes

- The server calls the compiled `project3.exe` with `--api` arguments. I added a JSON output mode to `main.cpp`.
- The frontend renders the PNG map and overlays SVG lines scaled to the image size. Bus segments are drawn in a different color.
- If you want HTTPS or to run on another port, change `web/server.js` accordingly.
