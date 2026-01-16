# Set the absolute path.
$ABS_SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Definition
$ABS_SCRIPT_DIR = Resolve-Path $ABS_SCRIPT_DIR

# Set PROJ_ROOT
$PROJ_ROOT = Split-Path -Parent $ABS_SCRIPT_DIR
$PROJ_ROOT = Resolve-Path $PROJ_ROOT

# Windows only: vswhere.exe
Write-Host "Checking for Visual Studio C++ build tools..."
try {
    $VS_INFO = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Product.BuildTools -property installationPath 2>$null
    if (-not $VS_INFO) {
        Write-Host "Error: Visual Studio with C++ tools not found. Please install Visual Studio with C++ development workload." -ForegroundColor Red
        exit 1
    }
    Write-Host "Visual Studio C++ build tools found."
} catch {
    Write-Host "Error: Visual Studio with C++ tools not found. Please install Visual Studio with C++ development workload." -ForegroundColor Red
    exit 1
}

# Check the integrity.
if (-not $env:VCPKG_ROOT) {
    Write-Host "VCPKG_ROOT environment variable not set. Please set it to your vcpkg installation path:" -ForegroundColor Red
    Write-Host "`$env:VCPKG_ROOT='C:/vcpkg'" -ForegroundColor Red
    Write-Host "To make it permanent, add to System Environment Variables" -ForegroundColor Red
    exit 1
}

$VCPKG_ROOT = $env:VCPKG_ROOT
if (Test-Path $VCPKG_ROOT) {
    $VCPKG_ROOT = Resolve-Path $VCPKG_ROOT
    Write-Host "VCPKG_ROOT: $VCPKG_ROOT"
} else {
    Write-Host "VCPKG_ROOT path does not exist: $VCPKG_ROOT" -ForegroundColor Red
    Write-Host "Please install vcpkg first:" -ForegroundColor Red
    Write-Host "git clone https://github.com/Microsoft/vcpkg.git C:/vcpkg" -ForegroundColor Red
    Write-Host "cd C:/vcpkg" -ForegroundColor Red
    Write-Host ".\bootstrap-vcpkg.bat" -ForegroundColor Red
    exit 1
}

# Find vcpkg. If no vcpkg, install it.
Write-Host "Finding vcpkg..."
$VCPKG_EXE = Join-Path $VCPKG_ROOT "vcpkg.exe"
if (Test-Path $VCPKG_EXE) {
    Write-Host "Vcpkg found."
} else {
    Write-Host "Vcpkg not found. Bootstrapping..."

    $BOOTSTRAP_SCRIPT = Join-Path $VCPKG_ROOT "bootstrap-vcpkg.bat"
    if (Test-Path $BOOTSTRAP_SCRIPT) {
        $process = Start-Process -FilePath $BOOTSTRAP_SCRIPT -WorkingDirectory $VCPKG_ROOT -Wait -PassThru
        if ($process.ExitCode -eq 0) {
            Write-Host "Bootstrapping successfully ended."
        } else {
            Write-Host "Bootstrapping failed." -ForegroundColor Red
            exit 1
        }
    } else {
        Write-Host "Bootstrap script not found: $BOOTSTRAP_SCRIPT" -ForegroundColor Red
        exit 1
    }
}

# Install dependencies.
Write-Host "Running vcpkg install..."
$process = Start-Process -FilePath $VCPKG_EXE -ArgumentList "install" -WorkingDirectory $PROJ_ROOT -Wait -PassThru -NoNewWindow
if ($process.ExitCode -eq 0) {
    Write-Host "Dependencies installing successfully ended."
} else {
    Write-Host "Dependencies installing failed." -ForegroundColor Red
    exit 1
}

# Run cmake -B build.
Write-Host "Running cmake -B build..."
$CMAKE_TOOLCHAIN_FILE = Join-Path $VCPKG_ROOT "scripts\buildsystems\vcpkg.cmake"
$process = Start-Process -FilePath "cmake" -ArgumentList "-B", "build", "-DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE" -WorkingDirectory $PROJ_ROOT -Wait -PassThru -NoNewWindow
if ($process.ExitCode -eq 0) {
    Write-Host "Running cmake -B build successfully ended."
} else {
    Write-Host "Running cmake -B build failed." -ForegroundColor Red
    exit 1
}

# Build.
Write-Host "Running cmake --build build..."
$process = Start-Process -FilePath "cmake" -ArgumentList "--build", "build" -WorkingDirectory $PROJ_ROOT -Wait -PassThru -NoNewWindow
if ($process.ExitCode -eq 0) {
    Write-Host "The project has been built successfully."
} else {
    Write-Host "Running cmake --build build failed." -ForegroundColor Red
    exit 1
}