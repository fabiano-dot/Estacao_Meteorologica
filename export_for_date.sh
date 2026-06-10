#!/bin/bash
set -e
ORIG_PATH="$PATH"
source "$(dirname "$0")/config.env"
PATH="$ORIG_PATH:/mingw64/bin"
export PATH

DATE="$1"
if [ -z "$DATE" ]; then
  echo "Uso: $0 YYYY-MM-DD"
  exit 1
fi

YEAR=$(date -d "$DATE" +%Y)
MONTH=$(date -d "$DATE" +%m)
DAY=$(date -d "$DATE" +%d)
OUT_DIR="data/$YEAR/$MONTH/$DAY"
RAW_DIR="$OUT_DIR/raw_json"
mkdir -p "$RAW_DIR"

BASE_START_SEC=$(TZ="America/Sao_Paulo" date -d "$DATE 00:00:00" +%s)
BASE_END_SEC=$(TZ="America/Sao_Paulo" date -d "$DATE 23:59:59" +%s)
OFFSET_FIX_SEC=0
START_TS=$((BASE_START_SEC + OFFSET_FIX_SEC))000
END_TS=$((BASE_END_SEC + OFFSET_FIX_SEC))000

echo "[INFO] Exportando dia local (America/Sao_Paulo) $DATE → $OUT_DIR"
echo "[INFO] Janela (ms) com offset_fix=${OFFSET_FIX_SEC}s: $START_TS – $END_TS"

TOKEN=$(curl -s -X POST "$TB_URL/api/auth/login" \
  -H "Content-Type: application/json" \
  -d "{\"username\":\"$TB_USER\",\"password\":\"$TB_PASS\"}" | jq -r '.token')

