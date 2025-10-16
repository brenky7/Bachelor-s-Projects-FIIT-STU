#!/usr/bin/env bash
# Testovací skript na meranie priepustnosti OCSP (požiadavky za minútu)
set -uo pipefail # -e odstránené, chyby budeme odchytávať

# --- Konfiguračné premenné ---
TARGET="http://192.168.100.8:8081/ocsp" # Cieľový endpoint
REQUEST_FILE="test_remote/request1.der" # DER súbor s OCSP požiadavkou
CA_CERT_FILE="test_remote/ca.pem"       # CA certifikát na overenie odpovede
DURATION_SECONDS=30                     # Trvanie testu v sekundách
CONCURRENCY=4                           # Počet paralelných požiadaviek
USE_METHOD="POST"                       # Metóda na použitie: "POST" alebo "GET"
RESULTS_CSV_FILE="throughput_summary.csv" # Výstupný CSV súbor pre súhrnné výsledky

# --- Čas spustenia pre CSV ---
run_timestamp=$(date +'%Y-%m-%d %H:%M:%S')

# --- Dočasný adresár pre odpovede ---
TEMP_DIR=$(mktemp -d)
echo "Dočasné súbory budú ukladané do: $TEMP_DIR"
trap 'echo "Čistím dočasný adresár..."; rm -rf "$TEMP_DIR"' EXIT # Mazanie pri ukončení

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
for cmd in curl openssl mktemp date sleep awk grep base64 tr; do
    if ! command -v $cmd &> /dev/null; then
        echo "Chyba: Príkaz '$cmd' nebol nájdený. Nainštalujte ho." >&2
        exit 1
    fi
done

# --- Funkcia na vykonanie jednej požiadavky ---
# Argumenty: $1 = ID požiadavky
run_request() {
  local id=$1
  local method="$USE_METHOD"
  local curl_opts=()
  local url=""
  local b64=""
  local response_file="$TEMP_DIR/response_${id}.der"
  local headers_file="$TEMP_DIR/headers_${id}.txt"
  local curl_exit_code=0

  # --- Príprava požiadavky ---
  if [[ "$method" == "GET" ]]; then
    local b64_output
    b64_output=$(openssl base64 -in "$REQUEST_FILE") || return 1
    b64=$(echo "$b64_output" | tr -d '\n' | tr '+/' '-_' | tr -d '=')
    url="$TARGET/$b64"
    curl_opts=(-X GET "$url")
  elif [[ "$method" == "POST" ]]; then
    url="$TARGET/"
    curl_opts=(-X POST --data-binary "@$REQUEST_FILE" \
               -H "Content-Type: application/ocsp-request" \
               -H "Accept: application/ocsp-response" \
               "$url")
  else
      echo "Chyba: Neznáma metóda '$method'" >&2
      return 1
  fi

  curl --silent --fail --show-error \
       -D "$headers_file" \
       "${curl_opts[@]}" \
       -o "$response_file"
  curl_exit_code=$?

  if [[ $curl_exit_code -ne 0 ]]; then
      rm -f "$response_file" "$headers_file"
      return $curl_exit_code
  fi
  return 0
}

# --- Hlavná časť skriptu ---
echo "Spúšťam test priepustnosti na $DURATION_SECONDS sekúnd s $CONCURRENCY paralelnými požiadavkami..."
echo "Cieľ: $TARGET, Metóda: $USE_METHOD"

start_time=$(date +%s)
end_time=$((start_time + DURATION_SECONDS))
current_time=$start_time

declare -a pids=()
declare -A pid_to_id=()
request_counter=0
completed_requests=0
failed_requests=0

