# PSQ
A multithreaded Pipeline based on Shared Queues C++ header-only library

Has no dependencies and consists of only four header files:
1. TimeStampedData.h
2. Queue.h
3. VectorOfQueues.h
4. BaseThread.h

# Overview
Was looking for an easy way to process data from multiple sensors or cameras in a flexible multithreaded C++ pipeline. While other pipeline libraries exist such as YAPP (https://github.com/picanumber/yapp), we sought a solution with flexible pipeline configurations that would allow a) multiple threads (e.g. sensors, cameras) to write to a common vector of queues and b) subsequent threads to read data generated and provided by one of more input threads. 

As such, our multithreaded pipeline is based on shared queues or shared vector of queues to share data between threads. To signal new data, we use condition variables to notify and wake the waiting thread(s). Note: A thread should only wait on one condition variable so in the case of a thread that reads from multiple threads, you will need to choice which thread it should wait on.

## Sample Code
To see how we can use this library, please see the provided sample code in which we implement the following simplified pipeline:

![FlowDiagram](https://github.com/lucky13bbq/PSQ/assets/1645316/cb2f8391-31bd-4d4e-9c69-c9019d799510)

You may use CodeBlocks to compile and run.
