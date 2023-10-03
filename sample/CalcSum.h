#ifndef CALCSUM_H
#define CALCSUM_H

#include "PSQ/BaseThread.h"
#include "PSQ/Queue.h"
#include "PSQ/VectorOfQueues.h"


class CalcSum : public BaseThread
{
  public:
    CalcSum();
    void run(Queue<float> & iMinNumbers, Queue<float> & iMaxNumbers, Queue<float> & oSums);
    virtual ~CalcSum();

  protected:

  private:
};

#endif // CALCSUM_H
