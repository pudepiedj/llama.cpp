# simple CLI to show what caddy is logging

sudo tail -F /var/log/caddy/access.log | \
  jq -r '
    "\u001b[33m" + (.user_id // "-----") + "\u001b[0m  " +
    (.ts | strftime("%Y-%m-%d %H:%M:%S")) + "  " +
    ((.request.headers["X-Forwarded-For"]? // ["-"])[0]) + "  " +
    ((.request.headers["Authorization"]? // ["no-creds"])[0]) + "  " +
    (.request.uri) + "  " +
    (.status | tostring) + "  " +
    "\u001b[36m" + "Duration: " + (.duration * 1000 | tostring) + " ms\u001b[0m" + "  " +
    "\u001b[32m" + "Size: " + (.size | tostring) + "bytes\u001b[0m"
  '

