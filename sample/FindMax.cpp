#include <thread>
#include "FindMax.h"


extern std::atomic<bool> runThreads;


FindMax::FindMax()
{
  //ctor
}


void FindMax::run(VectorOfQueues<float> & iRandomNumbers, Queue<float> &oMaxNumbers)
{
  std::string funcName = "FindMax::run(...)";

  while(runThreads)
  {
    float maxVal = 0;
    TimestampedData<float> randomNumber;
    std::vector<TimestampedData<float> > randomNumbers;
    TimestampedData<float> newEntry;

    // wait for new data
    randomNumbers = iRandomNumbers.backButWaitNewData(_lastDataTimestamp);

    // find max
    for (unsigned int i=0; i<randomNumbers.size(); ++i)
    {
      if (randomNumbers.at(i)._data > maxVal)
      {
        maxVal = randomNumbers.at(i)._data;
      }
    }

    // write data
    newEntry._timestamp = _lastDataTimestamp;
    newEntry._data = maxVal;

    // output to console
//    newEntry.printDataAndLatency(funcName);

    // write data
    oMaxNumbers.push_back(newEntry,10);

    std::this_thread::yield();
  }
}


FindMax::~FindMax()
{
  //dtor
}
