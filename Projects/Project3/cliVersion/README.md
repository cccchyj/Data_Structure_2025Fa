# CLI Version

This folder provides simple helpers to run the compiled backend from the command line on Windows.

Usage

- Preferred: run the provided Node wrapper which will locate the backend executable and attach to its stdio:

  ```powershell
  node cli.js
  ```

- Or run the PowerShell helper (from this folder):

  ```powershell
  .\run.ps1
  ```

- Or the CMD helper:

  ```cmd
  run.cmd
  ```

Notes

- The wrapper looks for the backend executable in these locations (relative to the project root):
  - `build\project3.exe`
  - `test_bus_plans.exe`
  - `backend.exe`

- If none is found the wrapper prints an explanation.

- The backend already supports an interactive CLI; this folder simply helps locate and run it from the project root on Windows.
