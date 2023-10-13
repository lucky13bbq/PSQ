#include <thread>
#include "CalcSum.h"


extern std::atomic<bool> runThreads;


CalcSum::CalcSum()
{
  //ctor
}


void CalcSum::run(Queue<float> & iMinNumbers, Queue<float> & iMaxNumbers, Queue<float> & oSums)
{
  std::string funcName = "CalcSum::run(...)";

  while(runThreads)
  {
    TimestampedData<float> minNumber, maxNumber, newEntry;

    // wait for new data
    minNumber = iMinNumbers.backButWaitNewData(_lastDataTimestamp);

    // get other input data
    maxNumber = iMaxNumbers.back();

    // process input data
    newEntry._timestamp = _lastDataTimestamp;
    newEntry._data = minNumber._data + maxNumber._data;

    // check frequency
//    this->threadFrequency(funcName,0,true);

    // output to console
//    newEntry.printDataAndLatency(funcName);

    oSums.push_back(newEntry,10);

    std::this_thread::yield();
  }
}


CalcSum::~CalcSum()
{
  //dtor
}
