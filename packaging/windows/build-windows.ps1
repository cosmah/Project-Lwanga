# Lwanga Windows Build Script
# Cosmc Technologies
# Builds Lwanga compiler for Windows using MSVC and LLVM

param(
    [string]$BuildType = "Release",
    [string]$LLVMPath = "C:\Program Files\LLVM",
    [string]$OutputDir = "build-windows"
)

$ErrorActionPreference = "Stop"

Write-Host "Building Lwanga for Windows..." -ForegroundColor Cyan
Write-Host "Build Type: $BuildType" -ForegroundColor Yellow
Write-Host "LLVM Path: $LLVMPath" -ForegroundColor Yellow
Write-Host ""

# Check for required tools
Write-Host "Checking prerequisites..." -ForegroundColor Cyan

# Check CMake
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: CMake not found. Please install CMake." -ForegroundColor Red
    exit 1
}

# Check Visual Studio
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vsWhere)) {
    Write-Host "ERROR: Visual Studio not found. Please install Visual Studio 2019 or later." -ForegroundColor Red
    exit 1
}

# Check LLVM
if (-not (Test-Path $LLVMPath)) {
    Write-Host "ERROR: LLVM not found at $LLVMPath" -ForegroundColor Red
    Write-Host "Please install LLVM from https://releases.llvm.org/" -ForegroundColor Yellow
    exit 1
}

Write-Host "All prerequisites found!" -ForegroundColor Green
Write-Host ""

# Create build directory
Write-Host "Creating build directory..." -ForegroundColor Cyan
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$BuildPath = Join-Path $ProjectRoot $OutputDir

if (Test-Path $BuildPath) {
    Write-Host "Cleaning existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BuildPath
}

New-Item -ItemType Directory -Path $BuildPath | Out-Null
Set-Location $BuildPath

# Configure with CMake
Write-Host "Configuring with CMake..." -ForegroundColor Cyan
cmake "$ProjectRoot\compiler" `
    -G "Visual Studio 17 2022" `
    -A x64 `
    -DCMAKE_BUILD_TYPE=$BuildType `
    -DLLVM_DIR="$LLVMPath\lib\cmake\llvm" `
    -DCMAKE_INSTALL_PREFIX="$BuildPath\install"

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: CMake configuration failed" -ForegroundColor Red
    exit 1
}

# Build
Write-Host ""
Write-Host "Building..." -ForegroundColor Cyan
cmake --build . --config $BuildType --parallel

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Build failed" -ForegroundColor Red
    exit 1
}

# Run tests
Write-Host ""
Write-Host "Running tests..." -ForegroundColor Cyan
ctest -C $BuildType --output-on-failure

if ($LASTEXITCODE -ne 0) {
    Write-Host "WARNING: Some tests failed" -ForegroundColor Yellow
} else {
    Write-Host "All tests passed!" -ForegroundColor Green
}

# Install
Write-Host ""
Write-Host "Installing..." -ForegroundColor Cyan
cmake --install . --config $BuildType

Write-Host ""
Write-Host "Build complete!" -ForegroundColor Green
Write-Host "Binaries location: $BuildPath\install\bin" -ForegroundColor Cyan
Write-Host ""
Write-Host "Executables:" -ForegroundColor Yellow
Write-Host "  - lwangac.exe (compiler)" -ForegroundColor White
Write-Host "  - lwangafmt.exe (formatter)" -ForegroundColor White
Write-Host "  - lwangacat.exe (syntax highlighter)" -ForegroundColor White
