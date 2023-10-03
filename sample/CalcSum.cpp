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
    std::shared_ptr<TimestampedData<float> > minNumber, maxNumber, newData;

    // wait for new data
    minNumber = iMinNumbers.backButWaitNewData(_lastDataTimestamp);

    // get other input data
    maxNumber = iMaxNumbers.back();

    // process input data
    newData = std::make_shared<TimestampedData<float> > ();
    newData->_timestamp = _lastDataTimestamp;
    newData->_data = minNumber->_data;
    if (maxNumber)
    {
      newData->_data += maxNumber->_data;
    }

    // check frequency
//    this->threadFrequency(funcName,0,true);

    // output to console
//    newData->printDataAndLatency(funcName);

    oSums.push_back(newData,10);

    std::this_thread::yield();
  }
}


CalcSum::~CalcSum()
{
  //dtor
}
