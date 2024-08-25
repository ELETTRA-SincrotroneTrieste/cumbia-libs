#!/bin/bash

# Check if the argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <replacement_string>"
    exit 1
fi

# Assign the argument to a variable
replacement=$1

# Iterate over each *.pro.in file in the directory
for file in "qumbia_project_template/*.pro.in; do
    # Check if the file exists (to avoid issues if no files match the pattern)
    if [ -f "$file" ]; then

        # Determine the output file name by replacing .pro.in with .pro
        output_file="${file%.pro.in}.pro"

        echo "Processing file: $file"
        # Use sed to replace the string and save the output to file.pro
        sed "s|%CUMBIA_ROOT%|$replacement|g" "$file" > "$output_file"
        # You can perform any operation on the file here
    else
        echo "No .pro.in files found in $directory"
    fi
done


echo "Replacement done. Saved as file.pro"
