#!/bin/bash

# Path to your virtualenv Python
VENV_PYTHON="./venv/bin/python"

if [ ! -x "$VENV_PYTHON" ]; then
    echo "Error: Python not found at $VENV_PYTHON"
    echo "Please update the path to your virtual environment."
    exit 1
fi

echo "Searching for .bmp or .BMP files..."

# Find all .bmp and .BMP files recursively
find . -type f \( -name "*.bmp" -o -name "*.BMP" \) | while read -r bmp; do
    png="${bmp%.*}.png"

    if [ -f "$png" ]; then
        echo "Skipping existing: $png"
        continue
    fi

    echo "Converting: $bmp -> $png"

    "$VENV_PYTHON" <<EOF
from PIL import Image
img = Image.open(r'''$bmp''')
img.save(r'''$png''', "PNG")
EOF

done

echo "Done."
