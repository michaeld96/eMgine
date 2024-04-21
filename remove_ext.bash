#!/bin/bash

# Path to the libs folder
LIBS_FOLDER="libs"

# Find and remove files ending with :Zone.Identifier in the libs folder
find "$LIBS_FOLDER" -type f -name '*:Zone.Identifier' -exec rm {} \;
