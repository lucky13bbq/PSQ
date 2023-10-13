#include <thread>
#include <vector>
#include "CalcSum.h"
#include "FindMax.h"
#include "FindMin.h"
#include "MultiplyBy10.h"
#include "RandomNumberGenerator.h"
#include "PSQ/Queue.h"
#include "Viewer.h"

std::atomic<bool> runThreads = true;


int main()
{
  std::vector<std::thread> threads;

  // threads
  FindMax findMax;
  FindMin findMin;
  MultiplyBy10 multiplyBy10;
  RandomNumberGenerator rand0;
  RandomNumberGenerator rand1;
  RandomNumberGenerator rand2;
  RandomNumberGenerator rand3;
  CalcSum calcSum;
  Viewer viewer;

  // data queues
  Queue<float> minNumbers;
  Queue<float> maxNumbers;
  Queue<float> multipliedBy10;
  Queue<float> sums;
  VectorOfQueues<float> randomNumbers;

  // launch threads
  threads.emplace_back(std::thread(&RandomNumberGenerator::run,&rand0,0,std::ref(randomNumbers)));
  threads.emplace_back(std::thread(&RandomNumberGenerator::run,&rand1,1,std::ref(randomNumbers)));
  threads.emplace_back(std::thread(&RandomNumberGenerator::run,&rand2,2,std::ref(randomNumbers)));
  threads.emplace_back(std::thread(&RandomNumberGenerator::run,&rand3,3,std::ref(randomNumbers)));
  threads.emplace_back(std::thread(&FindMax::run,&findMax,std::ref(randomNumbers),std::ref(maxNumbers)));
  threads.emplace_back(std::thread(&FindMin::run,&findMin,std::ref(randomNumbers),std::ref(minNumbers)));
  threads.emplace_back(std::thread(&CalcSum::run,&calcSum,std::ref(minNumbers),std::ref(maxNumbers),std::ref(sums)));
  threads.emplace_back(std::thread(&MultiplyBy10::run,&multiplyBy10,std::ref(maxNumbers),std::ref(multipliedBy10)));
  threads.emplace_back(std::thread(&Viewer::run,&viewer,std::ref(minNumbers),std::ref(maxNumbers),std::ref(multipliedBy10),std::ref(sums),std::ref(randomNumbers)));

  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::cout << "Stopping threads and exiting..." << std::endl;
  runThreads = false;

  for (unsigned int i=0; i<threads.size(); ++i)
  {
    if (threads[i].joinable())
    {
      threads[i].join();
    }
  }

  std::cout << "Stopping threads and exiting... done!" << std::endl;
}
