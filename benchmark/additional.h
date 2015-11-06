#ifndef ADDITIONAL_H
#define ADDITIONAL_H

/* This header contains some additional adaptors for testing
 * (only used when TEST_ADDITIONAL is defined).
 */

#include "timerdata.h"

#include <QList>
#include <QMultiMap>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <boost/heap/binomial_heap.hpp>
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/pairing_heap.hpp>

#include "libuvheapadaptor.h"

#include <algorithm>

template< class Container >
class TimerQueueWrapper {
public:
    typedef typename Container::value_type ValueType;

    int registerTimer(int interval, int64_t current = 0)
    {
        ValueType v;
        const int id = m_nextId++;
        v.create(id, interval, current);
        m_data.push(v);
        return id;
    }

    void unregisterTimer(int timerId)
    {
        m_data.remove(timerId);
    }

    void activate()
    {
        ValueType v = m_data.top();
        const auto t = v.timeoutRef();
        do {
            m_data.pop();
            v.advance();
            m_data.push(v);
            v = m_data.top();
        } while( v.timeoutRef() == t );
    }

    long currentTopTime() const
    {
        return m_data.top().time();
    }

private:
    int m_nextId = 0;
    Container m_data;
};


class QListAdaptor : public QList<QTimerInfo> {
public:
    typedef value_type T;

    const T & top() const { return this->first(); }

    void pop() { this->removeFirst(); }

    void push(const T &ti)
    {
        int index = this->size();
        while (index--) {
            if (!(ti < this->at(index)))
                break;
        }
        this->insert(index+1, ti);
    }

    void remove(int timerId)
    {
        auto it = std::find_if(this->begin(), this->end(), [timerId] (const value_type &v) {
            return v.Id() == timerId;
        });
        if (it != this->end())
            this->erase(it);
    }
};

class QMultiMapAdaptor : public QMultiMap<QTimerInfo, bool> {
public:
    typedef key_type value_type;

    const QTimerInfo & top() const { return firstKey(); }

    void pop() { erase(begin()); }

    void push(const QTimerInfo &ti) { insert(ti, true); }

    void remove(int timerId)
    {
        for (auto it = begin(); it != end(); ++it) {
            if (it.key().Id() == timerId) {
                erase(it);
                return;
            }
        }
    }
};

class BoostMultiIndexAdaptor {
public:
    typedef QTimerInfo value_type;

    const QTimerInfo & top() const
    {
        return *(c.get<1>().begin());
    }

    void pop()
    {
        auto &idx = c.get<1>();
        idx.erase(idx.begin());
    }

    void push(const QTimerInfo &ti)
    {
        c.insert(ti);
    }

    void remove(int timerId)
    {
        c.get<0>().erase(timerId);
    }

private:
    typedef boost::multi_index_container<
        QTimerInfo,
        boost::multi_index::indexed_by<
          boost::multi_index::ordered_unique<boost::multi_index::member<
            QTimerInfo, int, &QTimerInfo::id
          > >,
          boost::multi_index::ordered_non_unique<boost::multi_index::member<
            QTimerInfo, TimeSpec, &QTimerInfo::timeout
          >>
        >
      > Cont;

    Cont c;
};

template< class BoostHeap >
class BoostHeapAdaptor: public BoostHeap {
public:
    typedef typename BoostHeap::value_type value_type;

    void remove(int timerId)
    {
        auto it = std::find_if(this->begin(), this->end(), [timerId] (const value_type &v) {
            return v.Id() == timerId;
        });
        if (it != this->end()) {
            this->erase(this->s_handle_from_iterator(it));
        }
    }
};

using BoostHeapBinomialAdaptor = BoostHeapAdaptor<boost::heap::binomial_heap<QTimerInfo, boost::heap::compare<std::greater<QTimerInfo> > > >;

using BoostHeapFibonacciAdaptor = BoostHeapAdaptor<boost::heap::fibonacci_heap<QTimerInfo, boost::heap::compare<std::greater<QTimerInfo> > > >;

using BoostHeapPairingAdaptor = BoostHeapAdaptor<boost::heap::pairing_heap<QTimerInfo, boost::heap::compare<std::greater<QTimerInfo> > > >;


#endif // ADDITIONAL_H

