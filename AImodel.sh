#!/bin/zsh
emulate -L zsh

typeset -A MODEL
MODEL=(
    KK1  ~/Development/llama.cpp/models/models--unsloth--Kimi-K2-Instruct-GGUF/snapshots/dc35109e1b6ecf981f1d30d57c117b9a58c0b31b/UD-Q3_K_XL/Kimi-K2-Instruct-UD-Q3_K_XL-00001-of-00010.gguf
    DS3  ~/Development/llama.cpp/models/models--unsloth--DeepSeek-V3.1-GGUF/snapshots/f938c2568d34511a0e9fb5d340c7050a9df3cb13/Q4_K_M/DeepSeek-V3.1-Q4_K_M-00001-of-00009.gguf
    KK2  /Users/edsilmacstudio/Development/llama.cpp/models/Kimi-K2-Instruct-0905-UD-Q3_K_XL/models--unsloth--Kimi-K2-Instruct-0905-GGUF/snapshots/ca516d05c7621c0615db3fc7efa63c9617547363/UD-Q3_K_XL/Kimi-K2-Instruct-0905-UD-Q3_K_XL-00001-of-00010.gguf
    MIS  ~/Development/llama.cpp/models/Mixtral-8x7b/Mixtral-8x7B-Instruct-v0.1.Q4_K_M.gguf
    GPT  ~/Development/llama.cpp/models/gguf/gpt-oss-120B.gguf
    QW2  ~/Development/llama.cpp/models/Qwen_2.5_Omni/Qwen2.5-Omni-7B.gguf
    QW3  ~/Development/llama.cpp/models/gguf/qwen3-235b-f16.gguf
    NH4  ~/Development/llama.cpp/models/Nous/Hermes-4-70B-Q8_0-00001-of-00002.gguf
    XBA  ~/Development/llama.cpp/models/XBai-o4.Q8_0.gguf
    GGU  ~/Development/llama.cpp/models/7B/ggml-model-q4_0.gguf
    LLA  ~/Development/llama.cpp/models/Llama_70B/Llama-3.3-70B-Instruct-F16.gguf
)

# ---------- sanity checks ---------------------------------
[[ -z $1 ]] && { printf "Usage: $0 <alias>  [extra-opts...]\nAliases: ${(k)MODEL}\n"; exit 1; }
[[ -z ${MODEL[$1]} ]] && { printf "Unknown alias ‘$1’. Valid: ${(k)MODEL}\n"; exit 1; }
[[ -x ./build/bin/llama-server ]] || { echo "llama-server not found in ./build/bin"; exit 1; }

# Save the alias before we throw it away
ALIAS=$1
MODEL_PATH=${MODEL[$ALIAS]}
echo "ALIAS=$ALIAS  MODEL_PATH=${MODEL[$ALIAS]}" >&2
echo "ALIAS=$ALIAS MODEL_PATH=$MODEL_PATH" >&2
LOG_FILE=$LOG_DIR/${ALIAS:t:r}-$(date +%Y%m%d-%H%M%S).log   # use $ALIAS, not $1
echo "LOG_FILE=$LOG_FILE"
shift

# ---------- defaults (tweak once, affects every model) -----
DEF_OPTS=(                   # ← array, not scalar string
  --parallel 4
  -ngl 99
  --no-warmup
  --host 0.0.0.0
  --port 3000
)
LOG_DIR=~/llama-logs
mkdir -p $LOG_DIR

# ---------- launch ----------------------------------------
./build/bin/llama-server \
    -m "$MODEL_PATH" \
    $DEF_OPTS \
    --log-file "$LOG_FILE" \
    "$@"          # any extra flags you add on CLI come last

# example:
#   llama KK2 -ngl 80 --port 3030
# overrides ngl and port for that single run.

