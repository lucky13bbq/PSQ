#ifndef RANDOMNUMBERGENERATOR_H
#define RANDOMNUMBERGENERATOR_H

#include "PSQ/BaseThread.h"
#include "PSQ/VectorOfQueues.h"



class RandomNumberGenerator : public BaseThread
{
  public:
    RandomNumberGenerator();
    void run(const unsigned int & index, VectorOfQueues<float> & oRandomNumbers);
    virtual ~RandomNumberGenerator();

  protected:

  private:
};

#endif // RANDOMNUMBERGENERATOR_H
