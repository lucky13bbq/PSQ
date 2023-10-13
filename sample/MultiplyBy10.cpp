#include <thread>
#include "MultiplyBy10.h"

extern std::atomic<bool> runThreads;


MultiplyBy10::MultiplyBy10()
{
  // ctor
}


void MultiplyBy10::run(Queue<float> &iMaxNumbers, Queue<float> &oMultipliedBy10)
{
  std::string funcName = "MultiplyBy10::run(...)";

  while(runThreads)
  {
    TimestampedData<float> maxNumber, newEntry;

    // wait for new data
    maxNumber = iMaxNumbers.backButWaitNewData(_lastDataTimestamp);

    // process input data
    newEntry._timestamp = _lastDataTimestamp;
    newEntry._data = maxNumber._data * 10.0;

    // output to console
//    newEntry.printDataAndLatency(funcName);
//    this->threadFrequency(funcName,0,true);

    oMultipliedBy10.push_back(newEntry,10);

    std::this_thread::yield();
  }
}


MultiplyBy10::~MultiplyBy10()
{
  // dtor
}
