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
    std::vector<std::shared_ptr<TimestampedData<DataType> > > back();
    std::shared_ptr<TimestampedData<DataType> > back(const unsigned int &index);
    std::vector<std::shared_ptr<TimestampedData<DataType> > > backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp);
    std::shared_ptr<TimestampedData<DataType> > backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &index);

    std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > copyVector();
    std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > copyVector(const std::chrono::duration<double> &duration);
    std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > copyVector(const unsigned int &numElements);

    std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp);
    std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const std::chrono::duration<double> &duration);
    std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &numElements);

    static std::vector<unsigned int> numElementsAfterTimestamp(const std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > & vectorTemp, const std::chrono::time_point<std::chrono::high_resolution_clock> & timestamp);

    unsigned int push_back(const unsigned int & index, const std::shared_ptr<TimestampedData<DataType> > & data, const unsigned int & maxQueueSize);
    unsigned int push_back(const unsigned int & index, const std::shared_ptr<TimestampedData<DataType> > & data, const std::chrono::duration<double> &duration);

    virtual ~VectorOfQueues()
    {
      //
    };

  protected:

  private:

    std::condition_variable_any _cvAny;
    std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > _vectorOfQueues;
    std::shared_mutex _sharedMutex;

};


template <class DataType>
std::vector<std::shared_ptr<TimestampedData<DataType> > > VectorOfQueues<DataType>::back()
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);

  std::vector<std::shared_ptr<TimestampedData<DataType> > > data(_vectorOfQueues.size(),NULL);
  for (unsigned int i=0; i<data.size(); ++i)
  {
    if (!_vectorOfQueues.at(i).empty())
    {
      data.at(i) = _vectorOfQueues.at(i).back();
    }
  }
  return data;
}


template <class DataType>
std::shared_ptr<TimestampedData<DataType> > VectorOfQueues<DataType>::back(const unsigned int &index)
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);

  if (_vectorOfQueues.size() <= index
    || _vectorOfQueues.at(index).empty())
  {
    return NULL;
  }
  return _vectorOfQueues.at(index).back();
}


template <class DataType>
std::vector<std::shared_ptr<TimestampedData<DataType> > > VectorOfQueues<DataType>::backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp)
{
  bool stopWaiting;
  std::vector<std::shared_ptr<TimestampedData<DataType> > >  data;

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
      data.clear();
      data.resize(_vectorOfQueues.size(),NULL);
      stopWaiting = false;
      for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
      {
        if (_vectorOfQueues.at(i).empty())
        {
          continue;
        }
        data.at(i) = _vectorOfQueues.at(i).back();
        if (data.at(i)->_timestamp > lastDataTimestamp)
        {
          lastDataTimestamp = data.at(i)->_timestamp;
          stopWaiting = true;
        }
      }
      return stopWaiting;
    });
  }
  return data;
}


template <class DataType>
std::shared_ptr<TimestampedData<DataType> > VectorOfQueues<DataType>::backButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &index)
{
  std::shared_ptr<TimestampedData<DataType> >  data = NULL;

  // wait for new data
  {
    std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
    _cvAny.wait(sharedLock,[&] ()->bool
    {
      // check vector size
      if (_vectorOfQueues.empty()
        || _vectorOfQueues.size() <= index
        || _vectorOfQueues.at(index).empty()
        || !_vectorOfQueues.at(index).back())
      {
        data = NULL;
        return false; // keep waiting
      }

      // check timestamps if new data
      data = _vectorOfQueues.at(index).back();
      if (data->_timestamp > lastDataTimestamp)
      {
        lastDataTimestamp = data->_timestamp;
        return true; // stop waiting
      }
      data = NULL;
      return false;
    });
  }
  return data;
}


template <class DataType>
std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > VectorOfQueues<DataType>::copyVector()
{
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  return _vectorOfQueues;
}


