#ifndef FINDMAX_H
#define FINDMAX_H

#include "PSQ/BaseThread.h"
#include "PSQ/Queue.h"
#include "PSQ/VectorOfQueues.h"


class FindMax : public BaseThread
{
  public:
    FindMax();
    void run(VectorOfQueues<float> & iRandomNumbers, Queue<float> &oMaxNumbers);
    virtual ~FindMax();

  protected:

  private:
};

#endif // FINDMAX_H
