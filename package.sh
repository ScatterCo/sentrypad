#!/usr/bin/env bash
set -eux

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

OUT_DIR="$SCRIPT_DIR/out"
rm -rf "$OUT_DIR"
mkdir $OUT_DIR

### Sentrypad
SENTRYPAD_REMOTE="https://github.com/getsentry/sentrypad/"
SENTRYPAD_REVISION=$(git rev-parse ${1:-HEAD})
SENTRYPAD_IN_DIR="$SCRIPT_DIR/.sentrypad-tmp"
if [ -d "$SENTRYPAD_IN_DIR" ]; then
    cd "$SENTRYPAD_IN_DIR"
    git fetch origin
    git checkout -f "$SENTRYPAD_REVISION"
else
    git clone "$SENTRYPAD_REMOTE" "$SENTRYPAD_IN_DIR"
    cd "$SENTRYPAD_IN_DIR"
    git checkout -f "$SENTRYPAD_REVISION"
fi

SENTRYPAD_OUT_DIR="$OUT_DIR"
SENTRYPAD_SRC=("example.c" "include" "src" "premake" "README.md")

mkdir -p $SENTRYPAD_OUT_DIR

# Copy files
for f in "${SENTRYPAD_SRC[@]}"; do
    cp -r "$SENTRYPAD_IN_DIR/$f" "$SENTRYPAD_OUT_DIR/"
done


### Crashpad
# CRASHPAD_REMOTE="git@github.com:getsentry/crashpad.git"
# CRASHPAD_REVISION="origin/getsentry"
CRASHPAD_OUT_DIR="$OUT_DIR/crashpad"

# FIXME this should be a clean sentrypad checkout
CRASHPAD_IN_DIR="$SCRIPT_DIR/crashpad"
CRASHPAD_COPY_SRC=("examples" "fetch_crashpad.sh" "vars.sh")

# Copy files
mkdir -p "$CRASHPAD_OUT_DIR"
for f in "${CRASHPAD_COPY_SRC[@]}"; do
    cp -r "$CRASHPAD_IN_DIR/$f" "$CRASHPAD_OUT_DIR/"
done

# Fetch crashpad and its dependencies
bash "$CRASHPAD_OUT_DIR/fetch_crashpad.sh"
# Clean up unneeded files
rm -rf $CRASHPAD_OUT_DIR/build/{depot_tools,buildtools} $CRASHPAD_OUT_DIR/build/crashpad/third_party/{gtest,gyp}