while (( current_time < end_time )); do
  while (( ${#pids[@]} < CONCURRENCY && current_time < end_time )); do
    request_counter=$((request_counter + 1))
    run_request "$request_counter" &
    pid=$!
    pids+=("$pid")
    pid_to_id["$pid"]=$request_counter
  done

  if (( ${#pids[@]} > 0 )); then
    wait_exit_code=0
    finished_pid=""
    if command -v wait &>/dev/null && wait -n -p finished_pid &>/dev/null; then
        wait_exit_code=$?
    else
        sleep 0.01
        for i in "${!pids[@]}"; do
            pid_to_check="${pids[$i]}"
            if ! kill -0 "$pid_to_check" 2>/dev/null; then
                wait "$pid_to_check"
                wait_exit_code=$?
                finished_pid="$pid_to_check"
                break
            fi
        done
    fi

    if [[ -n "$finished_pid" ]]; then
        new_pids=()
        for pid_val in "${pids[@]}"; do
            if [[ "$pid_val" != "$finished_pid" ]]; then
                new_pids+=("$pid_val")
            fi
        done
        pids=("${new_pids[@]}")
        # req_id=${pid_to_id[$finished_pid]} # req_id sa nepoužíva ďalej
        unset pid_to_id["$finished_pid"]

        if [[ $wait_exit_code -eq 0 ]]; then
            completed_requests=$((completed_requests + 1))
        else
            failed_requests=$((failed_requests + 1))
        fi
    fi
  else
      sleep 0.1
  fi
  current_time=$(date +%s)
done

echo "Časový limit ($DURATION_SECONDS s) dosiahnutý. Čakám na dokončenie zvyšných ${#pids[@]} požiadaviek..."

while (( ${#pids[@]} > 0 )); do
    wait_exit_code=0
    finished_pid=""
    if command -v wait &>/dev/null && wait -n -p finished_pid &>/dev/null; then
        wait_exit_code=$?
    else
        sleep 0.01
        for i in "${!pids[@]}"; do
            pid_to_check="${pids[$i]}"
            if ! kill -0 "$pid_to_check" 2>/dev/null; then
                wait "$pid_to_check"
                wait_exit_code=$?
                finished_pid="$pid_to_check"
                break
            fi
        done
    fi

    if [[ -n "$finished_pid" ]]; then
        new_pids=()
        for pid_val in "${pids[@]}"; do
            if [[ "$pid_val" != "$finished_pid" ]]; then
                new_pids+=("$pid_val")
            fi
        done
        pids=("${new_pids[@]}")
        unset pid_to_id["$finished_pid"]

        if [[ $wait_exit_code -eq 0 ]]; then
            completed_requests=$((completed_requests + 1))
        else
            failed_requests=$((failed_requests + 1))
        fi
    fi
done

actual_end_time=$(date +%s)
actual_duration=$((actual_end_time - start_time))

echo "Všetky požiadavky dokončené."
echo "Spracovávam uložené odpovede..."

valid_ocsp_responses=0
invalid_ocsp_responses=0
processed_files=0

shopt -s nullglob
response_files=("$TEMP_DIR"/response_*.der)
shopt -u nullglob

for response_file in "${response_files[@]}"; do
    processed_files=$((processed_files + 1))
    openssl_output=$(openssl ocsp -respin "$response_file" -CAfile "$CA_CERT_FILE" -no_nonce -text 2>&1)
    openssl_ec=$?

    if [[ $openssl_ec -eq 0 ]]; then
        if echo "$openssl_output" | grep -q 'Cert Status: \(good\|revoked\|unknown\)'; then
            valid_ocsp_responses=$((valid_ocsp_responses + 1))
        else
            invalid_ocsp_responses=$((invalid_ocsp_responses + 1))
            echo "Varovanie: openssl OK, ale nenašiel sa platný Cert Status v odpovedi '$response_file'" >&2
        fi
    else
        invalid_ocsp_responses=$((invalid_ocsp_responses + 1))
    fi
done

# --- Výpis výsledkov na konzolu ---
echo "----------------------------------------"
echo "            VÝSLEDKY TESTU"
echo "----------------------------------------"
echo "Čas spustenia:              $run_timestamp"
echo "Celkové trvanie testu:      ${actual_duration} s (požadované: ${DURATION_SECONDS} s)"
echo "Celkový počet pokusov:      $request_counter"
echo "Úspešné HTTP požiadavky:    $completed_requests"
echo "Neúspešné HTTP požiadavky:  $failed_requests"
echo "----------------------------------------"
echo "Spracovaných OCSP odpovedí: $processed_files (zo $completed_requests úspešných HTTP)"
echo "Platné OCSP odpovede:       $valid_ocsp_responses"
echo "Neplatné OCSP odpovede:     $invalid_ocsp_responses"
echo "----------------------------------------"

requests_per_second="0.00"
requests_per_minute="0.00"
if (( actual_duration > 0 )); then
  requests_per_second=$(awk "BEGIN {printf \"%.2f\", $completed_requests / $actual_duration}")
  requests_per_minute=$(awk "BEGIN {printf \"%.2f\", $completed_requests / $actual_duration * 60}")
  echo "Priepustnosť (HTTP OK):     ${requests_per_second} req/s"
  echo "Priepustnosť (HTTP OK):     ${requests_per_minute} req/min"
else
  echo "Priepustnosť: N/A (trvanie testu bolo 0 sekúnd)"
fi

valid_ocsp_percentage="0.00"
if (( completed_requests > 0 )); then # Použijeme completed_requests ako základ pre percento platných
    valid_ocsp_percentage=$(awk "BEGIN {printf \"%.2f\", $valid_ocsp_responses / $completed_requests * 100}")
    echo "Percento platných OCSP:     ${valid_ocsp_percentage}% (z úspešných HTTP)"
fi
echo "----------------------------------------"

# --- Zápis súhrnných výsledkov do CSV súboru ---
echo "Zapisujem súhrn do CSV: $RESULTS_CSV_FILE"

# Hlavička pre CSV súbor. Používame bodkočiarku ako oddeľovač.
CSV_HEADER="Timestamp;TotalAttempts;SuccessfulHTTP;FailedHTTP;ValidOCSP;InvalidOCSP;ProcessedOCSPResponses;RequestsPerMinute;ActualDurationSec;TargetDurationSec;Concurrency;Method"

# Vytvorí hlavičku, ak súbor neexistuje
if [[ ! -f "$RESULTS_CSV_FILE" ]]; then
    echo "$CSV_HEADER" > "$RESULTS_CSV_FILE"
fi

# Príprava dátového riadku pre CSV
# requests_per_minute je už vypočítané vyššie
CSV_DATA_ROW="${run_timestamp};${request_counter};${completed_requests};${failed_requests};${valid_ocsp_responses};${invalid_ocsp_responses};${processed_files};${requests_per_minute};${actual_duration};${DURATION_SECONDS};${CONCURRENCY};${USE_METHOD}"

# Appendovanie dát do CSV
echo "$CSV_DATA_ROW" >> "$RESULTS_CSV_FILE"

echo "Súhrn zapísaný do $RESULTS_CSV_FILE."
echo "Test dokončený."
