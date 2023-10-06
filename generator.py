import os
import random
from PIL import Image, ImageDraw

# Directory containing the .png files
folder_path = "different"

# List all files in the directory
file_list = os.listdir(folder_path)

# Iterate through the files
for filename in file_list:
    if filename.lower().endswith("b.png"):
        print(filename)
        # Load the image
        image_path = os.path.join(folder_path, filename)
        img = Image.open(image_path)

        # Get image dimensions
        width, height = img.size

        # Generate a random vertical position for the white line
        line_position = random.randint(0, height - 1)

        # Create a drawing context
        draw = ImageDraw.Draw(img)

        # Draw a white line across the image
        draw.line([(0, line_position), (width, line_position)], fill="white", width=15)

        # Save the modified image
        output_path = os.path.join(folder_path, filename)
        img.save(output_path)

print("White lines added to 'b' images.")
