# pacPrism Fast Build Script
# Removes existing build directory, configures, builds and runs the application

# Remove existing build directory
if (Test-Path "build") {
    Remove-Item -Recurse -Force "build"
    Write-Host "Removed existing build directory" -ForegroundColor Green
}

# Configure with CMake
Write-Host "Configuring project..." -ForegroundColor Yellow
cmake -B build
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

# Build the project
Write-Host "Building project..." -ForegroundColor Yellow
cmake --build build
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

# Run the application
Write-Host "Running pacPrism..." -ForegroundColor Cyan
./build/bin/pacprism.exe