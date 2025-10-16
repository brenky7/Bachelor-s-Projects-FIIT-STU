#!/usr/bin/env bash
# Testovací skript na meranie OCSP odpovedí
set -euo pipefail # Vrátené späť pre robustnosť

# --- Konfiguračné premenné ---
TARGET="http://192.168.100.8:8081/ocsp"         # Cieľový endpoint
REQUEST_FILE="test_remote/request1.der"         # DER súbor s OCSP požiadavkou
CA_CERT_FILE="test_remote/ca.pem"               # CA certifikát na overenie odpovede
N=1000                                          # Počet požiadaviek
OUT_CSV="results_$(date +%d_%m_%H_%M).csv"      # Výstupný CSV súbor

# --- Dočasný súbor pre odpoveď ---
TEMP_RESPONSE_FILE=$(mktemp)
trap 'rm -f "$TEMP_RESPONSE_FILE"' EXIT # Tiché mazanie pri ukončení

# --- Kontrola existencie request súboru ---
if [[ ! -f "$REQUEST_FILE" ]]; then
    echo "Chyba: Súbor s požiadavkou '$REQUEST_FILE' neexistuje!" >&2
    exit 1
fi

# --- Kontrola existencie CA certifikátu ---
if [[ ! -f "$CA_CERT_FILE" ]]; then
    echo "Chyba: Súbor s CA certifikátom '$CA_CERT_FILE' neexistuje!" >&2
    exit 1
fi

# --- Kontrola dostupnosti nástrojov ---
for cmd in curl openssl bc tr grep awk mktemp stat sed tail; do
    if ! command -v $cmd &> /dev/null; then
        echo "Chyba: Príkaz '$cmd' nebol nájdený. Nainštalujte ho." >&2
        exit 1
    fi
done

# --- Header CSV súboru ---
echo "METHOD;TIME_MS;X-PROXY-CACHE;OCSP-CACHE;CERT_STATUS" > "$OUT_CSV"
echo "Spúšťam testovanie s $N požiadavkami na $TARGET..."

