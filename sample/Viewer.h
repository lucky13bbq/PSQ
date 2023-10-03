#ifndef VIEWER_H
#define VIEWER_H

#include "PSQ/BaseThread.h"
#include "PSQ/Queue.h"
#include "PSQ/VectorOfQueues.h"


class Viewer : public BaseThread
{
  public:
    Viewer();
    void run(Queue<float> &iMinNumbers, Queue<float> &iMaxNumbers, Queue<float> &iMultipliedBy10, Queue<float> &iSums, VectorOfQueues<float> & iRandomNumbers);
    virtual ~Viewer();

  protected:

  private:
};

#endif // VIEWER_H
