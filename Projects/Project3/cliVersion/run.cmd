@echo off
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..\
if exist "%PROJECT_ROOT%build\project3.exe" (
  "%PROJECT_ROOT%build\project3.exe"
  exit /b %errorlevel%
)
if exist "%PROJECT_ROOT%test_bus_plans.exe" (
  "%PROJECT_ROOT%test_bus_plans.exe"
  exit /b %errorlevel%
)
echo No executable found. Checked:
echo   %PROJECT_ROOT%build\project3.exe
echo   %PROJECT_ROOT%test_bus_plans.exe
echo   %PROJECT_ROOT%backend.exe
echo Build the project first from project root:
echo   cmake -S . -B build ; cmake --build build --config Release
exit /b 1
