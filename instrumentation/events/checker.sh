
# grep -h -r -P '^LTTNG_TRACEPOINT_EVENT[\w_]*?\(\s*([\w_]+?),' . | grep -F -v 'CLASS' | sort

for i in $(find ./lttng-module | sort); do ./check.pl "$i" ; done

# also try | sort | uniq
