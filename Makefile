all:
	make clean
	make clock_precision_1
	make clock_precision_2
	make kernel_call_1
	make kernel_call_2
	make socketpair_latency
	make socketpair_throughput

clean:
	rm -f clock_precision_1 clock_precision_2 kernel_call_1 kernel_call_2 socketpair_latency socketpair_throughput

clock_precision_1:
	gcc clock_precision_1.c -o clock_precision_1

clock_precision_2:
	gcc clock_precision_2.c -o clock_precision_2

kernel_call_1:
	gcc kernel_call_1.c -o kernel_call_1

kernel_call_2:
	gcc kernel_call_2.c -o kernel_call_2

socketpair_latency:
	gcc socketpair_latency.c -o socketpair_latency

socketpair_throughput:
	gcc socketpair_throughput.c -o socketpair_throughput