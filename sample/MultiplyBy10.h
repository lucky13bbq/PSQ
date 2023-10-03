#ifndef MULTIPLYBY10_H
#define MULTIPLYBY10_H

#include "PSQ/BaseThread.h"
#include "PSQ/Queue.h"


class MultiplyBy10 : public BaseThread
{
  public:
    MultiplyBy10();
    void run(Queue<float> & iMaxNumbers, Queue<float> & oMultipliedBy10);
    virtual ~MultiplyBy10();

  protected:

  private:
};

#endif // MULTIPLYBY10_H