// get the most recent elements from each queue within a desired duration
template <class DataType>
std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > VectorOfQueues<DataType>::copyVector(const std::chrono::duration<double> &duration)
{
  std::shared_ptr<TimestampedData<DataType> > dataPtr;
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > vecTemp(_vectorOfQueues.size());
  for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
  {
    if (_vectorOfQueues.at(i).empty()
      || !_vectorOfQueues.at(i).back())
    {
      continue;
    }
    dataPtr = _vectorOfQueues.at(i).back();
    auto timestampCutoff = dataPtr->_timestamp - duration;
    auto queueItr = _vectorOfQueues.at(i).begin();
    for ( ; _vectorOfQueues.at(i).end()!=queueItr; ++queueItr)
    {
      if (*queueItr
        && (*queueItr)->_timestamp > timestampCutoff)
      {
        break;
      }
    }
    vecTemp.at(i) = std::deque(queueItr,_vectorOfQueues.at(i).end());
  }
  return vecTemp;
}


// get the N most recent elements from each queue
template <class DataType>
std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > VectorOfQueues<DataType>::copyVector(const unsigned int &numElements)
{
  std::shared_ptr<TimestampedData<DataType> > dataPtr;
  std::deque<std::shared_ptr<TimestampedData<DataType> > > * queuePtr;
  std::shared_lock<std::shared_mutex> sharedLock(_sharedMutex);
  std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > vecTemp(_vectorOfQueues.size());
  for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
  {
    if (_vectorOfQueues.at(i).empty()
      || !_vectorOfQueues.at(i).back())
    {
      continue;
    }
    queuePtr = &(_vectorOfQueues.at(i));
    if (numElements >= queuePtr->size())
    {
      vecTemp.at(i) = *queuePtr;
    }
    else
    {
      auto queueItr = std::prev(queuePtr->end(),numElements);
      vecTemp.at(i) = std::deque(queueItr,queuePtr->end());
    }
  }
  return vecTemp;
}


template <class DataType>
std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > VectorOfQueues<DataType>::copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp)
{
  bool stopWaiting;
  std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;

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
        if (_vectorOfQueues.at(i).empty()
          || !_vectorOfQueues.at(i).back())
        {
          continue;
        }
        timestamp = _vectorOfQueues.at(i).back()->_timestamp;
        if (timestamp > lastDataTimestamp)
        {
          lastDataTimestamp = timestamp;
          stopWaiting = true;
        }
      }
      return stopWaiting;
    });
  }
  return _vectorOfQueues;
}


template <class DataType>
std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > VectorOfQueues<DataType>::copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const std::chrono::duration<double> &duration)
{
  bool stopWaiting;
  std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
  std::shared_ptr<TimestampedData<DataType> > dataPtr;
  std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > vecTemp;

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
        if (_vectorOfQueues.at(i).empty()
          || !_vectorOfQueues.at(i).back())
        {
          continue;
        }
        timestamp = _vectorOfQueues.at(i).back()->_timestamp;
        if (timestamp > lastDataTimestamp)
        {
          lastDataTimestamp = timestamp;
          stopWaiting = true;
        }
      }
      return stopWaiting;
    });

    vecTemp.clear();
    vecTemp.resize(_vectorOfQueues.size());
    for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
    {
      if (_vectorOfQueues.at(i).empty()
        || !_vectorOfQueues.at(i).back())
      {
        continue;
      }
      dataPtr = _vectorOfQueues.at(i).back();
      auto timestampCutoff = dataPtr->_timestamp - duration;
      auto queueItr = _vectorOfQueues.at(i).begin();
      for ( ; _vectorOfQueues.at(i).end()!=queueItr; ++queueItr)
      {
        if (*queueItr
          && (*queueItr)->_timestamp > timestampCutoff)
        {
          break;
        }
      }
      vecTemp.at(i) = std::deque(queueItr,_vectorOfQueues.at(i).end());
    }
  }
  return vecTemp;
}


