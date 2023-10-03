#ifndef FINDMIN_H
#define FINDMIN_H

#include "PSQ/BaseThread.h"
#include "PSQ/Queue.h"
#include "PSQ/VectorOfQueues.h"

class FindMin : public BaseThread
{
  public:
    FindMin();
    void run(VectorOfQueues<float> & iRandomNumbers, Queue<float> &oMinNumbers);
    virtual ~FindMin();

  protected:

  private:

};

#endif // FINDMIN_H
