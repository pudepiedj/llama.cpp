# Shell to run KK2-0905-UD-Q3_K_XL without having to type the monster CLI
# assumes we have CD'd to ~/Development/llama.cpp

#!/bin/bash
MODEL_PATH="/Users/edsilmacstudio/Development/llama.cpp/models/Kimi-K2-Instruct-0905-UD-Q3_K_XL/models--unsloth--Kimi-K2-Instruct-0905-GGUF/snapshots/ca516d05c7621c0615db3fc7efa63c9617547363/UD-Q3_K_XL/Kimi-K2-Instruct-0905-UD-Q3_K_XL-00001-of-00010.gguf"

./build/bin/llama-server \
  -m "$MODEL_PATH" \
  -c 225176 \
  -ngl 99 \
  --parallel 4 \
  --no-warmup \
  --host 0.0.0.0 \
  --port 3000 \
  --log-file "KK2_0905_log_3"
