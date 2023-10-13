#include <thread>
#include "Viewer.h"

extern std::atomic<bool> runThreads;


Viewer::Viewer()
{
  //ctor
}


void Viewer::run(Queue<float> &iMinNumbers, Queue<float> &iMaxNumbers, Queue<float> &iMultipliedBy10, Queue<float> &iSums, VectorOfQueues<float> & iRandomNumbers)
{
  using namespace std::chrono_literals;

  std::string funcName = "Viewer::run(...)";

  // test VectorOfQueues::back
  TimestampedData<float> randomNumber;
  randomNumber = iRandomNumbers.back(0);
  randomNumber = iRandomNumbers.back(1);
  randomNumber = iRandomNumbers.back(10);

  // test VectorOfQueues::backBuWaitNewData
  randomNumber = iRandomNumbers.backButWaitNewData(_lastDataTimestamp,0);
  randomNumber = iRandomNumbers.backButWaitNewData(_lastDataTimestamp,1);
  randomNumber = iRandomNumbers.backButWaitNewData(_lastDataTimestamp,2);

  // test VectorOfQueues::copyVector
  std::vector<std::deque<TimestampedData<float> > > randomNumbersTest = iRandomNumbers.copyVector();
  randomNumbersTest = iRandomNumbers.copyVector();
  randomNumbersTest = iRandomNumbers.copyVector(5s);
  randomNumbersTest = iRandomNumbers.copyVector(5); // TODO

  // test VectorOfQueues::copyVectorButWaitNewData
  randomNumbersTest = iRandomNumbers.copyVectorButWaitNewData(_lastDataTimestamp);
  randomNumbersTest = iRandomNumbers.copyVectorButWaitNewData(_lastDataTimestamp,5s); // TODO
  randomNumbersTest = iRandomNumbers.copyVectorButWaitNewData(_lastDataTimestamp,5); // TODO

  // test Queue::copyQueue
  std::deque<TimestampedData<float> > multipliedBy10sTest = iMultipliedBy10.copyQueue();
  multipliedBy10sTest = iMultipliedBy10.copyQueue(5);
  multipliedBy10sTest = iMultipliedBy10.copyQueue(5s);

  // test Queue::copyQueueButWaitNewData
  std::deque<TimestampedData<float> > sums = iSums.copyQueueButWaitNewData(_lastDataTimestamp);
  sums = iSums.copyQueueButWaitNewData(_lastDataTimestamp,5);
  sums = iSums.copyQueueButWaitNewData(_lastDataTimestamp,5s);

  while(runThreads)
  {
    // wait for new data
    TimestampedData<float> sum = iSums.backButWaitNewData(_lastDataTimestamp);

    // get other input data
    std::deque<TimestampedData<float> > minNumbers = iMinNumbers.copyQueue();
    std::deque<TimestampedData<float> > maxNumbers = iMaxNumbers.copyQueue();
    std::deque<TimestampedData<float> > multipliedBy10s = iMultipliedBy10.copyQueue();
    std::vector<std::deque<TimestampedData<float> > > randomNumbers = iRandomNumbers.copyVector();

    // count number of elements in other queues after current data timestamp
    unsigned int minNumberCount = iMinNumbers.numElementsAfterTimestamp(_lastDataTimestamp);
    unsigned int maxNumberCount = iMaxNumbers.numElementsAfterTimestamp(_lastDataTimestamp);
    unsigned int multipliedBy10Count = iMultipliedBy10.numElementsAfterTimestamp(_lastDataTimestamp);
    std::vector<unsigned int> randomNumberCounts = iRandomNumbers.numElementsAfterTimestamp(_lastDataTimestamp);

//    // output data to console
    sum.printDataAndLatency(funcName);

    // output timestamp and counts (number of elements in other queues after current data timestamp)
    std::string str = funcName + " sum timestamp " + std::to_string(sum._timestamp.time_since_epoch().count())
      + ": minNumber timestamp " + std::to_string(minNumbers.back()._timestamp.time_since_epoch().count()) + " count " + std::to_string(minNumberCount)
      + ", maxNumber timestamp " + std::to_string(maxNumbers.back()._timestamp.time_since_epoch().count()) + " count " + std::to_string(maxNumberCount)
      + ", multipliedBy10 timestamp " + std::to_string(multipliedBy10s.back()._timestamp.time_since_epoch().count()) + " count " + std::to_string(multipliedBy10Count);
    for (unsigned int i=0; i<randomNumberCounts.size(); ++i)
    {
      str += ", randomNumber " + std::to_string(i) + " timestamp " + std::to_string(randomNumbers[i].back()._timestamp.time_since_epoch().count()) + " count " + std::to_string(randomNumberCounts[i]);
    }
    std::cout << str << std::endl;

    std::this_thread::yield();
  }
}

Viewer::~Viewer()
{
  //dtor
}
