#!/usr/bin/env bash

# 用法：
#   ./clean-git-folders.sh <target-directory>
# 例如：
#   ./clean-git-folders.sh vendor/

set -e

if [ $# -ne 1 ]; then
  echo "Usage: $0 <target-directory>"
  exit 1
fi

TARGET=$1

if [ ! -d "$TARGET" ]; then
  echo "Error: $TARGET does not exist or is not a directory."
  exit 1
fi

echo "Searching for .git and .github folders under $TARGET ..."

find "$TARGET" -type d \( -name ".git" -o -name ".github" \) -print -exec rm -rf {} +

echo "Done! All .git and .github folders under $TARGET have been removed."

