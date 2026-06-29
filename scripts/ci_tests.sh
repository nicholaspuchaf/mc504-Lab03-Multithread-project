#!/usr/bin/env bash
set -eu

BIN="${BIN:-./aeroporto}"
TIMEOUT="${TIMEOUT:-10s}"

run_and_assert() {
    name="$1"
    expected_completed="$2"
    shift 2

    output_file="$(mktemp)"

    echo "==> ${name}"
    timeout "${TIMEOUT}" "${BIN}" "$@" > "${output_file}"

    grep -q "Concluidos: ${expected_completed}/${expected_completed}" "${output_file}"
    grep -q "Emergencia:  \\[vazia\\]" "${output_file}"
    grep -q "Pouso:       \\[vazia\\]" "${output_file}"
    grep -q "Decolagem:   \\[vazia\\]" "${output_file}"

    rm -f "${output_file}"
}

run_and_assert \
    "smoke test basico" \
    10 \
    --planes 10 \
    --runways 2 \
    --no-animation \
    --runway-time-ms 1 \
    --arrival-delay-ms 0

run_and_assert \
    "sincronizacao com uma pista" \
    20 \
    --planes 20 \
    --runways 1 \
    --no-animation \
    --runway-time-ms 1 \
    --arrival-delay-ms 0

single_runway_output="$(mktemp)"
timeout "${TIMEOUT}" "${BIN}" \
    --planes 20 \
    --runways 1 \
    --no-animation \
    --runway-time-ms 1 \
    --arrival-delay-ms 0 > "${single_runway_output}"
grep -q "Pista 0: \\[LIVRE\\]" "${single_runway_output}"
grep -q "Concluidos: 20/20" "${single_runway_output}"
rm -f "${single_runway_output}"

emergency_output="$(mktemp)"
echo "==> cenario com emergencias"
timeout "${TIMEOUT}" "${BIN}" \
    --planes 20 \
    --runways 2 \
    --landing-prob 100 \
    --emergency-prob 100 \
    --no-animation \
    --runway-time-ms 1 \
    --arrival-delay-ms 0 > "${emergency_output}"

grep -q "Concluidos: 20/20" "${emergency_output}"
grep -q "Emergencias: 20" "${emergency_output}"
grep -q "Emergencia:  \\[vazia\\]" "${emergency_output}"
grep -q "Pouso:       \\[vazia\\]" "${emergency_output}"
grep -q "Decolagem:   \\[vazia\\]" "${emergency_output}"
rm -f "${emergency_output}"

echo "Todos os testes de CI passaram."
