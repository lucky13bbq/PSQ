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
    std::shared_ptr<Data<float> > randomNumber;
    std::vector<std::shared_ptr<Data<float> > > randomNumbers;
    std::shared_ptr<Data<float> > newData;

    // wait for new data
    randomNumbers = iRandomNumbers.backButWaitNewData(_lastDataTimestamp);

    // find min
    for (unsigned int i=0; i<randomNumbers.size(); ++i)
    {
      if (randomNumbers.at(i)
        && randomNumbers.at(i)->_data > maxVal)
      {
        maxVal = randomNumbers.at(i)->_data;
      }
    }

    // write data
    newData = std::make_shared<Data<float> > ();
    newData->_timestamp = _lastDataTimestamp;
    newData->_data = maxVal;

    // output to console
//    newData->printDataAndLatency(funcName);

    // write data
    oMaxNumbers.push_back(newData,10);

    std::this_thread::yield();
  }
}


FindMax::~FindMax()
{
  //dtor
}
