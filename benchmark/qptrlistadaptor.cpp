#include "qptrlistadaptor.h"

#include <algorithm>

QPtrListAdaptor::~QPtrListAdaptor()
{
    for (auto &v : list)
        v.destroy();
}

int QPtrListAdaptor::registerTimer(int interval, int64_t current)
{
    QTimerInfoPtr v;
    const int id = m_nextId++;
    v.create(id, interval, current);
    push(v);
    return id;
}

void QPtrListAdaptor::unregisterTimer(int timerId)
{
    auto it = std::find_if(list.begin(), list.end(), [timerId] (QTimerInfoPtr ptr) {
        return ptr.Id() == timerId;
    });
    if (it != list.end()) {
        it->destroy();
        list.erase(it);
    }
}

void QPtrListAdaptor::activate()
{
    QTimerInfoPtr v = list.first();
    TimeSpec t = v.timeoutRef();
    do {
        list.removeFirst();
        v.advance();
        push(v);
        v = list.first();
    } while( v.timeoutRef() == t );
}

long QPtrListAdaptor::currentTopTime() const
{
    return list.first().time();
}

void QPtrListAdaptor::push(const QTimerInfoPtr &ti)
{
    int index = list.size();
    while (index--) {
        if (!(ti < list.at(index)))
            break;
    }
    list.insert(index+1, ti);
}
