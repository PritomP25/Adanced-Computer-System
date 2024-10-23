# Course Project #3: SSD Performance Profiling

## Command Prompt
Throughout this project, I've used Windows PowerShell and FIO to work on this project  
For each of the commands to run the READ, WRITE, 50/50 R/W, and 70/30 R/W commonad is from this following respectally:  
```
fio --name=128k-read --ioengine=windowsaio --rw=read --bs=128k --size=1G --numjobs=1 --iodepth=1 --direct=1 --thread --output=128k_read_qd1.txt
fio --name=128k-write --ioengine=windowsaio --rw=write --bs=128k --size=1G --numjobs=1 --iodepth=1 --direct=1 --thread --output=128k_write_qd1.txt
fio --name=128k-randrw --ioengine=windowsaio --rw=randrw --rwmixread=50 --bs=128k --size=1G --numjobs=1 --iodepth=1 --direct=1 --thread --output=128k_rw_qd1.txt
fio --name=128k-randrw --ioengine=windowsaio --rw=randrw --rwmixread=70 --bs=128k --size=1G --numjobs=1 --iodepth=1 --direct=1 --thread --output=32k_70_30_rw_qd1.txt
```

Explanation:  
- --name=128k-read: Names the test for identification. Can change the name to anything.
- --ioengine=windowsaio: Uses the asynchronous I/O engine specific to Windows.
- --rw=read: Specifies a read-only workload. Type in write for write only or use randrw for R/W ratio
- --rwmixread=50: Specifies that 50% of operations will be reads and 50% (or any percentage) will be writes. NOTE: you have to use this if using --rw=randrw
- --bs=128k: Block size. Can change to any size for the data access size.
- --size=1G: Runs the test on 1GB of data.
- --numjobs=1: Runs a single job.
- --iodepth=1: Sets the I/O queue depth to 1. Can change to any number you want
- --direct=1: Bypasses the OS cache for direct disk access.
- --output=128k_read_qd1.txt: Redirects the output to a file for later analysis. Can change the name to anything.

