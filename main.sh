#!/bin/bash

set -e

tz_offset_str=$(date +%z)
tz_offset_hours=${tz_offset_str:0:3}
tz_offset=$((tz_offset_hours * 3600))

year=$(date +%Y)
jan_1="${year}-01-01 00:00:00"
jan_1_ts=$(date -jf "%Y-%m-%d %H:%M:%S" "$jan_1" +%s)

year_progress_secs=$(( $(date +%s) + $tz_offset - $jan_1_ts ))

if (( (year % 4 == 0 && year % 100 != 0) || year % 400 == 0 )); then
    year_length_days=366
else
    year_length_days=365
fi

year_length_secs=$(($year_length_days * 86400))

year_progress_pct=$(echo "scale=9; $year_progress_secs / $year_length_secs * 100" | bc)

printf "%.7f\n" $year_progress_pct
