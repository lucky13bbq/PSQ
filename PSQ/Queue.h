#ifndef QUEUE_H
#define QUEUE_H

#include <condition_variable>
#include <deque>
#include <shared_mutex>
#include "TimestampedData.h"


template <class DataType>
class Queue
{
  public:
    Queue()
    {
      //
    };

    TimestampedData<DataType> back();

    TimestampedData<DataType> backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp);

    void clear();

    std::deque<TimestampedData<DataType> > copyQueue();
    std::deque<TimestampedData<DataType> > copyQueue(const std::chrono::duration<double> &duration);
    std::deque<TimestampedData<DataType> > copyQueue(const unsigned int &numElements);

    std::deque<TimestampedData<DataType> > copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> &lastDataTimestamp);
    std::deque<TimestampedData<DataType> > copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> &lastDataTimestamp, const std::chrono::duration<double> &duration);
    std::deque<TimestampedData<DataType> > copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> &lastDataTimestamp, const unsigned int &numElements);

    unsigned int numElementsAfterTimestamp(const std::chrono::time_point<std::chrono::high_resolution_clock> & timestamp);

    unsigned int push_back(const TimestampedData<DataType> & data, const std::chrono::duration<double> &maxDuration);
    unsigned int push_back(const TimestampedData<DataType> & data, const unsigned int & maxQueueSize);

    void swap(std::deque<TimestampedData<DataType> > &other);

    virtual ~Queue()
    {
      //
    };

  protected:

  private:

    std::condition_variable_any _cvAny;
    std::deque<TimestampedData<DataType> > _queue;
    std::shared_mutex _sharedMutex;

};


template <class DataType>
TimestampedData<DataType> Queue<DataType>::back()
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  if (_queue.empty())
  {
    return TimestampedData<DataType> ();
  }
  return _queue.back();
}


template <class DataType>
TimestampedData<DataType> Queue<DataType>::backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp)
{
  TimestampedData<DataType> lastEntry;

  // wait for new data
  {
    std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
    _cvAny.wait(sharedLock,[&] ()->bool
    {
      // check vector size and check if same data timestamp at the end of the queue
      if (_queue.empty()
        || lastDataTimestamp >= _queue.back()._timestamp)
      {
        return false;
      }
      return true;
    });
    lastEntry = _queue.back();
  }
  lastDataTimestamp = lastEntry._timestamp; // update last timestamp
  return lastEntry;
}


template <class DataType>
void Queue<DataType>::clear()
{
  std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);
  _queue.clear();
}


template <class DataType>
std::deque<TimestampedData<DataType> > Queue<DataType>::copyQueue()
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  return _queue;
}


template <class DataType>
std::deque<TimestampedData<DataType> > Queue<DataType>::copyQueue(const std::chrono::duration<double> &duration)
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  if (_queue.empty())
  {
    return std::deque<TimestampedData<DataType> > ();
  }
  auto afterTimestamp = _queue.back()._timestamp - duration;
  auto itr=_queue.begin();
  for ( ; _queue.end()!=itr; ++itr)
  {
    if (itr->_timestamp > afterTimestamp)
    {
      break;
    }
  }
  return std::deque(itr,_queue.end());
}


template <class DataType>
std::deque<TimestampedData<DataType> > Queue<DataType>::copyQueue(const unsigned int &numElements)
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  if (numElements >= _queue.size())
  {
    return _queue;
  }
  auto itr = std::prev(_queue.end(),numElements);
  return std::deque(itr,_queue.end());
}


template <class DataType>
std::deque<TimestampedData<DataType> > Queue<DataType>::copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp)
{
  // wait for new data
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  _cvAny.wait(sharedLock,[&] ()->bool
  {
    // check vector size and check if same data timestamp at the end of the queue
    if (_queue.empty()
      || lastDataTimestamp >= _queue.back()._timestamp)
    {
      return false;
    }
    return true;
  });
  lastDataTimestamp = _queue.back()._timestamp; // update last timestamp
  return _queue;
}


template <class DataType>
std::deque<TimestampedData<DataType> > Queue<DataType>::copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const std::chrono::duration<double> &duration)
{
  // wait for new data
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  _cvAny.wait(sharedLock,[&] ()->bool
  {
    // check vector size and check if same data timestamp at the end of the queue
    if (_queue.empty()
      || lastDataTimestamp >= _queue.back()._timestamp)
    {
      return false;
    }
    return true;
  });
  lastDataTimestamp = _queue.back()._timestamp; // update last timestamp
  auto afterTimestamp = lastDataTimestamp - duration;
  auto itr=_queue.begin();
  for ( ; _queue.end()!=itr; ++itr)
  {
    if (itr->_timestamp > afterTimestamp)
    {
      break;
    }
  }
  return std::deque(itr,_queue.end());
}


template <class DataType>
std::deque<TimestampedData<DataType> > Queue<DataType>::copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &numElements)
{
  // wait for new data
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  _cvAny.wait(sharedLock,[&] ()->bool
  {
    // check vector size and check if same data timestamp at the end of the queue
    if (_queue.empty()
      || lastDataTimestamp >= _queue.back()._timestamp)
    {
      return false;
    }
    return true;
  });
  lastDataTimestamp = _queue.back()._timestamp; // update last timestamp
  if (numElements >= _queue.size())
  {
    return _queue;
  }
  auto itr = std::prev(_queue.end(),numElements);
  return std::deque(itr,_queue.end());
}


template <class DataType>
unsigned int Queue<DataType>::numElementsAfterTimestamp(const std::chrono::time_point<std::chrono::high_resolution_clock> & timestamp)
{
  unsigned int counter = 0;

  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);

  for (auto dataItr=_queue.rbegin(); _queue.rend()!=dataItr; ++dataItr)
  {
    if (dataItr->_timestamp < timestamp)
    {
      break;
    }
    ++counter;
  }
  return counter;
}


template <class DataType>
unsigned int Queue<DataType>::push_back(const TimestampedData<DataType> &newEntry, const std::chrono::duration<double> &maxDuration)
{
  std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);

  // add data to queue and pop if max size reached
  auto timestamp = newEntry._timestamp - maxDuration;
  while (!_queue.empty()
    && _queue.front()._timestamp < timestamp)
  {
    _queue.pop_front();
  }
  _queue.push_back(newEntry);

  // notify waiting threads
  _cvAny.notify_all();
  return _queue.size();
};


template <class DataType>
unsigned int Queue<DataType>::push_back(const TimestampedData<DataType> &newEntry, const unsigned int & maxQueueSize)
{
  std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);

  // add data to queue and pop if max size reached
  _queue.push_back(newEntry);
  if (_queue.size() > maxQueueSize)
  {
    _queue.pop_front();
  }

  // notify waiting threads
  _cvAny.notify_all();
  return _queue.size();
};


template <class DataType>
void Queue<DataType>::swap(std::deque<TimestampedData<DataType> > &other)
{
  std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);
  _queue.swap(other);
}


#endif // QUEUE_H
