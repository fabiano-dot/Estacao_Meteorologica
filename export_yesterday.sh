#!/bin/bash
set -e

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

DATE=$(TZ="America/Sao_Paulo" date -d "yesterday" +%Y-%m-%d)

bash "$ROOT_DIR/ScriptsToDataDailyUpdate/export_for_date.sh" "$DATE"
