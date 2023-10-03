#include <thread>
#include "MultiplyBy10.h"

extern std::atomic<bool> runThreads;


MultiplyBy10::MultiplyBy10()
{
  // ctor
}


void MultiplyBy10::run(Queue<float> & iMaxNumbers, Queue<float> & oMultipliedBy10)
{
  std::string funcName = "MultiplyBy10::run(...)";

  while(runThreads)
  {
    std::shared_ptr<Data<float> > maxNumber, newData;

    // wait for new data
    maxNumber = iMaxNumbers.backButWaitNewData(_lastDataTimestamp);

    // process input data
    newData = std::make_shared<Data<float> > ();
    newData->_timestamp = _lastDataTimestamp;
    newData->_data = maxNumber->_data * 10.0;

    // output to console
//    newData->printDataAndLatency(funcName);
//    this->threadFrequency(funcName,0,true);

    oMultipliedBy10.push_back(newData,10);

    std::this_thread::yield();
  }
}


MultiplyBy10::~MultiplyBy10()
{
  // dtor
}
