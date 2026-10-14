#!/bin/bash

# Script to change permissions of LED brightness files to allow current user access

# Get the original user ID and group ID (before sudo)
if [ "$SUDO_USER" ]; then
    # If running with sudo, get the original user
    ORIGINAL_USER="$SUDO_USER"
    ORIGINAL_UID=$(id -u "$SUDO_USER")
    ORIGINAL_GID=$(id -g "$SUDO_USER")
else
    # If not running with sudo, use current user
    ORIGINAL_USER=$(whoami)
    ORIGINAL_UID=$(id -u "$ORIGINAL_USER")
    ORIGINAL_GID=$(id -g "$ORIGINAL_USER")
fi

# Find all brightness files in led directories and change ownership
for brightness_file in /sys/class/leds/led*/brightness; do
    if [ -f "$brightness_file" ]; then
        echo "Changing ownership of $brightness_file to $ORIGINAL_USER (UID: $ORIGINAL_UID, GID: $ORIGINAL_GID)"
        chown $ORIGINAL_UID:$ORIGINAL_GID "$brightness_file"

        # Ensure the file has read-write permissions for the owner
        chmod 664 "$brightness_file"
    fi
done

echo "LED brightness file permissions updated successfully."
