# PowerShell helper to run the backend executable from the project root
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Resolve-Path (Join-Path $scriptDir "..")
$candidates = @(Join-Path $projectRoot "build\project3.exe", Join-Path $projectRoot "test_bus_plans.exe", Join-Path $projectRoot "backend.exe")
$exe = $null
foreach ($c in $candidates){ if (Test-Path $c){ $exe = $c; break } }
if (-not $exe){
    Write-Error "No executable found. Checked:`n $($candidates -join "`n ")"
    Write-Host "Build the project first from project root:" -ForegroundColor Yellow
    Write-Host "  cmake -S . -B build; cmake --build build --config Release" -ForegroundColor Yellow
    exit 1
}
Write-Host "Running: $exe"
& $exe
