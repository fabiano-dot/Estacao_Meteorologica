#!/bin/bash
set -e
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

git pull --rebase origin main

bash "$ROOT_DIR/scripts/export_yesterday.sh"

if [ -d ".git" ]; then
  git add data
  if ! git diff --cached --quiet; then
    git commit -m "Daily export $(date +%Y-%m-%d)"
    git pull --rebase origin main
    git push origin main
  fi
fi
