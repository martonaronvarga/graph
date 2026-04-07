#!/usr/bin/env bash
set -e

echo "Building Idris 2 FFI Kernel..."

export IDRIS2_CC=gcc
export IDRIS2_CFLAGS="$IDRIS2_CFLAGS -fPIC -include idris_bridge.h"

echo "Running idris2 --build..."
idris2 --build graph_orientation.ipkg

for f in build/exec/*.o; do
    objcopy --strip-debug --redefine-sym main=idris_main "$f"
done

echo "Compiling C bridge..."
gcc -c -fPIC idris_bridge.c -I.. -o idris_bridge.o

find build/exec -name "*.o" > o_files.list

echo "Compiling Idris 2 RTS (Runtime System)..."
if [ -z "$IDRIS2_SRC" ]; then
    echo "Error: IDRIS2_SRC environment variable is not set."
    echo "Make sure you added IDRIS2_SRC = \"\${idris2}\"; to your flake.nix!"
    exit 1
fi

REFC_SUPPORT="$IDRIS2_SRC/support/refc"
C_SUPPORT="$IDRIS2_SRC/support/c"

# Compile both the refc specific files AND the common C support files
for c_file in "$REFC_SUPPORT"/*.c "$C_SUPPORT"/*.c; do
    echo "  -> Compiling $(basename "$c_file")"
    # We compile the raw C files with -fPIC for our shared library
    gcc -c -fPIC "$c_file" -I"$REFC_SUPPORT" -I"$C_SUPPORT" -o "rts_$(basename "$c_file" .c).o"
    echo "rts_$(basename "$c_file" .c).o" >> o_files.list
done

echo "Linking shared library..."
gcc -shared -o libgraph-orientation-idris2.so \
    idris_bridge.o \
    $(cat o_files.list) \
    -L "$IDRIS2_SRC/libs" \
    -lgmp -lm

echo "Successfully built libgraph-orientation-idris2.so!"
