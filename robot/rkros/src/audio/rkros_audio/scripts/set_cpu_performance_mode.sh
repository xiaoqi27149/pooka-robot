#!/bin/bash

# Script to set CPU scaling governor to performance mode for all CPUs

echo "Setting CPU scaling governor to performance mode..."

# Loop through CPUs 0 to 7
for cpu in {0..7}; do
    governor_path="/sys/devices/system/cpu/cpu${cpu}/cpufreq/scaling_governor"

    # Check if the governor file exists
    if [ -f "$governor_path" ]; then
        current_governor=$(cat "$governor_path")
        echo "CPU${cpu}: Current governor is $current_governor"

        # Set the governor to performance
        echo "performance" | sudo tee "$governor_path"

        # Verify the change
        new_governor=$(cat "$governor_path")
        echo "CPU${cpu}: Governor set to $new_governor"
    else
        echo "Warning: $governor_path does not exist"
    fi
done

echo "Done!"
