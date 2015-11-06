#ifndef LIBUVHEAPADAPTOR_H
#define LIBUVHEAPADAPTOR_H

#include "timerdata.h"
#include <QVector>

class LibuvHeapAdaptor
{
public:
    struct HeapData {
        void *min;
        unsigned int nelts;
    };

    LibuvHeapAdaptor();
    ~LibuvHeapAdaptor();

    int registerTimer(int interval, int64_t current = 0);

    void unregisterTimer(int timerId);

    void activate();

    long currentTopTime() const;

private:
    HeapData d;
    QVector<void*> idToNode;
    int m_nextId = 0;
};

#endif // LIBUVHEAPADAPTOR_H
