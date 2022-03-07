ulimit -c unlimited

echo "core.%e.%p" > /proc/sys/kernel/core_pattern

