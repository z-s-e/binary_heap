#ifndef MYHEAPADAPTOR_H
#define MYHEAPADAPTOR_H

#include "timerdata.h"
#include "binary_heap.h"

class MyHeapAdaptor {
public:
    int registerTimer(int interval, int64_t current = 0);

    void unregisterTimer(int timerId);

    void activate();

    long currentTopTime() const;

private:
    binary_max_heap::heap<QTimerInfo, std::greater<QTimerInfo> > heap;
    int m_nextId = 0;
};

class MyHeapAdaptorPtr1 {
public:
    ~MyHeapAdaptorPtr1();

    int registerTimer(int interval, int64_t current = 0);

    void unregisterTimer(int timerId);

    void activate();

    long currentTopTime() const;

private:
    binary_max_heap::heap<QTimerInfoPtr, std::greater<QTimerInfoPtr> > heap;
    int m_nextId = 0;
};

class MyHeapAdaptorPtr2 {
public:
    ~MyHeapAdaptorPtr2();

    int registerTimer(int interval, int64_t current = 0);

    void unregisterTimer(int timerId);

    void activate();

    long currentTopTime() const;

private:
    binary_max_heap::heap<QTimerInfoPtr2, std::greater<QTimerInfoPtr2> > heap;
    int m_nextId = 0;
};

class MyHeapAdaptorPtr3 {
public:
    ~MyHeapAdaptorPtr3();

    int registerTimer(int interval, int64_t current = 0);

    void unregisterTimer(int timerId);

    void activate();

    long currentTopTime() const;

private:
    binary_max_heap::heap<QTimerInfoPtr3, std::greater<QTimerInfoPtr3> > heap;
    int m_nextId = 0;
};

#endif // MYHEAPADAPTOR_H
