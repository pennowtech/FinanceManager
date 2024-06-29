#!/bin/sh

license_header_cpp=$(cat << 'EOF'
/*----------------------------------------------------------------------
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ----------------------------------------------------------------------*/
EOF
)

license_header_py=$(cat << 'EOF'
# ----------------------------------------------------------------------
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
# ----------------------------------------------------------------------
EOF
)

license_check_fail=0

# Function to prepend license to files
add_license() {
    file=$1
    license_header="$2"
    temp_file="$(mktemp)"
    echo "$license_header" > "$temp_file"
    cat "$file" >> "$temp_file"
    mv "$temp_file" "$file"
    grep -qF "$license_header_cpp" "$file"
    return $?
}

# Function to check license in files
check_and_add_cpp_license() {
    # files="Utils/include/Utils/Logger.h Utils/include/Utils/EnvReader.h"
    # for file in $files; do
    for file in $(git diff --cached --name-only | grep -E '\.(c|h|cpp|hpp)$'); do
         echo "Checking license header in: $file."
        if ! grep -qF "$license_header_cpp" "$file"; then
            echo "License header missing in: $file."
            if add_license "$file" "$license_header_cpp"; then
                echo "License header added!"
                git add "$file"
            else
                echo "License header can't be added"
                license_check_fail=1
            fi
        fi
    done
}
check_and_add_py_license() {
    for file in $(git diff --cached --name-only | grep -E '\.(py|txt)$'); do
         echo "Checking license header in: $file."
        if ! grep -qF "$license_header_py" "$file"; then
            echo "License header missing in: $file."
            if add_license "$file" "$license_header_py"; then
                echo "License header added!"
                git add "$file"
            else
                echo "License header can't be added"
                license_check_fail=1
            fi
        fi
    done
}

# Run the license header check
check_and_add_cpp_license
check_and_add_py_license

# Exit script based on license check
if [ $license_check_fail -ne 0 ]; then
    exit 1
fi

exit 0
