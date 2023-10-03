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
    std::shared_ptr<TimestampedData<float> > randomNumber;
    std::vector<std::shared_ptr<TimestampedData<float> > > randomNumbers;
    std::shared_ptr<TimestampedData<float> > newData;

    // wait for new data
    randomNumbers = iRandomNumbers.backButWaitNewData(_lastDataTimestamp);

    // find min
    for (unsigned int i=0; i<randomNumbers.size(); ++i)
    {
      if (randomNumbers.at(i)
        && randomNumbers.at(i)->_data < minVal)
      {
        minVal = randomNumbers.at(i)->_data;
      }
    }

    // prepare data
    newData = std::make_shared<TimestampedData<float> > ();
    newData->_timestamp = _lastDataTimestamp;
    newData->_data = minVal;

    // output to console
//    newData->printDataAndLatency(funcName);

    // write data
    oMinNumbers.push_back(newData,10);

    std::this_thread::yield();
  }
}


FindMin::~FindMin()
{
  //dtor
}
