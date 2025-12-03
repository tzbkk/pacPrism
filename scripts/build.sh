# Set the absolute path.
ABS_SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

# Set PROJ_ROOT
PROJ_ROOT=$(cd "$ABS_SCRIPT_DIR/.." && pwd)

# Check the integrity.
if [[ -d "$PROJ_ROOT/vcpkg" ]]; then
    VCPKG_ROOT=$(cd "$PROJ_ROOT/vcpkg" && pwd)
    echo "VCPKG_ROOT: $VCPKG_ROOT"
else
    echo "VCPKG_ROOT not found. Please clone the project again." 1>&2
    exit 1
fi

# Find vcpkg. If no vcpkg, install it.
echo "Finding vcpkg..."
if [[ -f "$VCPKG_ROOT/vcpkg" ]]; then
    echo "Vcpkg found."
else
    echo "Vcpkg not found. Bootstraping..."

    if bash "$VCPKG_ROOT/bootstrap-vcpkg.sh"; then
        echo "Bootstraping successfully ended."
    else
        echo "Bootstraping failed." >&2
        exit 1
    fi
fi

# Install dependencies.
echo "Running vcpkg install..."
if "$VCPKG_ROOT/vcpkg" install; then
    echo "Dependencies installing successfully ended."
else
    echo "Dependencies installing failed." >&2
    exit 1
fi

# Run cmake -B build.
echo "Running cmake -B build..."
if cmake -B build -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"; then
    echo "Running cmake -B build successfully ended."
else
    echo "Running cmake -B build failed." >&2
    exit 1;
fi

# Build.
echo "Running cmake --build build..."
if cmake --build build; then
    echo "The project has been built successfully."
else
    echo "Running cmake --build build failed." >&2
    exit 1;
fi