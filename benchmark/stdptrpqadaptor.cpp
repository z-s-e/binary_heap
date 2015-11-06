#include "stdptrpqadaptor.h"

#include <algorithm>

StdPtrPQAdaptor::~StdPtrPQAdaptor()
{
    for (auto &v : c)
        v.destroy();
}

int StdPtrPQAdaptor::registerTimer(int interval, int64_t current)
{
    QTimerInfoPtr v;
    const int id = m_nextId++;
    v.create(id, interval, current);
    push(v);
    return id;
}

void StdPtrPQAdaptor::unregisterTimer(int timerId)
{
    auto it = std::find_if(c.begin(), c.end(), [timerId] (QTimerInfoPtr ptr) {
        return ptr.Id() == timerId;
    });
    if (it != c.end()) {
        it->destroy();
        c.erase(it);
        std::make_heap(c.begin(), c.end(), comp);
        // Note: in theory one can implement remove more efficiently, but sadly
        //       std::priority_queue does not support adding that algorithm in a
        //       portable way.
    }
}

void StdPtrPQAdaptor::activate()
{
    QTimerInfoPtr v = top();
    TimeSpec t = v.timeoutRef();
    do {
        pop();
        v.advance();
        push(v);
        v = top();
    } while( v.timeoutRef() == t );
}

long StdPtrPQAdaptor::currentTopTime() const
{
    return top().time();
}
