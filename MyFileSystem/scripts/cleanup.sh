#!/bin/bash

# Find and delete .DS_Store files, ._ files, and .vscode folders
find .. \( -name '*.DS_Store' -o -name '._*' \) -type f -delete
find .. -name '.vscode' -type d -exec rm -rf {} +

echo "Cleanup complete."