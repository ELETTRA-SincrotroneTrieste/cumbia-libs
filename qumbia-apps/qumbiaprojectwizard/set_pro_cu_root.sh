#!/bin/bash

# Check if the argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <replacement_string>"
    exit 1
fi

# Assign the argument to a variable
replacement=$1
in_directory=templates.in
output_directory=qumbia_project_template

# Iterate over each *.pro.in file in the in_directory
for file in "$in_directory"/*.pro.in; do
    # Check if the file exists (to avoid issues if no files match the pattern)

    if [ -f "$file" ]; then

        # Determine the output file name by replacing .pro.in with .pro
        output_file="$output_directory/$(basename "${file%.pro.in}.pro")"

        echo ""
        echo "$file --> $output_file setting CUMBIA_ROOT"
        echo " >>> $replacement"
        echo ""
        # Use sed to replace the string and save the output to file.pro

        sed "s|%CUMBIA_ROOT%|$replacement|g" "$file" > "$output_file"

        echo "Replacement done. Saved as $output_file"

    else
        echo "No .pro.in files found in $in_directory"
    fi
done


