#ifndef STDPTRPQADAPTOR_H
#define STDPTRPQADAPTOR_H

#include "timerdata.h"
#include <queue>


class StdPtrPQAdaptor : private std::priority_queue<QTimerInfoPtr, std::vector<QTimerInfoPtr>, std::greater<QTimerInfoPtr> > {
    // need inheritance here (instead of just a member) to access the protected container member of std::priority_queue
public:
    ~StdPtrPQAdaptor();

    int registerTimer(int interval, int64_t current = 0);

    void unregisterTimer(int timerId);

    void activate();

    long currentTopTime() const;

private:
    int m_nextId = 0;
};

#endif // STDPTRPQADAPTOR_H
