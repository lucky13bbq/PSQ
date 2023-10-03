#include <thread>
#include "RandomNumberGenerator.h"

extern std::atomic<bool> runThreads;


RandomNumberGenerator::RandomNumberGenerator()
{
  //ctor
}

void RandomNumberGenerator::run(const unsigned int & index, VectorOfQueues<float> & oRandomNumbers)
{
  std::string funcName = "RandomNumberGenerator::run(" + std::to_string(index) +")";

  bool outputFrequencyToConsole;
  std::shared_ptr<TimestampedData<float> > data;
  unsigned int desiredFrequencyInHz;

  while (runThreads)
  {
    // cap thread frequency to simulate data coming for a 30Hz camera
//    desiredFrequencyInHz = 0; // unlimited, run as fast as possible
//    desiredFrequencyInHz = 1; // 1Hz is slowest
    desiredFrequencyInHz = 31; // simulate a 30Hz input e.g. camera
    outputFrequencyToConsole = false;
    this->threadFrequency(funcName,desiredFrequencyInHz,outputFrequencyToConsole);

    // prepare data
    data = std::make_shared<TimestampedData<float> > ();
    data->_timestamp = std::chrono::high_resolution_clock::now();
    data->_data = float(rand())/RAND_MAX;

    // write data
    oRandomNumbers.push_back(index,data,10);

    std::this_thread::yield();
  }
}

RandomNumberGenerator::~RandomNumberGenerator()
{
  //dtor
}
