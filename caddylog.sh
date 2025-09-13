#!/bin/bash

# caddylog.sh - Parse Caddy access logs into one-line format
# Output format: timestamp user ip redacted_field request_id uri status duration size

# Function to parse Caddy JSON logs
parse_caddy_log() {
    while IFS= read -r line; do
        if [[ -n "$line" && "$line" =~ ^\{.*\}$ ]]; then
            # Extract fields using jq if available, fallback to basic parsing
            if command -v jq >/dev/null 2>&1; then
                timestamp=$(echo "$line" | jq -r '.ts // .timestamp // ""' 2>/dev/null)
                user=$(echo "$line" | jq -r '.request.headers.user[0] // .user // .request.headers."x-user"[0] // "admin"' 2>/dev/null)
                remote_ip=$(echo "$line" | jq -r '.request.remote_ip // .request.remote_addr // .remote_ip // ""' 2>/dev/null)
                method=$(echo "$line" | jq -r '.request.method // .method // ""' 2>/dev/null)
                uri=$(echo "$line" | jq -r '.request.uri // .uri // ""' 2>/dev/null)
                status=$(echo "$line" | jq -r '.status // ""' 2>/dev/null)
                duration=$(echo "$line" | jq -r '.duration // ""' 2>/dev/null)
                size=$(echo "$line" | jq -r '.size // .response.size // ""' 2>/dev/null)
                request_id=$(echo "$line" | jq -r '.request.headers."x-request-id"[0] // .request_id // (.ts | tostring | .[0:6])' 2>/dev/null)
            else
                # Fallback parsing without jq
                timestamp=$(echo "$line" | sed -n 's/.*"ts":\([^,}]*\).*/\1/p' | tr -d '"')
                user="admin"
                remote_ip=$(echo "$line" | sed -n 's/.*"remote_ip":"\([^"]*\)".*/\1/p')
                [[ -z "$remote_ip" ]] && remote_ip=$(echo "$line" | sed -n 's/.*"remote_addr":"\([^"]*\)".*/\1/p')
                method=$(echo "$line" | sed -n 's/.*"method":"\([^"]*\)".*/\1/p')
                uri=$(echo "$line" | sed -n 's/.*"uri":"\([^"]*\)".*/\1/p')
                status=$(echo "$line" | sed -n 's/.*"status":\([^,}]*\).*/\1/p')
                duration=$(echo "$line" | sed -n 's/.*"duration":\([^,}]*\).*/\1/p')
                size=$(echo "$line" | sed -n 's/.*"size":\([^,}]*\).*/\1/p')
                request_id=$(echo "$timestamp" | cut -c1-6 2>/dev/null || echo "000000")
            fi
            
            # Format timestamp
            if [[ -n "$timestamp" ]]; then
                if [[ "$timestamp" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
                    # Unix timestamp
                    formatted_time=$(date -d "@${timestamp%.*}" "+%Y-%m-%d %H:%M:%S" 2>/dev/null || echo "$timestamp")
                else
                    # Already formatted or other format
                    formatted_time="$timestamp"
                fi
            else
                formatted_time=$(date "+%Y-%m-%d %H:%M:%S")
            fi
            
            # Format duration - convert nanoseconds to milliseconds with high precision
            if [[ -n "$duration" ]]; then
                # Remove any non-numeric characters except decimal point
                duration_num=$(echo "$duration" | sed 's/[^0-9.]//g')
                if [[ -n "$duration_num" ]]; then
                    # If duration is in nanoseconds (very large number), convert to ms
                    if (( $(echo "$duration_num > 1000000" | bc -l 2>/dev/null || echo 0) )); then
                        duration_ms=$(echo "scale=9; $duration_num / 1000000" | bc -l 2>/dev/null || echo "$duration_num")
                    else
                        duration_ms="$duration_num"
                    fi
                    duration_formatted="$duration_ms ms"
                else
                    duration_formatted="$duration"
                fi
            else
                duration_formatted="- ms"
            fi
            
            # Set default values
            user=${user:-"admin"}
            remote_ip=${remote_ip:-"-"}
            uri=${uri:-"/"}
            status=${status:-"200"}
            size=${size:-"0"}
            request_id=${request_id:-$(date +%s | tail -c 7)}
            
            # Output in the exact format you showed
            printf "%s  %-8s %-15s %-9s %-8s %-25s %-4s %-15s %s\n" \
                "$formatted_time" \
                "$user" \
                "$remote_ip" \
                "REDACTED" \
                "$request_id" \
                "$uri" \
                "$status" \
                "$duration_formatted" \
                "$size"
        fi
    done
}

# Main execution
main() {
    if [[ $# -eq 0 ]]; then
        # Read from stdin
        if tty -s; then
            echo "Usage: $0 [logfile] or pipe logs to stdin"
            echo "Example: tail -f /var/log/caddy/access.log | $0"
            echo "Example: $0 /var/log/caddy/access.log"
            exit 1
        else
            parse_caddy_log
        fi
    else
        # Read from file
        logfile="$1"
        if [[ ! -f "$logfile" ]]; then
            echo "Error: Log file '$logfile' not found"
            exit 1
        fi
        
        parse_caddy_log < "$logfile"
    fi
}

# Check for bc command (needed for duration calculation)
if ! command -v bc >/dev/null 2>&1; then
    echo "Warning: 'bc' command not found. Duration calculations may be inaccurate." >&2
fi

# Run main function
main "$@"
