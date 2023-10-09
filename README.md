# PSQ: A C++ multithreaded Pipeline based on Shared Queues

A small C++ template library that has no 3rd party dependencies and consists of only four header files:
1. TimestampedData.h
2. Queue.h
3. VectorOfQueues.h
4. BaseThread.h

# Disclaimer
I am not a C++ concurrency expert so please use at your own risk and do let me know what can be improved. This is also my first github repo post ever so please excuse me in advance for any faux pas.

# Overview
Was looking for an easy way to process live data from multiple sensors or cameras in a flexible multithreaded C++ pipeline. While other pipeline libraries exist such as YAPP (https://github.com/picanumber/yapp), we sought a solution with flexible pipeline configurations that would allow a) multiple threads (e.g. sensors, cameras) to write to a common vector of queues, if desired, and allow b) subsequent threads to read data from one or more input threads that they could use in turn to generate new data.

As such, our multithreaded pipeline is based on shared queues or shared vector of queues to share data between threads. To signal new data, we use condition variables to notify and wake the waiting thread(s). Using shared mutexes, multiple readers can share a lock and don't block eachother from reading a queue. However only one thread may write to or modify a queue at any time via an exclusive lock.

# Notes
* A thread should only wait on one condition variable so in the case of a thread that reads from multiple threads, you will need to choice which thread it should wait on.
* Data are timestamped as soon as they are recorded by the sensor or camera. New data are always pushed to the back of the queue.
* Threads are supposed to stop waiting and wake up when new timestamped data are available.
* We only pop the front of the queue (i.e. remove elements in the queue) during write operations, when writers have exclusive lock.
* There is no swap() function available (which would be more efficient than copyQueue*()) because the assumption here is that multiple readers want to access the same data (hence the shared_mutex). If you only have a single consumer, then use a regular mutex and use a swap instead of a copy instead.

# Usage Tips
* User defined threads should inherit from BaseThread, see sample code provided
* Call Queue::push_back() or VectorOfQueues::push_back() to write new data and notify other threads
* Call Queue::back(), Queue::copyQueue(), VectorOfQueues::back(), VectorOfQueues::copyVector() to read data
* Call Queue::backButWaitNewData(), Queue::copyQueueButWaitNewData(), VectorOfQueues::backButWaitNewData(), VectorOfQueues::copyVectorButWaitNewData() to wait for and read new data when available

## Sample Code
To see how you could use this library, please see the provided sample code in which we implement the following simplified pipeline:

![FlowDiagram](https://github.com/lucky13bbq/PSQ/assets/1645316/f2710566-a85d-4ba7-a64b-1fa8ac95e643)

Each box represents a thread. Multiple random number generators write to a common vector of queues. Subsequent threads use these data to generate new data that they write to their own output queues, except for Viewer which does not generate any data. If desired, you may change the random number generation frequency to match your sensor frequency as desired e.g. 30Hz. 

You may use CodeBlocks to open the project file, compile and run. The program will run for 10 seconds, output various info to the console such as the overall latency of the data that are propagated through the pipeline from start to finish, then exit.

Good luck and hope this helps someone!
