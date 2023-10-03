#ifndef BASETHREAD_H
#define BASETHREAD_H

#include <atomic>
#include <chrono>
#include <iostream>
#include <string>


class BaseThread
{
  public:
    BaseThread()
    {
      //
    }
    virtual ~BaseThread() {};

  protected:

    void threadFrequency(const std::string & funcName, const unsigned int & desiredFrequency, const bool & print);


    std::chrono::time_point<std::chrono::high_resolution_clock> _lastDataTimestamp;
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastExecutionTimestamp;

  private:
};

// set or get thread's update frequency
inline void BaseThread::threadFrequency(const std::string & funcName2, const unsigned int & desiredFrequency, const bool & print)
{
  std::string funcName = "BaseThread::threadFrequency(" + funcName2 + ")";

  using namespace std::chrono_literals;

  auto period = std::chrono::high_resolution_clock::now() - _lastExecutionTimestamp;
  unsigned int desiredLatency = int(1000000.0/desiredFrequency);
  unsigned int microseconds = std::chrono::duration_cast<std::chrono::microseconds>(period).count();

  // first run
  if (microseconds > 10000000)
  {
    _lastExecutionTimestamp = std::chrono::high_resolution_clock::now();
    return;
  }

  // sleep thread if max frequency set
  if (0 != desiredFrequency
    && desiredLatency > microseconds)
  {
    std::this_thread::sleep_for(std::chrono::microseconds(desiredLatency-microseconds));
    period = std::chrono::high_resolution_clock::now() - _lastExecutionTimestamp;
    microseconds = std::chrono::duration_cast<std::chrono::microseconds>(period).count();
  }
  _lastExecutionTimestamp = std::chrono::high_resolution_clock::now();

  // print thread frequency
  if (print)
  {
    std::string str = funcName + " period " + std::to_string(microseconds) + "us, frequency " + std::to_string(1000000.0/microseconds) + "Hz";
    std::cout << str << std::endl;
  }
}

#endif // BASETHREAD_H
