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

    std::shared_ptr<TimestampedData<DataType> > back();

    std::shared_ptr<TimestampedData<DataType> > backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp);

    std::deque<std::shared_ptr<TimestampedData<DataType> > > copyQueue();
    std::deque<std::shared_ptr<TimestampedData<DataType> > > copyQueue(const std::chrono::duration<double> &duration);
    std::deque<std::shared_ptr<TimestampedData<DataType> > > copyQueue(const unsigned int &numElements);

    std::deque<std::shared_ptr<TimestampedData<DataType> > > copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> &lastDataTimestamp);
    std::deque<std::shared_ptr<TimestampedData<DataType> > > copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> &lastDataTimestamp, const std::chrono::duration<double> &duration);
    std::deque<std::shared_ptr<TimestampedData<DataType> > > copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> &lastDataTimestamp, const unsigned int &numElements);

    static unsigned int numElementsAfterTimestamp(const std::deque<std::shared_ptr<TimestampedData<DataType> > > & queueTemp, const std::chrono::time_point<std::chrono::high_resolution_clock> & timestamp);

    unsigned int push_back(const std::shared_ptr<TimestampedData<DataType> > & data, const std::chrono::duration<double> &maxDuration);
    unsigned int push_back(const std::shared_ptr<TimestampedData<DataType> > & data, const unsigned int & maxQueueSize);

    virtual ~Queue()
    {
      //
    };

  protected:

  private:

    std::condition_variable_any _cvAny;
    std::deque<std::shared_ptr<TimestampedData<DataType> > > _queue;
    std::shared_mutex _sharedMutex;

};


template <class DataType>
std::shared_ptr<TimestampedData<DataType> > Queue<DataType>::back()
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  if (_queue.empty())
  {
    return NULL;
  }
  return _queue.back();
}


template <class DataType>
std::shared_ptr<TimestampedData<DataType> > Queue<DataType>::backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp)
{
  std::shared_ptr<TimestampedData<DataType> > lastEntry;

  // wait for new data
  {
    std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
    _cvAny.wait(sharedLock,[&] ()->bool
    {
      // check vector size and check if same data timestamp at the end of the queue
      if (_queue.empty()
        || !_queue.back()
        || lastDataTimestamp == _queue.back()->_timestamp)
      {
        return false;
      }
      return true;
    });
    lastEntry = _queue.back();
  }
  lastDataTimestamp = lastEntry->_timestamp; // update last timestamp
  return lastEntry;
}


template <class DataType>
std::deque<std::shared_ptr<TimestampedData<DataType> > > Queue<DataType>::copyQueue()
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  return _queue;
}


template <class DataType>
std::deque<std::shared_ptr<TimestampedData<DataType> > > Queue<DataType>::copyQueue(const std::chrono::duration<double> &duration)
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  if (_queue.empty()
    || !_queue.back())
  {
    return std::deque<std::shared_ptr<TimestampedData<DataType> > > ();
  }
  auto afterTimestamp = _queue.back()->_timestamp - duration;
  auto itr=_queue.begin();
  for ( ; _queue.end()!=itr; ++itr)
  {
    if (*itr
      && (*itr)->_timestamp > afterTimestamp)
    {
      break;
    }
  }
  return std::deque(itr,_queue.end());
}


template <class DataType>
std::deque<std::shared_ptr<TimestampedData<DataType> > > Queue<DataType>::copyQueue(const unsigned int &numElements)
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
std::deque<std::shared_ptr<TimestampedData<DataType> > > Queue<DataType>::copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp)
{
  // wait for new data
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  _cvAny.wait(sharedLock,[&] ()->bool
  {
    // check vector size and check if same data timestamp at the end of the queue
    if (_queue.empty()
      || !_queue.back()
      || lastDataTimestamp == _queue.back()->_timestamp)
    {
      return false;
    }
    return true;
  });
  lastDataTimestamp = _queue.back()->_timestamp; // update last timestamp
  return _queue;
}


template <class DataType>
std::deque<std::shared_ptr<TimestampedData<DataType> > > Queue<DataType>::copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const std::chrono::duration<double> &duration)
{
  // wait for new data
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  _cvAny.wait(sharedLock,[&] ()->bool
  {
    // check vector size and check if same data timestamp at the end of the queue
    if (_queue.empty()
      || !_queue.back()
      || lastDataTimestamp == _queue.back()->_timestamp)
    {
      return false;
    }
    return true;
  });
  lastDataTimestamp = _queue.back()->_timestamp; // update last timestamp
  auto afterTimestamp = lastDataTimestamp - duration;
  auto itr=_queue.begin();
  for ( ; _queue.end()!=itr; ++itr)
  {
    if (*itr
      && (*itr)->_timestamp > afterTimestamp)
    {
      break;
    }
  }
  return std::deque(itr,_queue.end());
}


template <class DataType>
std::deque<std::shared_ptr<TimestampedData<DataType> > > Queue<DataType>::copyQueueButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &numElements)
{
  // wait for new data
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  _cvAny.wait(sharedLock,[&] ()->bool
  {
    // check vector size and check if same data timestamp at the end of the queue
    if (_queue.empty()
      || !_queue.back()
      || lastDataTimestamp == _queue.back()->_timestamp)
    {
      return false;
    }
    return true;
  });
  lastDataTimestamp = _queue.back()->_timestamp; // update last timestamp
  if (numElements >= _queue.size())
  {
    return _queue;
  }
  auto itr = std::prev(_queue.end(),numElements);
  return std::deque(itr,_queue.end());
}


template <class DataType>
unsigned int Queue<DataType>::numElementsAfterTimestamp(const std::deque<std::shared_ptr<TimestampedData<DataType> > > & queueTemp, const std::chrono::time_point<std::chrono::high_resolution_clock> & timestamp)
{
  unsigned int counter = 0;
  for (auto dataItr=queueTemp.rbegin(); queueTemp.rend() != dataItr; ++dataItr)
  {
    if ((*dataItr)->_timestamp < timestamp)
    {
      break;
    }
    ++counter;
  }
  return counter;
}


template <class DataType>
unsigned int Queue<DataType>::push_back(const std::shared_ptr<TimestampedData<DataType> > & data, const std::chrono::duration<double> &maxDuration)
{
  if (!data)
  {
    std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);
    return _queue.size();
  }

  // add data to queue and pop if max size reached
  auto timestamp = data->_timestamp - maxDuration;
  std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);
  while (!_queue.empty()
    && _queue.front()
    && _queue.front()->_timestamp < timestamp)
  {
    _queue.pop_front();
  }
  _queue.push_back(data);

  // notify waiting threads
  _cvAny.notify_all();
  return _queue.size();
};


template <class DataType>
unsigned int Queue<DataType>::push_back(const std::shared_ptr<TimestampedData<DataType> > & data, const unsigned int & maxQueueSize)
{
  if (!data)
  {
    std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);
    return _queue.size();
  }

  // add data to queue and pop if max size reached
  std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);
  _queue.push_back(data);
  if (_queue.size() > maxQueueSize)
  {
    _queue.pop_front();
  }

  // notify waiting threads
  _cvAny.notify_all();
  return _queue.size();
};


#endif // QUEUE_H
