#ifndef VECTOROFQUEUES_H
#define VECTOROFQUEUES_H

#include <condition_variable>
#include <deque>
#include <shared_mutex>
#include <vector>
#include "TimestampedData.h"


template <class DataType>
class VectorOfQueues
{
  public:
    VectorOfQueues()
    {
      //
    }
    std::vector<TimestampedData<DataType> > back();
    TimestampedData<DataType> back(const unsigned int &index);
    std::vector<TimestampedData<DataType> > backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp);
    TimestampedData<DataType> backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &index);

    std::vector<std::deque<TimestampedData<DataType> > > copyVector();
    std::vector<std::deque<TimestampedData<DataType> > > copyVector(const std::chrono::duration<double> &duration);
    std::vector<std::deque<TimestampedData<DataType> > > copyVector(const unsigned int &numElements);

    std::vector<std::deque<TimestampedData<DataType> > > copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp);
    std::vector<std::deque<TimestampedData<DataType> > > copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const std::chrono::duration<double> &duration);
    std::vector<std::deque<TimestampedData<DataType> > > copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &numElements);

    std::vector<unsigned int> numElementsAfterTimestamp(const std::chrono::time_point<std::chrono::high_resolution_clock> & timestamp);

    unsigned int push_back(const unsigned int & index, const TimestampedData<DataType> & data, const unsigned int & maxQueueSize);
    unsigned int push_back(const unsigned int & index, const TimestampedData<DataType> & data, const std::chrono::duration<double> &duration);

    virtual ~VectorOfQueues()
    {
      //
    };

  protected:

  private:

    std::condition_variable_any _cvAny;
    std::vector<std::deque<TimestampedData<DataType> > > _vectorOfQueues;
    std::shared_mutex _sharedMutex;

};


template <class DataType>
std::vector<TimestampedData<DataType> > VectorOfQueues<DataType>::back()
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);

  std::vector<TimestampedData<DataType> > vec(_vectorOfQueues.size());
  for (unsigned int i=0; i<vec.size(); ++i)
  {
    if (!_vectorOfQueues[i].empty())
    {
      vec[i] = _vectorOfQueues[i].back();
    }
  }
  return vec;
}


template <class DataType>
TimestampedData<DataType> VectorOfQueues<DataType>::back(const unsigned int &index)
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);

  if (_vectorOfQueues.size() <= index
    || _vectorOfQueues.at(index).empty())
  {
    return TimestampedData<DataType> ();
  }
  return _vectorOfQueues.at(index).back();
}


template <class DataType>
std::vector<TimestampedData<DataType> > VectorOfQueues<DataType>::backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp)
{
  bool stopWaiting;
  std::vector<TimestampedData<DataType> > vec;

  // wait for new data
  {
    std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
    _cvAny.wait(sharedLock,[&] ()->bool
    {
      // check vector size
      if (_vectorOfQueues.empty())
      {
        return false; // keep waiting
      }

      // check timestamps if new data
      vec.clear();
      vec.resize(_vectorOfQueues.size());
      stopWaiting = false;
      for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
      {
        if (_vectorOfQueues[i].empty())
        {
          continue;
        }
        vec[i] = _vectorOfQueues[i].back();
        if (vec[i]._timestamp > lastDataTimestamp)
        {
          lastDataTimestamp = vec[i]._timestamp;
          stopWaiting = true;
        }
      }
      return stopWaiting;
    });
  }
  return vec;
}


template <class DataType>
TimestampedData<DataType> VectorOfQueues<DataType>::backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &index)
{
  // wait for new data
  TimestampedData<DataType> *entry;
  {
    std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
    _cvAny.wait(sharedLock,[&] ()->bool
    {
      // check vector size
      if (_vectorOfQueues.empty()
        || _vectorOfQueues.size() <= index
        || _vectorOfQueues.at(index).empty())
      {
        return false; // keep waiting
      }
      entry = &(_vectorOfQueues.at(index).back());

      // check timestamps if new data
      if (entry->_timestamp > lastDataTimestamp)
      {
        lastDataTimestamp = entry->_timestamp;
        return true; // stop waiting
      }
      return false;
    });
  }
  return *entry;
}