## Results and Analysis
![Data Table used from this experiment](https://github.com/PritomP25/Adanced-Computer-System/blob/f896f5f1ba2b5e96a08352e0fe40acee16102df8/Project3/Images/Full_Data.PNG)
The table above shows all the data collected from the all of the possible experiments.

## Latency Measurements
Latency throughout this experiment will be measued in micro-seconds (us).

### Effect of data access size on latency: 
![Experiment 1](https://github.com/PritomP25/Adanced-Computer-System/blob/86b966c1e909a9f30383d4ba60b5b1b223c9ae52/Project3/Images/Data%20Access%20Size%20vs.%20Latency%20at%20READ%20only.png)

For this experiment, I will talk about the relationship between data access size and latency. Wheather than showing all of the read vs. write intensity ratio, I've decide to show the READ only graph because in the end, the explanatation will be the same regardless of whatever read vs. write intensity ratio I've chosen. In other words, I choose to fix the read/write ratio size to be READ only (aka 100:0) for easier usages. As shown in the graph, there are 4 legends included to showncase different queue lengths. Note, the blue legend is presented, however its very small compared to the other legends. We can see from the graph, as the data access size increases, the latency also increases. This make sense because when the file is either reading or writing the data one at a time, it going to take longer time to complete the operation when there's more data to go to. 


### Effect of read/write intensity ratio on latency:
![Experiment 2](https://github.com/PritomP25/Adanced-Computer-System/blob/8f6518b7f4e6d54726214ca7d4ffc72ed0032ff2/Project3/Images/R_W%20Ratio%20vs%20Latency%20at%20I_O%20queue%20depth%20of%2064.png)

In this section, I will talk about the relationship between read/write intensity ratio and the latency. I've decided to held the queue length constant at 64 because throughout my experiment, I/O queue length at 64 is an interesting data point I've seen as that seem to have a big jump in the latency. From the grpah above, I've included 4 legends for different data access sizes. When loojing at the graph, the grpah will continue to increase until it rest its climax at 50%, which will decrease until the READ is 100%. As such, the latency is at the loweset when its either at READ only or WRITE only, with writes taking longer amount of time because writing to the SSD takes more resources than just reading. 


### Effect of I/O queue depth on latency:
![Experiment 3](https://github.com/PritomP25/Adanced-Computer-System/blob/926fc99f5288cc4e97e145e71ce34244dbc78e35/Project3/Images/I_O%20Queue%20Depth%20vs.%20Latency%20at%2050%25_50%25%20R_W%20Ratio.png)

In this section, I will talk about the relationship between the queue depth and the latency. I've decided to held the read/write ratio constant at 50:50 because no matter what intensity ratio I used, the results will be same, so I thought it will be intereseting to see the graph at Read and Write ratio to be 50/50 split. From the grpah above, I've 4 legends for different data access sizes. As seen in the graph, as the queue length increases, the more latency increases. Looking at the Data table, not only the latency increases, but also the bandwidth incrases. This support the result of the queuing theory, that stated from the assingment, "As you increase the storage access queue depth (hence increase data access workload stress), SSD will achieve higher resource utilization and hence higher throughput, but meanwhile the latency of each data access request will be longer." As such, my experiment proves this theory. 


## Bandwidth Measurements
Bandwidth throughout this experiment will be measured in (Mb/s) espically for large data size, while as (xk) for IOPS of small data size.

### Effect of data access size on bandwidth:
![Experiment 4](https://github.com/PritomP25/Adanced-Computer-System/blob/aaa60461a8fe796df7ca47ab76b784f54dcb9d06/Project3/Images/Data%20Access%20Size%20vs.%20Bandwidth%20at%20READ%20Only.png)

For this experiment, I will talk about the relationship between data access size and bandwidth. Wheather than showing all of the read vs. write intensity ratio, I've decide to show the READ only graph because in the end, the explanatation will be the same regardless of whatever read vs. write intensity ratio I've chosen. In other words, I choose to fix the read/write ratio size to be READ only (aka 100:0) for easier usages. As shown in the graph, there are 4 legends included to showncase different queue lengths. Note, the blue legend is presented, however its very small compared to the other legends. We can see from the graph, as the data access size increases, the bandwidth also increases. This is because having a wide bandwidth would allow you to transmit multiple data channels at the smae time that window allowed you to use. As such, when there's less time setting up the IO operation for the data access size, the amount of data can be processed is increased. From the graph, we can see that the relatinship is not linear, and the I/O queue depth lines for 128, 512, 1024 is simialr to each other is probably due to the hardware of my laptop reaching its bandwidth limit where from the data table, my guess for the upper limit is probably from the range of 128-256 queue depth because of how you can see the bandwidht dropping.


### Effect of read/write intensity ratio on bandwidth:
![Experiment 5](https://github.com/PritomP25/Adanced-Computer-System/blob/2c507f3bcb66c626550d48c7dd67b2767d1472b8/Project3/Images/R_W%20Ratio%20vs%20Bandwidth%20at%20I_O%20queue%20depth%20of%2064.png)

In this section, I will talk about the relationship between read/write intensity ratio and the bandwidth. I've decided to held the queue length constant at 64 because throughout my experiment, I/O queue length at 64 is an interesting data point I've seen as that seem to have either a increased or decrease in the bandwidth. From the grpah above, I've included 4 legends for different data access sizes. Looking at the graph, as the read % increases, the bandwidth also increases, expectp for the 128KB acces size where its decreases after 50% but then increases at 75% read ratio. What I think is that since reading data doesn't take that much resources, it can read more data which in return have a higher bandwidth data to be processed. 
When looking at the graph, the graph will continue to increase until it rest its climax at 50%, which will decrease until the READ is 100%. As such, the latency is at the loweset when its either at READ only or WRITE only, with writes taking longer amount of time because writing to the SSD takes more resources than just reading. 


### Effect of I/O queue depth on bandwidth:
![Experiment 6](https://github.com/PritomP25/Adanced-Computer-System/blob/c2e709e693d55d40e4b7d42ce1df3b8bd7da5893/Project3/Images/I_O%20Queue%20Depth%20vs.%20Bandwidth%20at%2050%25_50%25%20R_W%20Ratio.png)

In this section, I will talk about the relationship between the queue depth and the bandwidth. I've decided to held the read/write ratio constant at 50:50 because no matter what intensity ratio I used, the results will be same, so I thought it will be intereseting to see the graph at Read and Write ratio to be 50/50 split. From the grpah above, I've 4 legends for different data access sizes. As seen in the graph, as the queue length increases, the bandwisth intitallu start to increase, but then quickly decreases, and then reach a steady state after a certain point. For when the bandwidth incrases from the intial starting line, it was as expected since the increased in queue length would increase the usages not only for the memeory but as well as the hardware to ensure that there's space to work for the SSD. Each graph have thier own steady state is due to the point that the its trying to avoid I/O request failueres  that could lead to bottlenecks. Due to this it will probablay leads to delays in the lactency and not increased in perforamce. 

## Conclusion

Comparing my results to the Intel Data Center NVMe SSD D7-P5600, which has a random write-only 4KB IOPS of 130K. I have a few thoughts on some key differences between higher IOPS on Client-Grade SSDs. 
1. Targeted optimization for client workloads: Client-grade SSDs in my experiments are usually designed for lower queue depths and consumer workloads. These SSDs can sometimes achieve higher IOPS under certain conditions (particularly low queue depths) than enterprise-grade SSDs, which are designed for more sustained, high-performance environments with higher queue depths and consistent performance over time.
2. Client-grade: SSDs frequently employ aggressive caching techniques, such as DRAM and SLC caches, to improve short-term performance. These caches store incoming data quickly before gradually writing it to the main NAND storage. This caching can result in higher IOPS in shorter tests like yours, but in longer workloads (such as those in data centers), performance suffers as the cache fills up and the SSD must write directly to NAND.
3. Impact of Queue Depth: Client-grade SSDs may perform comparably, if not better, than data center SSDs at lower queue depths (such as 1 to 32), because they are designed to handle such workloads more efficiently. The Intel Data Center SSD, on the other hand, is designed to perform well at higher queue depths (64, 128, 256+), which are typical for enterprise workloads. If you notice higher IOPS at queue depths less than 32 or 64, this could explain why. However, as the queue depth increases beyond 64, enterprise-grade SSDs will generally outperform, as they are designed to handle a greater number of concurrent requests.

In conclusion, while my client-grade SSD may have higher IOPS under certain conditions, such as lower queue depths or short durations, the Intel Data Center NVMe SSD D7-P5600 is intended for long-term reliability, consistency, and sustained performance in enterprise environments. The Intel SSD's design priorities, which include endurance, power loss protection, and improved performance under heavy concurrent workloads, explain why its peak IOPS may appear lower in this specific metric than your client-grade SSD.
