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

In this section, I will talk about the relationship between the queue depth and the latency. I've decided to held the read/write ratio constant at 50:50 because no matter what intensity ratio I used, the results will be same, so I thought it will be intereseting to see the graph at Read and Write ratio to be 50/50 split. From the grpah above, I've 4 legends for different data access sizes. As seen in the graph, as the queue length increases, the more latency increases. Looking at the Data table, not only the latency increases, but also the bandwidth incrases. This support the result of the queuing theory, that stated from the assingment, "As you increase the storage access queue depth (hence increase data access workload stress),
SSD will achieve higher resource utilization and hence higher throughput, but meanwhile the latency of each data access request will be longer." As such, my experiment proves this theory. 


## Bandwidth Measurements
Bandwidth throughout this experiment will be measured in (Mb/s) espically for large data size, while as (xk) for IOPS of small data size.

### Effect of data access size on bandwidth:
![Experiment 4](https://github.com/PritomP25/Adanced-Computer-System/blob/aaa60461a8fe796df7ca47ab76b784f54dcb9d06/Project3/Images/Data%20Access%20Size%20vs.%20Bandwidth%20at%20READ%20Only.png)



### Effect of read/write intensity ratio on bandwidth:
![Experiment 5](https://github.com/PritomP25/Adanced-Computer-System/blob/2c507f3bcb66c626550d48c7dd67b2767d1472b8/Project3/Images/R_W%20Ratio%20vs%20Bandwidth%20at%20I_O%20queue%20depth%20of%2064.png)



### Effect of I/O queue depth on bandwidth:
![Experiment 6](https://github.com/PritomP25/Adanced-Computer-System/blob/c2e709e693d55d40e4b7d42ce1df3b8bd7da5893/Project3/Images/I_O%20Queue%20Depth%20vs.%20Bandwidth%20at%2050%25_50%25%20R_W%20Ratio.png)



## Conclusion





