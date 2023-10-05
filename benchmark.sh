#!/bin/bash

# Set the top directory
top_dir="dataset"

# Find all subdirectories containing pairs of images (excluding the top directory)
subdirs=$(find "$top_dir" -mindepth 1 -type d)

correct=0
tests=0
times=0

# Iterate through each subdirectory and list image pairs
for subdir in $subdirs; do
    # Find all image files in the subdirectory
    image_files=$(find "$subdir" -type f | sort)
    
    # Check if there are at least 2 image files in the subdirectory
    if [ $(echo "$image_files" | wc -l) -ge 2 ]; then
        tests=$(expr $tests + 1)
        # Print the subdirectory name as the pair identifier
        # printf "Pair in Subdirectory: %s\n" "$subdir"
        
        # Initialize variables to store image filenames
        image1=""
        image2=""
        
        # Loop through the sorted image files and list them as a pair
        while read -r image; do
            if [ -z "$image1" ]; then
                image1="$image"
            else
                image2="$image"
            fi
        done <<< "$image_files"
        
        output=$(./ComparerPlayground -f1 $image1 -f2 $image2 -s 1024 -b --algorithm custom)
        if [ $? -eq 0 ]; then
            correct=$(expr $correct + 1)
        fi
        
        echo $output
        IFS=";" read -ra parts <<< "$output"
        elapsed=$(echo "${parts[4]}" | grep -o '[0-9]*')
        times=$(expr $times + $(expr $elapsed))
    fi
done

perc=$(( (correct * 100) / tests ))
max_length=40
padding=$(( ($max_length - 11) / 2 ))

print_padded_line() {
    local text="$1"
    printf "║ %-${max_length}s ║\n" "$text"
}

# Define a function to print lines with center-aligned text
print_center_aligned_line() {
    local text="$1"
    printf "║ %${padding}s%-$(($max_length - $padding))s ║\n" "" "$text"
}

echo "╔══════════════════════════════════════════╗"
print_center_aligned_line "Cup results"
print_padded_line ""
print_padded_line "Got $correct out of $tests = $perc%"
print_padded_line "Took $times ms"
print_padded_line ""
echo "╚══════════════════════════════════════════╝"
