#!/bin/bash
# Build vendor libraries into persistent cache (survives "rm -rf build/")
# Usage: ./tools/build_vendor_libs.sh [debug|release]
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

CONFIG="${1:-debug}"
case "$CONFIG" in
    debug)   BUILD_TYPE=Debug   ;;
    release) BUILD_TYPE=Release ;;
    *)       echo "Usage: $0 [debug|release]"; exit 1 ;;
esac

BUILD_DIR="$ROOT_DIR/build-vendor-only-$CONFIG"
CACHE_DIR="$ROOT_DIR/build-vendor-cache/$BUILD_TYPE"

echo "=== Building vendor libs ($BUILD_TYPE) ==="
echo "    Build dir: $BUILD_DIR"
echo "    Cache dir: $CACHE_DIR"
echo ""

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure full project (fast — just cmake configure)
cmake "$ROOT_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DBUGL_VENDOR_CACHE=ON \
    -DBUGL_VENDOR_CACHE_DIR="$CACHE_DIR"

# Build only vendor targets (not main, not libbu, not tests)
TARGETS=(imgui miniz poly2tri raylib bugl_audio)

# Optional targets — only build if configured
for t in box2d Jolt assimp meshoptimizer libopensteer Recast Detour DetourCrowd DetourTileCache; do
    if ninja -t query "$t" &>/dev/null; then
        TARGETS+=("$t")
    fi
done

echo ""
echo "=== Building targets: ${TARGETS[*]} ==="
ninja "${TARGETS[@]}"

echo ""
echo "=== Vendor libs cached in: $CACHE_DIR ==="
find "$CACHE_DIR" -name "*.a" -printf "  %p\n" | sort
echo ""
echo "Now build your project normally:"
echo "  cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE"
echo "  cmake --build build"
echo ""
echo "When you 'rm -rf build/', vendor libs in $CACHE_DIR survive!"
