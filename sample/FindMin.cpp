#include <thread>
#include "FindMin.h"

extern std::atomic<bool> runThreads;


FindMin::FindMin()
{
  //ctor
}


void FindMin::run(VectorOfQueues<float> & iRandomNumbers, Queue<float> &oMinNumbers)
{
  std::string funcName = "FindMin::run(...)";

  while(runThreads)
  {
    float minVal = std::numeric_limits<float>::max();
    TimestampedData<float> randomNumber;
    std::vector<TimestampedData<float> > randomNumbers;
    TimestampedData<float> newEntry;

    // wait for new data
    randomNumbers = iRandomNumbers.backButWaitNewData(_lastDataTimestamp);

    // find min
    for (unsigned int i=0; i<randomNumbers.size(); ++i)
    {
      if (randomNumbers.at(i)._data < minVal)
      {
        minVal = randomNumbers.at(i)._data;
      }
    }

    // prepare data
    newEntry._timestamp = _lastDataTimestamp;
    newEntry._data = minVal;

    // output to console
//    newEntry.printDataAndLatency(funcName);

    // write data
    oMinNumbers.push_back(newEntry,10);

    std::this_thread::yield();
  }
}


FindMin::~FindMin()
{
  //dtor
}
