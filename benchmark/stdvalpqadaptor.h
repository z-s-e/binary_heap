#ifndef STDVALPQADAPTOR_H
#define STDVALPQADAPTOR_H

#include "timerdata.h"
#include <queue>


class StdValPQAdaptor : private std::priority_queue<QTimerInfo, std::vector<QTimerInfo>, std::greater<QTimerInfo> > {
public:
    int registerTimer(int interval, int64_t current = 0);

    void unregisterTimer(int timerId);

    void activate();

    long currentTopTime() const;

private:
    int m_nextId = 0;
};

#endif // STDVALPQADAPTOR_H
