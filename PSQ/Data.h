#ifndef DATA_H
#define DATA_H

#include <chrono>
#include <iostream>


template<class T>
class Data
{
  public:
    Data() {}
    void printDataAndLatency(const std::string &funcName2);
    virtual ~Data() {}


    std::chrono::time_point<std::chrono::high_resolution_clock> _timestamp;
    T _data;

  protected:

  private:
};


template<class T>
void Data<T>::printDataAndLatency(const std::string &funcName1)
{
  std::string funcName = "Data<T>::printDataAndLatency(" +funcName1 + ")";

  std::string str = funcName + " timestamp " + std::to_string(_timestamp.time_since_epoch().count()) + ", latency " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _timestamp).count()) + "us, data " + std::to_string(_data);

  std::cout << str << std::endl;
};


#endif // DATA_H
