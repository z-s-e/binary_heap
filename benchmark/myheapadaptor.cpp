#include "myheapadaptor.h"

#include <algorithm>

int MyHeapAdaptor::registerTimer(int interval, int64_t current)
{
    QTimerInfo v;
    const int id = m_nextId++;
    v.create(id, interval, current);
    heap.push(v);
    return id;
}

void MyHeapAdaptor::unregisterTimer(int timerId)
{
    auto it = std::find_if(heap.cbegin(), heap.cend(), [timerId] (const QTimerInfo &v) {
        return v.Id() == timerId;
    });
    if (it != heap.cend())
        heap.erase(it);
}

void MyHeapAdaptor::activate()
{
    QTimerInfo v = heap.top();
    const TimeSpec t = v.timeoutRef();
    do {
        //heap.pop();
        v.advance();
        //heap.push(v);
        //heap.update(heap.begin(), v);
        heap.decrease(heap.cbegin(), v);
        v = heap.top();
    } while( v.timeoutRef() == t );
}

long MyHeapAdaptor::currentTopTime() const
{
    return heap.top().time();
}


MyHeapAdaptorPtr1::~MyHeapAdaptorPtr1()
{
    if( heap.size() == 0 )
        return;

    auto first = heap.cbegin();
    for( auto it = heap.cend() - 1; it != first; --it ) {
        heap.take(it).destroy();
    }
    heap.take(first).destroy();
}

int MyHeapAdaptorPtr1::registerTimer(int interval, int64_t current)
{
    QTimerInfoPtr v;
    const int id = m_nextId++;
    v.create(id, interval, current);
    heap.push(v);
    return id;
}

void MyHeapAdaptorPtr1::unregisterTimer(int timerId)
{
    auto it = std::find_if(heap.cbegin(), heap.cend(), [timerId] (QTimerInfoPtr ptr) {
        return ptr.Id() == timerId;
    });
    if (it != heap.cend())
        heap.take(it).destroy();
}

void MyHeapAdaptorPtr1::activate()
{
    QTimerInfoPtr v = heap.top();
    const TimeSpec t = v.timeoutRef();
    do {
        //heap.pop();
        v.advance();
        //heap.push(v);
        //heap.update(heap.begin(), v);
        heap.decrease(heap.cbegin(), v);
        v = heap.top();
    } while( v.timeoutRef() == t );
}

long MyHeapAdaptorPtr1::currentTopTime() const
{
    return heap.top().time();
}


MyHeapAdaptorPtr2::~MyHeapAdaptorPtr2()
{
    if( heap.size() == 0 )
        return;

    auto first = heap.cbegin();
    for( auto it = heap.cend() - 1; it != first; --it ) {
        heap.take(it).destroy();
    }
    heap.take(first).destroy();
}

int MyHeapAdaptorPtr2::registerTimer(int interval, int64_t current)
{
    QTimerInfoPtr2 v;
    const int id = m_nextId++;
    v.create(id, interval, current);
    heap.push(v);
    return id;
}

void MyHeapAdaptorPtr2::unregisterTimer(int timerId)
{
    auto it = std::find_if(heap.cbegin(), heap.cend(), [timerId] (QTimerInfoPtr2 ptr) {
        return ptr.Id() == timerId;
    });
    if (it != heap.cend())
        heap.take(it).destroy();
}

void MyHeapAdaptorPtr2::activate()
{
    QTimerInfoPtr2 v = heap.top();
    const TimeSpec t = v.timeoutRef();
    do {
        //heap.pop();
        v.advance();
        //heap.push(v);
        heap.decrease(heap.cbegin(), v);
        v = heap.top();
    } while( v.timeoutRef() == t );
}

long MyHeapAdaptorPtr2::currentTopTime() const
{
    return heap.top().time();
}



MyHeapAdaptorPtr3::~MyHeapAdaptorPtr3()
{
}

int MyHeapAdaptorPtr3::registerTimer(int interval, int64_t current)
{
    QTimerInfoPtr3 v(new QTimerInfo);
    const int id = m_nextId++;
    v->create(id, interval, current);
    heap.push(std::move(v));
    return id;
}

void MyHeapAdaptorPtr3::unregisterTimer(int timerId)
{
    auto it = std::find_if(heap.cbegin(), heap.cend(), [timerId] (const QTimerInfoPtr3 &ptr) {
        return ptr->Id() == timerId;
    });
    if (it != heap.cend())
        heap.erase(it);
}

void MyHeapAdaptorPtr3::activate()
{
    const TimeSpec t = heap.top()->timeoutRef();
    do {
        QTimerInfoPtr3 v = heap.pop_top();
        v->advance();
        heap.push(std::move(v));
    } while( heap.top()->timeoutRef() == t );
}

long MyHeapAdaptorPtr3::currentTopTime() const
{
    return heap.top()->time();
}
