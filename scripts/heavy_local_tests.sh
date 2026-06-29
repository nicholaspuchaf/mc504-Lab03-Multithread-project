#!/usr/bin/env bash
set -eu

BIN="${BIN:-./aeroporto}"

run_visual_case() {
    name="$1"
    shift

    printf "\n==> %s\n" "${name}"
    printf "Parametros escolhidos:\n"
    printf "  %s\n" "$*"
    printf "Pressione Enter para iniciar..."
    read -r _
    "${BIN}" "$@"
}

if [ ! -x "${BIN}" ]; then
    echo "Binario nao encontrado em ${BIN}. Execute 'make' antes."
    exit 1
fi

run_visual_case \
    "fluxo balanceado com duas pistas" \
    --planes 24 \
    --runways 2 \
    --landing-prob 65 \
    --emergency-prob 15 \
    --runway-time-ms 900 \
    --arrival-delay-ms 240 \
    --max-landings 3

run_visual_case \
    "alta disputa com uma pista" \
    --planes 30 \
    --runways 1 \
    --landing-prob 70 \
    --emergency-prob 20 \
    --runway-time-ms 1000 \
    --arrival-delay-ms 180 \
    --max-landings 3

run_visual_case \
    "muitas emergencias de pouso" \
    --planes 25 \
    --runways 2 \
    --landing-prob 90 \
    --emergency-prob 55 \
    --runway-time-ms 900 \
    --arrival-delay-ms 200 \
    --max-landings 4

run_visual_case \
    "alto volume com tres pistas" \
    --planes 45 \
    --runways 3 \
    --landing-prob 60 \
    --emergency-prob 10 \
    --runway-time-ms 700 \
    --arrival-delay-ms 140 \
    --max-landings 3

echo "Cenarios locais concluidos."
