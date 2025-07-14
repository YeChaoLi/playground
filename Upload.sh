#!/usr/bin/env bash

# 用法：
#   ./clean-git-folders-and-push.sh <target-directory>
# 例如：
#   ./clean-git-folders-and-push.sh vendor/

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

echo "✅ All .git and .github folders under $TARGET have been removed."

# Add changes to staging
echo "Adding changes to staging..."
git add "$TARGET"

# Show git status
echo "📌 Current git status:"
git status

# Prompt for commit message
echo
read -rp "Enter commit message: " COMMIT_MSG

# Commit changes
git commit -m "$COMMIT_MSG"

# Push to remote
echo "Pushing to remote..."
git push

echo "🎉 Done! Changes pushed."

