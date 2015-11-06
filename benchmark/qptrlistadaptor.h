#ifndef QPTRLISTADAPTOR_H
#define QPTRLISTADAPTOR_H

#include "timerdata.h"
#include <QList>


class QPtrListAdaptor {
public:
    ~QPtrListAdaptor();

    int registerTimer(int interval, int64_t current = 0);

    void unregisterTimer(int timerId);

    void activate();

    long currentTopTime() const;

private:
    void push(const QTimerInfoPtr &ti);

    QList<QTimerInfoPtr> list;
    int m_nextId = 0;
};

#endif // QPTRLISTADAPTOR_H
