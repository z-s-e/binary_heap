#include "stdvalpqadaptor.h"

#include <algorithm>

int StdValPQAdaptor::registerTimer(int interval, int64_t current)
{
    QTimerInfo v;
    const int id = m_nextId++;
    v.create(id, interval, current);
    push(v);
    return id;
}

void StdValPQAdaptor::unregisterTimer(int timerId)
{
    auto it = std::find_if(c.begin(), c.end(), [timerId] (const QTimerInfo &i) {
        return i.Id() == timerId;
    });
    if (it != c.end()) {
        c.erase(it);
        std::make_heap(c.begin(), c.end(), comp);
        // Note: in theory one can implement remove more efficiently, but sadly
        //       std::priority_queue does not support adding that algorithm in a
        //       portable way.
    }
}

void StdValPQAdaptor::activate()
{
    QTimerInfo v = top();
    TimeSpec t = v.timeoutRef();
    do {
        pop();
        v.advance();
        push(v);
        v = top();
    } while( v.timeoutRef() == t );
}

long StdValPQAdaptor::currentTopTime() const
{
    return top().time();
}