template <class DataType>
std::vector<std::deque<TimestampedData<DataType> > > VectorOfQueues<DataType>::copyVector()
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  return _vectorOfQueues;
}


// get the most recent elements from each queue within a desired duration
template <class DataType>
std::vector<std::deque<TimestampedData<DataType> > > VectorOfQueues<DataType>::copyVector(const std::chrono::duration<double> &duration)
{
  TimestampedData<DataType> *lastEntry;

  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);

  std::vector<std::deque<TimestampedData<DataType> > > vecTemp(_vectorOfQueues.size());
  for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
  {
    if (_vectorOfQueues[i].empty())
    {
      continue;
    }
    lastEntry = &(_vectorOfQueues[i].back());
    auto timestampCutoff = lastEntry->_timestamp - duration;
    auto queueItr = _vectorOfQueues[i].begin();
    for ( ; _vectorOfQueues[i].end()!=queueItr; ++queueItr)
    {
      if (queueItr->_timestamp > timestampCutoff)
      {
        break;
      }
    }
    vecTemp[i] = std::deque(queueItr,_vectorOfQueues[i].end());
  }
  return vecTemp;
}


// get the N most recent elements from each queue
template <class DataType>
std::vector<std::deque<TimestampedData<DataType> > > VectorOfQueues<DataType>::copyVector(const unsigned int &numElements)
{
  std::deque<TimestampedData<DataType> > *queuePtr;

  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);

  std::vector<std::deque<TimestampedData<DataType> > > vec(_vectorOfQueues.size());
  for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
  {
    if (_vectorOfQueues[i].empty())
    {
      continue;
    }
    queuePtr = &(_vectorOfQueues[i]);
    if (numElements >= queuePtr->size())
    {
      vec[i] = *queuePtr;
    }
    else
    {
      auto queueItr = std::prev(queuePtr->end(),numElements);
      vec[i] = std::deque(queueItr,queuePtr->end());
    }
  }
  return vec;
}


template <class DataType>
std::vector<std::deque<TimestampedData<DataType> > > VectorOfQueues<DataType>::copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp)
{
  bool stopWaiting;
  TimestampedData<DataType> *entry;

  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);

  _cvAny.wait(sharedLock,[&] ()->bool
  {
    // check vector size
    if (_vectorOfQueues.empty())
    {
      return false; // keep waiting
    }

    // check timestamps if new data
    stopWaiting = false;
    for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
    {
      if (_vectorOfQueues[i].empty())
      {
        continue;
      }
      entry = &(_vectorOfQueues[i].back());
      if (entry->_timestamp > lastDataTimestamp)
      {
        lastDataTimestamp = entry->_timestamp;
        stopWaiting = true;
      }
    }
    return stopWaiting;
  });
  return _vectorOfQueues;
}


template <class DataType>
std::vector<std::deque<TimestampedData<DataType> > > VectorOfQueues<DataType>::copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const std::chrono::duration<double> &duration)
{
  bool stopWaiting;
  TimestampedData<DataType> *entry;
  std::vector<std::deque<TimestampedData<DataType> > > vec;

  // wait for new data
  {
    std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
    _cvAny.wait(sharedLock,[&] ()->bool
    {
      // check vector size
      if (_vectorOfQueues.empty())
      {
        return false; // keep waiting
      }

      // check timestamps if new data
      stopWaiting = false;
      for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
      {
        if (_vectorOfQueues[i].empty())
        {
          continue;
        }
        entry = &(_vectorOfQueues[i].back());
        if (entry->_timestamp > lastDataTimestamp)
        {
          lastDataTimestamp = entry->_timestamp;
          stopWaiting = true;
        }
      }
      return stopWaiting;
    });

    vec.resize(_vectorOfQueues.size());
    for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
    {
      if (_vectorOfQueues[i].empty())
      {
        continue;
      }
      entry = &(_vectorOfQueues[i].back());
      auto timestampCutoff = entry->_timestamp - duration;
      auto queueItr = _vectorOfQueues[i].begin();
      for ( ; _vectorOfQueues[i].end()!=queueItr; ++queueItr)
      {
        if (queueItr->_timestamp > timestampCutoff)
        {
          break;
        }
      }
      vec[i] = std::deque(queueItr,_vectorOfQueues[i].end());
    }
  }
  return vec;
}


