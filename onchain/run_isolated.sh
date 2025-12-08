#!/bin/bash
set -e

# Backup Anchor.toml
cp Anchor.toml Anchor.toml.bak

cleanup() {
    mv Anchor.toml.bak Anchor.toml
}
trap cleanup EXIT

# Function to run test for a specific file
run_test() {
    local file=$1
    echo "Running test for $file..."
    
    # Update Anchor.toml to run only this file
    # We use a temporary file for sed to avoid issues with in-place editing on different platforms
    sed "s|tests/\*\*/\*\.ts|$file|g" Anchor.toml.bak > Anchor.toml
    
    # Run anchor test
    # We use --skip-build to save time since we built once (or will build on first run)
    if [ "$file" == "tests/deposit.ts" ]; then
        anchor test
    else
        anchor test --skip-build
    fi
}

# List of test files
FILES=(
    "tests/deposit.ts"
    "tests/withdraw.ts"
    "tests/internalTransfer.ts"
    "tests/externalTransfer.ts"
    "tests/onchain.ts"
)

for file in "${FILES[@]}"; do
    run_test "$file"
done