# --- DEVICES ---
for DEV in "${!DEVICES[@]}"; do
  ID="${DEVICES[$DEV]}"
  RAW="$RAW_DIR/raw_${DEV}.json"
  CSV="$OUT_DIR/${DEV}.csv"
  KEYS="temperature,humidity,pressure,Vsys,luminosity,latitude,longitude,altitude,rssi,snr,dr,fcnt,aq_timestamp_ms"

  echo "[INFO] Baixando $DEV ($ID)..."
  curl -s -X GET "$TB_URL/api/plugins/telemetry/DEVICE/$ID/values/timeseries?keys=$KEYS&startTs=$START_TS&endTs=$END_TS&limit=50000" \
    -H "X-Authorization: Bearer $TOKEN" \
    -o "$RAW"

  HAS_DATA=$(jq 'to_entries | map(select(.value | type=="array" and length>0)) | length' "$RAW")
  if [ "$HAS_DATA" -eq 0 ]; then
    echo "[WARN] Sem dados para $DEV — não gerando arquivos."
    rm -f "$RAW"
    continue
  fi

  echo "[INFO] Gerando CSV para $DEV..."
  TZ="America/Sao_Paulo" jq -r '
    def q($x):
      (if $x == null then "" else ($x|tostring) end)
      | gsub("\""; "\"\"")
      | "\"" + . + "\"";
    def v($arr; $ts):
      (($arr // []) | map(select(.ts == $ts)) | .[0].value) // null;
    def ts_all:
      (to_entries
        | map(.value)
        | map(if type=="array" then map(.ts) else [] end)
        | add
        | unique
        | sort);
    ([
      "Nº",
      "Data e Hora (America/Sao_Paulo)",
      "Temperatura (°C)",
      "Umidade Relativa (%)",
      "Pressão Atmosférica (hPa)",
      "Tensão do Sistema (mV)",
      "Luminosidade (lux)",
      "Latitude (°)",
      "Longitude (°)",
      "Altitude (m)",
      "RSSI (dBm)",
      "SNR (dB)",
      "DR",
      "Frame Counter",
      "Timestamp Aquisição (ms)"
    ] | map(q(.)) | join(",")),
    (ts_all[] as $ts |
      [
        null,
        (($ts/1000) | strflocaltime("%Y-%m-%d %H:%M:%S")),
        v(.temperature;     $ts),
        v(.humidity;        $ts),
        v(.pressure;        $ts),
        v(.Vsys;            $ts),
        v(.luminosity;      $ts),
        v(.latitude;        $ts),
        v(.longitude;       $ts),
        v(.altitude;        $ts),
        v(.rssi;            $ts),
        v(.snr;             $ts),
        v(.dr;              $ts),
        v(.fcnt;            $ts),
        v(.aq_timestamp_ms; $ts)
      ]
      | map(q(.))
      | join(",")
    )
  ' "$RAW" \
  | awk 'BEGIN{FS=","; OFS=","} NR==1{print; next} { $1="\"" (NR-1) "\""; print }' > "$CSV"
  echo "[OK] Exportado $DEV → $CSV"
done

# --- OWM ASSET ---
echo "[INFO] Baixando OWM asset..."
OWM_RAW="$RAW_DIR/raw_owm_unicamp.json"
OWM_CSV="$OUT_DIR/owm_unicamp.csv"
OWM_KEYS="temperature,humidity,pressure,owm_wind_speed,owm_wind_deg,owm_clouds,owm_rain,owm_coord_lat,owm_coord_lon"

curl -s "$TB_URL/api/plugins/telemetry/ASSET/$OWM_ASSET_ID/values/timeseries?keys=$OWM_KEYS&startTs=$START_TS&endTs=$END_TS&limit=50000" \
  -H "X-Authorization: Bearer $TOKEN" \
  -o "$OWM_RAW"

HAS_OWM=$(jq 'to_entries | map(select(.value | type=="array" and length>0)) | length' "$OWM_RAW")
if [ "$HAS_OWM" -eq 0 ]; then
  echo "[WARN] Sem dados OWM — não gerando arquivo."
  rm -f "$OWM_RAW"
else
  echo "[INFO] Gerando CSV OWM..."
  TZ="America/Sao_Paulo" jq -r '
    def q($x):
      (if $x == null then "" else ($x|tostring) end)
      | gsub("\""; "\"\"")
      | "\"" + . + "\"";
    def v($arr; $ts):
      (($arr // []) | map(select(.ts == $ts)) | .[0].value) // null;
    def ts_all:
      (to_entries
        | map(.value)
        | map(if type=="array" then map(.ts) else [] end)
        | add
        | unique
        | sort);
    ([
      "Nº",
      "Data e Hora (America/Sao_Paulo)",
      "Temperatura OWM (°C)",
      "Umidade OWM (%)",
      "Pressão OWM (hPa)",
      "Vento (m/s)",
      "Direção Vento (°)",
      "Nuvens (%)",
      "Chuva 1h (mm)",
      "Latitude (°)",
      "Longitude (°)"
    ] | map(q(.)) | join(",")),
    (ts_all[] as $ts |
      [
        null,
        (($ts/1000) | strflocaltime("%Y-%m-%d %H:%M:%S")),
        v(.temperature;     $ts),
        v(.humidity;        $ts),
        v(.pressure;        $ts),
        v(.owm_wind_speed;  $ts),
        v(.owm_wind_deg;    $ts),
        v(.owm_clouds;      $ts),
        v(.owm_rain;        $ts),
        v(.owm_coord_lat;   $ts),
        v(.owm_coord_lon;   $ts)
      ]
      | map(q(.))
      | join(",")
    )
  ' "$OWM_RAW" \
  | awk 'BEGIN{FS=","; OFS=","} NR==1{print; next} { $1="\"" (NR-1) "\""; print }' > "$OWM_CSV"
  echo "[OK] Exportado OWM → $OWM_CSV"
fi

# Limpa raw_json após geração dos CSVs
rm -rf "$RAW_DIR"
echo "[INFO] raw_json removido."

echo "[FIN] Export para $DATE terminado."
