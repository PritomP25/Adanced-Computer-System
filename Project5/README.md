# Final Project: Video Compression via Multi-threading & SIMD Instructions

## Overview
Video compression is the process of reducting the size of video files by eliminating reduandant or non-essential data while preserving visual quality. Any video is breaking down into indiviual frames or images and worked into threads to reduced resolution. This is acheived through techniques such as spatial compression (reducing image resolution or complexity within each frame) and temporal compression (removing redundant information across consecutive frames). Efficient compression is crucial for optimizing storage and transmission, especially in applications like streaming, archiving, and real-time video communication.

**Project Purpose**: The purpose of this project is to implement a spatial video compression pipeline that supports baseline processing, multi-threading, and SIMD optimizations. By leveraging these techniques, the project aims to explore the trade-offs between execution speed, computational efficiency, and video quality, providing a comparative analysis of different optimization strategies for video compression tasks.

## Code Structure
There is a single file which is `proj5.cpp`: 
- Reads in the video file (mp4) and then output the compressed_video into your workspace
- Includes the necessary libraies & data types for the fucntions
- Includes No optimization, multi-threading via parallelism, and SIMD Instruction functions depending on the user input

|     Component             |   Purpose   |  Usage  |
|-------------------|-------------|---------|
|compressFrame              |	Baseline compression (no optimization) |	Standalone function |
| processVideoMultiThreaded |	Enables multi-threaded processing using producer-consumer model |	Separate function, uses threads |
|     compressFrame_SIMD    |	Implements SIMD optimization for pixel processing |	Standalone function |
| main	| Controls workflow and integrates functionalities |	Main program logic |
----------------------------------------------------------------------------------

## Setup Intructions
There are 2 necessary librabies that needed to install to run my program.
1. OpenCV library is nesessary to compile program otherwise, it wouldn't compile. You can follow the insturctions to download OpenCV in WSL/Ubuntu: https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html
2. `ffmpeg` is also neceessary to install as without it, it couldn't support the codec id 7 and format 'mp4 / MP4 (MPEG-4 Part 14)' video format. On Ubuntu, you can install this by running these command lines: 
```
sudo apt update  
sudo apt install ffmpeg
```

## Usage/Compile
Open your command terminal in a directory that contain 
Open a command terminal in a directory containing the `proj5.cpp` files as well as your <filename>.mp4 file, e.g. my_video.mp4, and use the commands below to compile and run the code.

**Compile Line**:  
`g++ -O3 -march=native -mavx2 proj5.cpp -o proj5.out -lopencv_core -lopencv_videoio -lopencv_imgproc -I/usr/include/opencv4`  
Note, if you are unable to complie, then you can use the excutable file `proj5.out` to run your test  

**Execute line**:  
`./proj5.out <filename>.mp4 <num_threads> <method_enable>`  

**Arguments**:  
`<filename>.mp4`: Your mp4 video file used as your input  
`<num_threads>`: Specfiy the # of threads you want to used when applying the multi-threading in the worker threads  
`<method_enable>`: Set `0` for No-optimization, Set `1` for Multi-threading, Set `2` for SIMD Instructions

Example Execute line:
```
./proj5.out dance.mp4 1 0
./proj5.out dance.mp4 4 1
./proj5.out dance.mp4 4 2
```  
Note, invalid method_enable other than 0, 1, and 2 will result into usuing No_Optimization option

## Data Result:
I used a personal video (1:35 min) from a Wedding event of my friends/family dancing that I've attended during the Fall sesmter. For the privacy reasons, I will not used include that video into this project as it a personal video for the event so I do not want to share them online.  

**Video Compression Excutation Time Table**:  
| Number of Threads | No Optimization Time (s) | Multi-Threading Time (s) | SIMD Instruction Time (s) |
|-------------------|--------------------------|--------------------------|---------------------------|
| 1 | 45.1035 | 41.1151 | 6.18483 |
| 2 | 42.8145 | 40.5398 | 5.98023 |
| 4 | 43.9711 | 41.1241 | 6.24823 |
| 8 | 43.0576 | 40.9155 | 6.0423 | 

![Video Compression Table](https://github.com/PritomP25/Adanced-Computer-System/blob/338bfceb1f044b50662c9986f55fcc4e62f94d8b/Project5/Processing_table.PNG)


## Analysis 
From the data, we can see that SIMD instruction has a fastest processing time to compress the video. It was within standard that from the fastest processing time, it would be SIMD instructions > Multi-threading > No optimization which aline with the results based on previous project experiments as well as in-class. The longest processing time by far was the No optimization when using hte OpenCV's resize method to compressed the video algothim. There is a problem with my code that while multi-threading does result in a little faster processing time, its not the ideal scencior for it. In which, over time I will try to optimize my multi-threading code because in the ideal scenior,, increasing the number of threads will reduced the processing time, though it probably wouldn't be the fastest compared to enabling SIMD instructions. 

## Conclusion
The advantages of using SIMD instructions and Multi-threading via parallelism for compressing video are clear. Based on our experiment, in the ideal world, the results indicate that using these techniques can reduced the processing time required for longer videos or videos with a higher frame rate. Since may streaming services like youtube, Netflix, etc requires to compression the video when uploading it, its important that longer videos are process in shorter time and contain their resolution quality for viewers to watched. Overtime, while my code doesn't fully represenat the true ideal world for multi-threading, we still have shown that any optimization will result in faster processing time compared to no optimization.

In summary, SIMD instructions are faster because they process multiple data elements simultaneously within a single CPU instruction, leveraging data-level parallelism. This is highly efficient for operations like resizing or filtering, where the same operation is applied repeatedly across pixels. In contrast, multi-threading divides the workload among threads but incurs overhead from thread management and synchronization. No optimizations process frames sequentially, which lacks both parallelism and data-level efficiency. Thus, SIMD reduces computation time significantly by directly utilizing the CPU's vectorized instruction set.