template <class DataType>
std::vector<std::deque<TimestampedData<DataType> > > VectorOfQueues<DataType>::copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &numElements)
{
  bool stopWaiting;
  std::deque<TimestampedData<DataType> > *queuePtr;
  std::vector<std::deque<TimestampedData<DataType> > > vec;
  TimestampedData<DataType> *entry;

  // wait for new data
  {
    std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
    _cvAny.wait(sharedLock,[&] ()->bool
    {
      // check vector size
      if (_vectorOfQueues.empty())
      {
        return false; // keep waiting
      }

      // check timestamps if new data
      stopWaiting = false;
      for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
      {
        if (_vectorOfQueues[i].empty())
        {
          continue;
        }
        entry = &(_vectorOfQueues[i].back());
        if (entry->_timestamp > lastDataTimestamp)
        {
          lastDataTimestamp = entry->_timestamp;
          stopWaiting = true;
        }
      }
      return stopWaiting;
    });

    vec.resize(_vectorOfQueues.size());
    for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
    {
      if (_vectorOfQueues[i].empty())
      {
        continue;
      }
      queuePtr = &(_vectorOfQueues[i]);
      if (numElements >= queuePtr->size())
      {
        // copy full queue
        vec[i] = *queuePtr;
      }
      else
      {
        // copy last N elements of queue
        auto queueItr = std::prev(queuePtr->end(),numElements);
        vec[i] = std::deque(queueItr,queuePtr->end());
      }
    }
  }
  return vec;
}


template <class DataType>
std::vector<unsigned int> VectorOfQueues<DataType>::numElementsAfterTimestamp(const std::chrono::time_point<std::chrono::high_resolution_clock> & timestamp)
{
  unsigned int counter;

  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);

  std::vector<unsigned int> counts(_vectorOfQueues.size(),0);
  for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
  {
    counter = 0;
    for (auto dataItr=_vectorOfQueues[i].rbegin(); _vectorOfQueues[i].rend()!=dataItr; ++dataItr)
    {
      if (dataItr->_timestamp < timestamp)
      {
        break;
      }
      ++counter;
    }
    counts[i] = counter;
  }
  return counts;
}


template <class DataType>
unsigned int VectorOfQueues<DataType>::push_back(const unsigned int &index, const TimestampedData<DataType> &newEntry, const std::chrono::duration<double> &duration)
{
  std::deque<TimestampedData<DataType> > *queuePtr;

  std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);

  // resize vector if needed
  if (_vectorOfQueues.size() <= index)
  {
    _vectorOfQueues.resize(index+1);
  }
  queuePtr = &(_vectorOfQueues.at(index));

  // pop old data and add data to queue
  auto timestampCutOff = newEntry._timestamp - duration;
  while (!queuePtr->empty()
    && queuePtr->front()._timestamp < timestampCutOff)
  {
    queuePtr->pop_front();
  }
  queuePtr->push_back(newEntry);

  // notify waiting threads
  _cvAny.notify_all();

  return queuePtr->size();
};


template <class DataType>
unsigned int VectorOfQueues<DataType>::push_back(const unsigned int & index, const TimestampedData<DataType> & data, const unsigned int & maxQueueSize)
{
  std::deque<TimestampedData<DataType> > *queuePtr;

  std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);

  // resize vector if needed
  if (_vectorOfQueues.size() <= index)
  {
    _vectorOfQueues.resize(index+1);
  }
  queuePtr = &(_vectorOfQueues.at(index));

  // add data to queue and pop if max size reached
  queuePtr->push_back(data);
  if (queuePtr->size() > maxQueueSize)
  {
    queuePtr->pop_front();
  }

  // notify waiting threads
  _cvAny.notify_all();
  return queuePtr->size();
};


#endif // VECTOROFQUEUES_H
