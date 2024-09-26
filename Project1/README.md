# Course Project #1: Cache and Memory Performance Profiling

The objective of this project is to gain deeper understanding of cache and memory hierarchy in modern computers.
You should design a set of experiments that will quantitatively reveal the following:
(1) the read/write latency of cache and main memory when the queue length is zero (i.e., zero queuing delay)
(2) the maximum bandwidth of the main memory under different data access granularity (i.e., 64B, 256B, 1024B) and different read vs. write intensity ratio (i.e., read-only, write-only, 70:30 ratio, 50:50 ratio)
(3) the trade-off between read/write latency and throughput of the main memory to demonstrate what the queuing theory predicts
(4) the impact of cache miss ratio on the software speed performance (the software is supposed to execute relatively light computations such as multiplication)
(5) the impact of TLB table miss ratio on the software speed performance (again, the software is supposed to execute relatively light computations such as multiplication)
_____________________________________________________________________________________________________________________________________________
## Task 1:
Uses: Open a command terminal in a directory containing the project files, and use the commands below to compile and run the code.
memory_latency.c

Compile line:
gcc -o memory_latency memory_latency.c -lrt
This will complie your file using gcc. The purpose of having the flag "-lrt" is used to link the real-time library for clock_gettime.

Execute line:
./memory_latency

How the Script Works:
The way how it works is that there are 2 arrays: one small array (64KB) simulates cache access, and the other one large array (64MB) simulates main memory access.
The measure_latency() function: Measures the time for reading or writing to the array using clock_gettime() for high-precision timing.
        It accesses the array with a stride of 64 bytes to simulate accessing different cache lines.
Once the program finishes, it will print out the avgerage read/write latency for cache and both memory 
