#!/bin/bash

# Script to generate MD5 checksums for files in para and lib directories

# Set the directory paths
PARA_DIR="para"
LIB_DIR="lib"
PARA_OUTPUT_FILE="$PARA_DIR/md5sums.txt"
LIB_OUTPUT_FILE="$LIB_DIR/md5sums.txt"

# Check if para directory exists
if [ ! -d "$PARA_DIR" ]; then
    echo "Error: para directory does not exist at $PARA_DIR"
    exit 1
fi

# Check if lib directory exists
if [ ! -d "$LIB_DIR" ]; then
    echo "Error: lib directory does not exist at $LIB_DIR"
    exit 1
fi

echo "Generating MD5 checksums for files in $PARA_DIR and $LIB_DIR"
echo "Output will be saved to $PARA_OUTPUT_FILE and $LIB_OUTPUT_FILE"

# Clear the output files
> "$PARA_OUTPUT_FILE"
> "$LIB_OUTPUT_FILE"

# Loop through all files in the para directory
echo "Processing files in $PARA_DIR directory:"
for file in "$PARA_DIR"/*; do
    if [ -f "$file" ]; then
        filename=$(basename "$file")
        # Skip the md5sums.txt file itself to avoid including its own checksum
        if [ "$filename" != "md5sums.txt" ]; then
            md5sum_value=$(md5sum "$file" | cut -d' ' -f1)
            echo "$md5sum_value  $filename" >> "$PARA_OUTPUT_FILE"
            echo "MD5 for $filename: $md5sum_value"
        fi
    fi
done

# Loop through all files in the lib directory
echo "Processing files in $LIB_DIR directory:"
for file in "$LIB_DIR"/*; do
    if [ -f "$file" ]; then
        filename=$(basename "$file")
        # Skip the md5sums.txt file itself to avoid including its own checksum
        if [ "$filename" != "md5sums.txt" ]; then
            md5sum_value=$(md5sum "$file" | cut -d' ' -f1)
            echo "$md5sum_value  $filename" >> "$LIB_OUTPUT_FILE"
            echo "MD5 for $filename: $md5sum_value"
        fi
    fi
done

echo "MD5 checksums have been saved to $PARA_OUTPUT_FILE and $LIB_OUTPUT_FILE"