for ((i=1; i<=N; i++)); do
  METHOD=""
  CURL_OPTS=()
  URL=""
  B64=""
  RAW=""
  TIME_SEC=""
  HEADER_BLOCK=""
  TIME_MS="error"
  X_PROXY_CACHE="error" # Pre X-Proxy-Cache
  OCSP_CACHE="error" # Pre moju OCSP-Cache
  CERT_STATUS="error"
  CURL_EC=-1
  OPENSSL_EC=-1

  # --- Príprava požiadavky ---
  if (( i % 2 == 1 )); then
    METHOD="GET"
    B64_OUTPUT=$(openssl base64 -in "$REQUEST_FILE") # Chyby ukončia skript (set -e)
    B64=$(echo "$B64_OUTPUT" | tr -d '\n' | tr '+/' '-_' | tr -d '=')
    URL="$TARGET/$B64"
    CURL_OPTS=(-X GET "$URL")
  else
    METHOD="POST"
    CURL_OPTS=(-X POST --data-binary "@$REQUEST_FILE" \
               -H "Content-Type: application/ocsp-request" \
               -H "Accept: application/ocsp-response" \
               "$TARGET/")
  fi

  # --- Odoslanie požiadavky a meranie ---
  RAW=$(curl --silent --fail --show-error -D - "${CURL_OPTS[@]}" -o "$TEMP_RESPONSE_FILE" -w "\n%{time_total}")
  # Ak curl zlyhá, set -e ukončí skript

  # Oddelenie času a hlavičiek
  TIME_SEC=$(echo "$RAW" | tail -n 1)
  HEADER_BLOCK=$(echo "$RAW" | sed '$d')

  # Kontrola, či sme získali platný čas
  if ! [[ "$TIME_SEC" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
      echo "Chyba: Nepodarilo sa získať platný čas z výstupu curl ('$TIME_SEC')." >&2
      echo "Prijaté hlavičky:" >&2
      echo "$HEADER_BLOCK" >&2
  fi

  # Výpočet času v ms
  TIME_MS_RAW=$(echo "$TIME_SEC * 1000" | bc -l)
  TIME_MS=$(printf "%.0f" "$TIME_MS_RAW")

  # --- Vyhodnotenie X-Proxy-Cache ---
  if echo "$HEADER_BLOCK" | grep -iq '^X-Proxy-Cache: *HIT'; then
    X_PROXY_CACHE=HIT
  elif echo "$HEADER_BLOCK" | grep -iq '^X-Proxy-Cache: *MISS'; then
    X_PROXY_CACHE=MISS
  else
    X_PROXY_CACHE=x # Neznámy alebo chýbajúci header
  fi

  # --- Vyhodnotenie OCSP-Cache ---
  OCSP_CACHE="x"
  HEADER_LINE=$(echo "$HEADER_BLOCK" | grep -i '^OCSP-Cache:' || true)

  if [[ -n "$HEADER_LINE" ]]; then
      HEADER_VALUE_UPPER=$(echo "$HEADER_LINE" | awk -F: '{sub(/^[ \t]+/, "", $2); sub(/[ \t\r]+$/, "", $2); print toupper($2)}' )

      case "$HEADER_VALUE_UPPER" in
          HIT)
              OCSP_CACHE="HIT"
              ;;
          MISS)
              OCSP_CACHE="MISS"
              ;;
          BYPASS_NONCE)
              OCSP_CACHE="BYPASS_NONCE"
              ;;
          DISABLED)
              OCSP_CACHE="x"
              ;;
          *)
              OCSP_CACHE="x"
              ORIG_VALUE=$(echo "$HEADER_LINE" | awk -F: '{sub(/^[ \t]+/, "", $2); sub(/[ \t\r]+$/, "", $2); print $2}')
              echo "Varovanie: Nájdená hlavička OCSP-Cache, ale s neočakávanou hodnotou: '$ORIG_VALUE' pre požiadavku $i ($METHOD)" >&2
              ;;
      esac
  fi

  # --- Overenie OCSP odpovede ---
  CERT_STATUS="error"

  if [[ ! -s "$TEMP_RESPONSE_FILE" ]]; then
      CERT_STATUS="empty_response_body"
      echo "Varovanie: Prijaté telo odpovede pre požiadavku $i ($METHOD) je prázdne." >&2
  else
      OPENSSL_OUTPUT=$(openssl ocsp -respin "$TEMP_RESPONSE_FILE" -text -CAfile "$CA_CERT_FILE" 2>&1)
      OPENSSL_EC=$?

      if [[ $OPENSSL_EC -eq 0 ]]; then
          STATUS_LINE=$(echo "$OPENSSL_OUTPUT" | grep 'Cert Status:' || true)
          if [[ -n "$STATUS_LINE" ]]; then
              CERT_STATUS=$(echo "$STATUS_LINE" | awk -F': ' '{print $2}')
          else
              CERT_STATUS="status_not_found_in_output"
              echo "Varovanie: openssl prebehol OK pre požiadavku $i ($METHOD), ale 'Cert Status:' sa nenašiel vo výstupe." >&2
          fi
      else
          CERT_STATUS="openssl_error_$OPENSSL_EC"
          echo "Chyba: openssl ocsp zlyhal s kódom $OPENSSL_EC pre požiadavku $i ($METHOD)." >&2
          echo "Výstup openssl:" >&2
          echo "$OPENSSL_OUTPUT" >&2
      fi
  fi

  # --- Uloženie do CSV súboru ---
  echo "$METHOD;$TIME_MS;$X_PROXY_CACHE;$OCSP_CACHE;$CERT_STATUS" >> "$OUT_CSV"

  # --- Vyčistenie dočasného súboru pred ďalšou iteráciou ---
  > "$TEMP_RESPONSE_FILE"

  # --- Výpis pokroku ---
  if (( i % 200 == 0 )); then
    echo "Spracovaných $i / $N požiadaviek..."
  fi

done

echo "Hotovo: Spracovaných $N požiadaviek. Výsledky sú v súbore: $OUT_CSV"