template <class DataType>
std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > VectorOfQueues<DataType>::copyVectorButWaitNewData(std::chrono::time_point<std::chrono::high_resolution_clock> & lastDataTimestamp, const unsigned int &numElements)
{
  bool stopWaiting;
  std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
  std::deque<std::shared_ptr<TimestampedData<DataType> > > * queuePtr;
  std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > vecTemp;

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
        if (_vectorOfQueues.at(i).empty()
          || !_vectorOfQueues.at(i).back())
        {
          continue;
        }
        timestamp = _vectorOfQueues.at(i).back()->_timestamp;
        if (timestamp > lastDataTimestamp)
        {
          lastDataTimestamp = timestamp;
          stopWaiting = true;
        }
      }
      return stopWaiting;
    });

    vecTemp.clear();
    vecTemp.resize(_vectorOfQueues.size());
    for (unsigned int i=0; i<_vectorOfQueues.size(); ++i)
    {
      if (_vectorOfQueues.at(i).empty()
        || !_vectorOfQueues.at(i).back())
      {
        continue;
      }
      queuePtr = &(_vectorOfQueues.at(i));
      if (numElements >= queuePtr->size())
      {
        // copy full queue
        vecTemp.at(i) = *queuePtr;
      }
      else
      {
        // copy last N elements of queue
        auto queueItr = std::prev(queuePtr->end(),numElements);
        vecTemp.at(i) = std::deque(queueItr,queuePtr->end());
      }
    }
  }
  return vecTemp;
}


template <class DataType>
std::vector<unsigned int> VectorOfQueues<DataType>::numElementsAfterTimestamp(const std::vector<std::deque<std::shared_ptr<TimestampedData<DataType> > > > & vectorTemp, const std::chrono::time_point<std::chrono::high_resolution_clock> & timestamp)
{
  std::vector<unsigned int> counts(vectorTemp.size(),0);
  unsigned int counter;

  for (unsigned int i=0; i<vectorTemp.size(); ++i)
  {
    counter = 0;
    for (auto dataItr=vectorTemp.at(i).rbegin(); vectorTemp.at(i).rend() != dataItr; ++dataItr)
    {
      if ((*dataItr)->_timestamp < timestamp)
      {
        break;
      }
      ++counter;
    }
    counts.at(i) = counter;
  }
  return counts;
}


template <class DataType>
unsigned int VectorOfQueues<DataType>::push_back(const unsigned int & index, const std::shared_ptr<TimestampedData<DataType> > & data, const std::chrono::duration<double> &duration)
{
  std::deque<std::shared_ptr<TimestampedData<DataType> > > * queuePtr;
  unsigned int queueSize = 0;

  {
    std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);

    // resize vector if needed
    if (_vectorOfQueues.size() <= index)
    {
      _vectorOfQueues.resize(index+1);
    }

    queuePtr = &(_vectorOfQueues.at(index));
    if (!data)
    {
      return queuePtr->size();
    }

    // pop old data and add data to queue
    auto timestamp = data->_timestamp - duration;
    queuePtr = &(_vectorOfQueues.at(index));
    while (!queuePtr->empty()
      && queuePtr->front()
      && queuePtr->front()->_timestamp < timestamp)
    {
      queuePtr->pop_front();
    }
    queuePtr->push_back(data);
    queueSize = queuePtr->size();
  }

  _cvAny.notify_all();
  return queueSize;
};


template <class DataType>
unsigned int VectorOfQueues<DataType>::push_back(const unsigned int & index, const std::shared_ptr<TimestampedData<DataType> > & data, const unsigned int & maxQueueSize)
{
  unsigned int queueSize = 0;
  {
    std::lock_guard<std::shared_mutex> lockGuard(_sharedMutex);

    // resize vector if needed
    if (_vectorOfQueues.size() <= index)
    {
      _vectorOfQueues.resize(index+1);
    }

    if (!data)
    {
      return _vectorOfQueues.at(index).size();
    }

    // add data to queue and pop if max size reached
    _vectorOfQueues.at(index).push_back(data);
    if (_vectorOfQueues.at(index).size() > maxQueueSize)
    {
      _vectorOfQueues.at(index).pop_front();
    }
    queueSize = _vectorOfQueues.at(index).size();
  }

  _cvAny.notify_all();
  return queueSize;
};


#endif // VECTOROFQUEUES_H
