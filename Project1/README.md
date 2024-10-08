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
The purpose of this experiment is to measure and compare the read and write latency of cache and main memory when the queue length is zero (i.e., with no queuing delay). By understanding these latencies, we gain insights into the efficiency of cache and memory hierarchy in modern computers.

### Uses: 
Open a command terminal in a directory containing the project files, and use the commands below to compile and run the code.  
```
memory_latency.c
```

Compile line:
```
gcc -o memory_latency memory_latency.c -lrt
```
This will complie your file using gcc. The purpose of having the flag "-lrt" is used to link the real-time library for clock_gettime.

Execute line:
```
./memory_latency
```

### How the Script Works:
The way how it works is that there are 2 arrays: one small array (64KB) simulates cache access, and the other one large array (64MB) simulates main memory access.  
The measure_latency() function: Measures the time for reading or writing to the array using clock_gettime() for high-precision timing.  
+ It accesses the array with a stride of 64 bytes to simulate accessing different cache lines.  

Once the program finishes, it will print out the avgerage read/write latency for cache and both memory   

Here are the results after I've run the code  
![Ouput of Memory)Latency](https://github.com/PritomP25/Adanced-Computer-System/blob/main/Project1/memory_lactenmcy_image.png?raw=true)

### Analysis:
The results highlight a significant difference in access latency between cache and main memory. We can see that the read has a faster time than the write operations no matter if its within the cache or main memory.  
To talk more about the latency, we can observed the cache takes less time than the memeory except for the read operation in the memory. This makes me think that for the write latency, the slower write speed in main memory is due to additional operations such as data storage in DRAM cells and memory controller overheads.  
Impact of Cache: These results illustrate the critical role cache plays in optimizing performance. Programs that frequently access data stored in cache lines benefit from faster memory operations, while those reliant on main memory suffer from slower access speeds

## Task 2:
The goal of this experiment is to measure the maximum bandwidth of main memory under different data access granularities (64B, 256B, 1024B) and varying read/write intensity ratios (read-only, write-only, 70:30 read/write, 50:50 read/write).  

### Uses: 
Open a command terminal in a directory containing the project files, and use the commands below to compile and run the code.  
```
memory_bandwidth.c
```

Compile line:
```
gcc -o memory_bandwidth memory_bandwidth.c -lrt
```
This will complie your file using gcc. The purpose of having the flag "-lrt" is used to link the real-time library for clock_gettime.

Execute line:
```
./memory_latency
```
###How the script works
The program will test 64B, 256B, and 1024B access granularities , which simulate different block sizes for memory access.  
For the Read/Write Ratios: there will be 4 different ratios in the terminal output:
+ 100% Read
+ 100% Write
+ 70% Read / 30% Write
+ 50% Read / 50% Write

Bandwidtch Calculation:  
The ```measure_bandwidth()``` function  was calculated as the total number of bytes transferred divided by the time taken, measured in megabytes per second (MB/s).  
Here are the results after I've run the code  
![alt text](https://cdn.discordapp.com/attachments/1019778992779309097/1288691980154441729/image.png?ex=66f61b6e&is=66f4c9ee&hm=850afffece7495f6099125971ea5ef9b1e7bcb9795f3544112e9849bf2647225&)

### Analysis
Effect of Data Granularity: Larger data granularity significantly increased bandwidth. As shown from the image above, there's a long MB/s being reached whne comparing the bandwidth for reads at 64B accesses and 1034B access. This shows that transferring larger blocks at once is more efficient.  

Read vs. Write Performance: Read operations consistently showed slightly higher bandwidth than write operations in general.  

Mixed Read/Write Ratios: As expected, the mixed read/write ratios (70:30 and 50:50) showed performance falling between the pure read and write scenarios.  

## Task 3:
Demonstrates the trade-off between read/write latency and throughput of main memory, as predicted by queuing theory. As the workload (queue depth) increases, memory throughput improves, but latency also increases.  

### Uses: 
Open a command terminal in a directory containing the project files, and use the commands below to compile and run the code.  
```
memory_queue_tradeoff.c
```

Compile line:
```
gcc -o memory_queue_tradeoff memory_queue_tradeoff.c -pthread -lrt
```
This will complie your file using gcc. The purpose of having the flag "-lrt" is used to link the real-time library for clock_gettime. The purpose of "-pthread" is used to link the pthread library for multithreading.  

Execute line:
```
./memory_queue_tradeoff
```

### How the Script Works:
The script uses multiple threads to simulate memory accesses with varying queue depths (1, 2, 4, 8, 16, and 32).  
The metrics measured are the latency (avg time taken per memory access) and the Throughout (total data transferred per second).  
For how the operations is being preformed, both read and write operations were tested over multiple runs.

Here are the results after I've run the code  
![alt text](https://cdn.discordapp.com/attachments/1019778992779309097/1288696501068894239/image.png?ex=66f61fa4&is=66f4ce24&hm=6638d8a0ab904571128bc528acf93ce8346855a32684049e3cefd29726770533&) 

### Analysis:
Queuing Theory Validation: The results did not align with queuing theory, which predicts that higher resource utilization (more threads) leads to greater throughput, but also increases the waiting time (latency) for individual operations. In my code, it actually improves both of the latceny and throughput.

Latency vs. Throughput: As the queue depth increases, throughput improves dramatically, since more memory operations are processed in parallel. But with my code, 
the latency per access also decreases instead of increase (orginially because when it increases, its due to queuing effects and memory contention).

Read vs. Write Performance: Similar trends were observed for both read and write operations, though writes generally exhibited slightly higher latency and lower throughput than reads at the same queue depth.

## Task 4:
The purpose of this experiment is to evaluate the impact of cache miss ratio on the performance of a simple software task, such as matrix multiplication. I aim to demonstrate how increasing cache misses, due to larger data sizes that exceed cache capacity, affects execution time.

### Uses: 
Open a command terminal in a directory containing the project files, and use the commands below to compile and run the code.  
```
matrix_cache_ratio.c
```

Compile line:
```
gcc -o matrix_cache_ratio matrix_cache_ratio.c - -O2
```

Execute line:
```
./matrix_cache_ratio
```

### How the Script Works:
I try to aim for a matrix of small vs big (512x512 vs 2048x2048) that will measured the exceution time. Note, because perf didn't work on my laptop when using Ubantu, I couldn't be able to check the Cache Miss Ratio %.

Here are the results after I've run the code  
![alt text](https://cdn.discordapp.com/attachments/1019778992779309097/1288702171994656839/image.png?ex=66f624ec&is=66f4d36c&hm=c1f0c19ebd73a4a092bb40b11a33dc52ff590e8374819fb69eec3ff45a75a9bc&)

### Analysis:
The larger matrix it is, then the more significant increases in the execution time. This is my inferences that while there's a higher execution time, I predict that there's also a increase in cache miss ratio. So this could represents of how the cache performance directly affects the speed of the computation tasks. 


## Task 5:
The objective of this experiment is to analyze the impact of TLB (Translation Lookaside Buffer) miss ratio on software performance while executing light computations (such as matrix multiplication).

### Uses: 
Open a command terminal in a directory containing the project files, and use the commands below to compile and run the code.  
```
matrix_tlb_test.c
```

Compile line:
```
gcc -o matrix_tlb_test matrix_tlb_test.c - -O2
```

Execute line:
```
./matrix_tlb_test
```

### How the Script Works:
Similar as task 4, we created a small and large matrix of the same size. But the code will now run for the TLB misses. Note, because perf didn't work on my laptop when using Ubantu, I couldn't be able to check the Cache Miss Ratio %.

Here are the results after I've run the code  
![alt text](https://cdn.discordapp.com/attachments/1019778992779309097/1288704910988283956/image.png?ex=66f62779&is=66f4d5f9&hm=b354f6a41b494c4115f289089e71bda56d56f4e8e72a3fbb64baad777e39800b&)

Seeing from the results, as I expected that the smaller matrix will run faster than the large matrix.

### Analysis:
The larger matrix it is, then the more significant increases in the execution time. This is my inferences that while there's a higher execution time, I predict that there's also a increase in TLB miss ratio. As my infrences, due to there a large TLB  missses adn the corresponding increases in execution time for larger datasets. Therefore, the higher TLB miss ratios indicicate, the more time spend in the page lookups, which will slows down the computation speed. 
