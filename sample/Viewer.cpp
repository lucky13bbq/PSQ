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

    // test VectorOfQueues::copyVector
    std::vector<std::deque<std::shared_ptr<Data<float> > > > randomNumbersTest = iRandomNumbers.copyVector();
    randomNumbersTest = iRandomNumbers.copyVector();
//    randomNumbersTest = iRandomNumbers.copyVector(5s); // TODO
//    randomNumbersTest = iRandomNumbers.copyVector(_lastDataTimestamp); // TODO
//    randomNumbersTest = iRandomNumbers.copyVector(5); // TODO

    // test VectorOfQueues::copyVectorButWaitNewData
    randomNumbersTest = iRandomNumbers.copyVectorButWaitNewData(_lastDataTimestamp);
//    randomNumbersTest = iRandomNumbers.copyVectorButWaitNewData(_lastDataTimestamp,5s); // TODO
//    randomNumbersTest = iRandomNumbers.copyVectorButWaitNewData(_lastDataTimestamp,_lastDataTimestamp); // TODO
//    randomNumbersTest = iRandomNumbers.copyVectorButWaitNewData(_lastDataTimestamp,5); // TODO

    // test Queue::copyQueue
    std::deque<std::shared_ptr<Data<float> > > multipliedBy10sTest = iMultipliedBy10.copyQueue();
    multipliedBy10sTest = iMultipliedBy10.copyQueue(5);
    multipliedBy10sTest = iMultipliedBy10.copyQueue(std::chrono::high_resolution_clock::now());
    multipliedBy10sTest = iMultipliedBy10.copyQueue(5s);

    // test Queue::copyQueueButWaitNewData
    std::deque<std::shared_ptr<Data<float> > > sums = iSums.copyQueueButWaitNewData(_lastDataTimestamp);
    sums = iSums.copyQueueButWaitNewData(_lastDataTimestamp,5);
    sums = iSums.copyQueueButWaitNewData(_lastDataTimestamp,std::chrono::high_resolution_clock::now());
    sums = iSums.copyQueueButWaitNewData(_lastDataTimestamp,5s);

  while(runThreads)
  {
    // wait for new data
    std::shared_ptr<Data<float> > sum = iSums.backButWaitNewData(_lastDataTimestamp);

    // get other input data
    std::deque<std::shared_ptr<Data<float> > > minNumbers = iMinNumbers.copyQueue();
    std::deque<std::shared_ptr<Data<float> > > maxNumbers = iMaxNumbers.copyQueue();
    std::deque<std::shared_ptr<Data<float> > > multipliedBy10s = iMultipliedBy10.copyQueue();
    std::vector<std::deque<std::shared_ptr<Data<float> > > > randomNumbers = iRandomNumbers.copyVector();

    // count number of elements in other queues after current data timestamp
    unsigned int minNumberCount = Queue<float>::numElementsAfterTimestamp(minNumbers,_lastDataTimestamp);
    unsigned int maxNumberCount = Queue<float>::numElementsAfterTimestamp(maxNumbers,_lastDataTimestamp);
    unsigned int multipliedBy10Count = Queue<float>::numElementsAfterTimestamp(multipliedBy10s,_lastDataTimestamp);
    std::vector<unsigned int> randomNumberCounts = VectorOfQueues<float>::numElementsAfterTimestamp(randomNumbers,_lastDataTimestamp);

//    // output data to console
    sum->printDataAndLatency(funcName);

    // output timestamp and counts (number of elements in other queues after current data timestamp)
    std::string str = funcName + " sum timestamp " + std::to_string(sum->_timestamp.time_since_epoch().count())
      + ": minNumber timestamp " + std::to_string(minNumbers.back()->_timestamp.time_since_epoch().count()) + " count " + std::to_string(minNumberCount)
      + ", maxNumber timestamp " + std::to_string(maxNumbers.back()->_timestamp.time_since_epoch().count()) + " count " + std::to_string(maxNumberCount)
      + ", multipliedBy10 timestamp " + std::to_string(multipliedBy10s.back()->_timestamp.time_since_epoch().count()) + " count " + std::to_string(multipliedBy10Count);
    for (unsigned int i=0; i<randomNumberCounts.size(); ++i)
    {
      str += ", randomNumber " + std::to_string(i) + " timestamp " + std::to_string(randomNumbers.at(i).back()->_timestamp.time_since_epoch().count()) + " count " + std::to_string(randomNumberCounts.at(i));
    }
    std::cout << str << std::endl;

    std::this_thread::yield();
  }
}

Viewer::~Viewer()
{
  //dtor
}
