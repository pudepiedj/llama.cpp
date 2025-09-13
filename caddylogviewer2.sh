#!/usr/bin/env zsh
sudo -E zsh -o pipefail - <<'EOF'
tail -F /var/log/caddy/access.log |
jq --unbuffered -r '
  [ (.ts | strftime("%Y-%m-%d %H:%M:%S")),
    (.user_id // "-"),
    ((.request.headers["X-Forwarded-For"] // ["-"])[0] // "-"),
    (.request.headers["Authorization"][0] // "no-creds"),
    (.request.uri // "-"),
    (.status // 0 | tostring),
    ((.duration // 0) * 1000),
    (.size // "null" | tostring)
  ] | @tsv' |
while IFS=$'\t' read -r ts user ip auth uri code dur size; do
  printf '%s  \033[33m%-12s\033[0m  %-42s  %-10s  %-30s  %6s  \033[34m%10.2f\033[0m ms  \033[31m%10s\033[0m\n' \
         "$ts" "$user" "$ip" "$auth" "$uri" "$code" "$dur" "$size"
done
EOF

