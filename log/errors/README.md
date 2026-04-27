# Error Log Directory

This directory contains error logs for the HSI Measurement System.

## Overview

Error logs are automatically created and maintained by the ErrorLogger system:
- **Location**: `log/errors/`
- **File Format**: `errors_YYYYMMDD.log` (daily log files)
- **Auto-rotation**: New file created automatically each day
- **Append Mode**: Errors are appended to existing daily log file

## Log File Format

Each error entry follows this format:

```
[DD.MM.YYYY HH:MM:SS.mmm] [CATEGORY] [0xCCEEEE] Error Message | Details: Context information
```

### Example Entries

```
[27.04.2026 14:23:45.123] [CAMERA] [0x050502] Failed to initialize camera device | Details: USB timeout
[27.04.2026 14:24:10.456] [LASER] [0x060605] Laser temperature warning | Details: Current: 45°C, Max: 40°C
[27.04.2026 14:25:30.789] [STAGE] [0x090905] Stage out of bounds | Details: X: 150mm, Max: 100mm
```

## Error Categories

| Code | Category | Prefix |
|------|----------|--------|
| 0x01 | SYSTEM | [SYSTEM] |
| 0x02 | FRONTEND | [FRONTEND] |
| 0x03 | BACKEND | [BACKEND] |
| 0x04 | FB_CONNECTOR | [FB_CONNECTOR] |
| 0x05 | CAMERA | [CAMERA] |
| 0x06 | LASER | [LASER] |
| 0x07 | SPECTROGRAPH | [SPECTROGRAPH] |
| 0x08 | POWERMETER | [POWERMETER] |
| 0x09 | STAGE | [STAGE] |
| 0x0A | FILE_IO | [FILE_IO] |
| 0x0B | CONFIG | [CONFIG] |
| 0x0C | MEASUREMENT | [MEASUREMENT] |

## Usage Examples

### View Today's Errors
```bash
cat errors_20260427.log
```

### Search for Camera Errors
```bash
grep "CAMERA" errors_20260427.log
```

### Search for Specific Error Code
```bash
grep "0x050502" errors_*.log
```

### Count Errors by Type
```bash
grep -o "\[.*\]" errors_20260427.log | sort | uniq -c
```

### View Last 20 Errors
```bash
tail -20 errors_20260427.log
```

### Find Critical Errors
```bash
grep "temperature\|disconnected\|failed" errors_*.log
```

## Maintenance

### Automatic Cleanup (Optional)

Delete logs older than 30 days:
```bash
find . -name "*.log" -mtime +30 -delete
```

### Archive Old Logs

Create archive of last month's logs:
```bash
tar -czf errors_2026_03.tar.gz errors_202603*.log
rm errors_202603*.log
```

### Monitor Log Directory Size

```bash
du -sh .
```

## Monitoring

### Enable/Disable Error Logging
Use the Admin Tab in the application:
1. Open the Admin Tab
2. Toggle "Log errors to file" checkbox
3. Errors will only be logged when checkbox is enabled

### Real-time Monitoring
The Admin Tab displays:
- Current error status
- Last error with timestamp
- Error history (last 20 errors)
- Current log directory path

### Remote Monitoring
Access log files via:
- File explorer: `log/errors/`
- SSH/SCP (if on remote server)
- Log aggregation systems

## Troubleshooting

### Logs Not Being Created
1. Check `log/errors/` directory exists
2. Verify write permissions on directory
3. Check Admin Tab logging checkbox is enabled
4. Verify disk space is available
5. Check file system is not read-only

### Log Files Too Large
1. Delete old log files manually
2. Archive logs periodically
3. Implement automated cleanup script
4. Reduce error frequency in application

### Cannot Read Logs
1. Ensure UTF-8 encoding support
2. Use text editor supporting the format
3. Use command-line tools: `cat`, `grep`, `tail`
4. Verify file permissions are readable

## Log Retention Policy (Recommended)

- **Keep logs for**: 30 days (minimum)
- **Archive logs after**: 30 days to separate storage
- **Delete archived logs**: After 6 months
- **Backup critical logs**: Important failures should be backed up

## Integration with Monitoring Systems

### ELK Stack (Elasticsearch, Logstash, Kibana)
1. Configure Logstash to read from `log/errors/`
2. Parse logs using the timestamp and category format
3. Index in Elasticsearch
4. Visualize in Kibana

### Splunk
1. Add `log/errors/` as a log source
2. Set sourcetype to `hsimeasurement:error`
3. Configure search and alerts

### CloudWatch (AWS)
1. Use CloudWatch agent to monitor `log/errors/`
2. Create metrics and alarms
3. Set up notifications

## Backup Recommendations

Important error logs should be backed up:
```bash
# Backup to external drive
cp errors_*.log /mnt/backup/hsimeasurement/errors/

# Backup to cloud storage
aws s3 sync . s3://backup-bucket/hsimeasurement/errors/
```

## Analysis Tools

### Generate Daily Report
```bash
#!/bin/bash
FILE="errors_$(date +%Y%m%d).log"
echo "=== Error Report for $(date +%Y-%m-%d) ==="
echo "Total Errors: $(wc -l < $FILE)"
echo ""
echo "Errors by Category:"
grep -o "\[.*\]" $FILE | sed 's/\[\(.*\)\]/\1/' | sort | uniq -c
echo ""
echo "Last 5 Errors:"
tail -5 $FILE
```

### Generate Weekly Summary
```bash
#!/bin/bash
echo "=== Weekly Error Summary ==="
for i in {6..0}; do
    DATE=$(date -d "-$i days" +%Y%m%d)
    FILE="errors_$DATE.log"
    if [ -f "$FILE" ]; then
        COUNT=$(wc -l < $FILE)
        echo "$DATE: $COUNT errors"
    fi
done
```

## Performance Impact

- Log file operations: <1ms per entry
- File I/O: Non-blocking, async
- UI updates: 500ms refresh interval
- Memory usage: ~50KB for 1000 error entries
- Disk usage: ~1MB per day (typical)

## Support

For issues or questions about error logging:
1. Check `ERROR_LOGGING_GUIDE.md` for detailed documentation
2. Review `ERROR_LOGGING_SUMMARY.md` for implementation details
3. Examine `EXAMPLE_ERROR_INTEGRATION.h` for code examples
4. Check Admin Tab for real-time error monitoring

---

**Created**: April 27, 2026  
**System**: HSI Measurement System  
**Version**: 1.0
