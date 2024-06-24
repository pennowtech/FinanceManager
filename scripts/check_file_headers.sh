#!/bin/sh

LICENSE_FILE="license_header.txt"
LICENSE_HEADER=$(cat "$LICENSE_FILE")

# Function to prepend license to files
add_license() {
    file=$1
    temp_file="$(mktemp)"
    echo "$LICENSE_HEADER" > "$temp_file"
    cat "$file" >> "$temp_file"
    mv "$temp_file" "$file"
    git add "$file"
}

# Function to check license in files
check_and_add_license() {
    for file in $(git diff --cached --name-only | rg '\.(c|h|cpp|hpp|py)$'); do
        # Check if the file contains the license header
        if ! grep -q "$LICENSE_HEADER" "$file"; then
            echo "License header missing in: $file. Adding license."
            add_license "$file"
            license_check_fail=1
        fi
    done
}

# Run the license header check
check_and_add_license

# Exit script based on license check
if [ "$license_check_fail" -ne 0 ]; then
    echo "Commit failed: some files are missing the license header."
    exit 1
fi

exit 0
