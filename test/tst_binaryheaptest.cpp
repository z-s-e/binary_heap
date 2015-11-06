#include <QString>
#include <QtTest>
#include <QDebug>

#include <algorithm>

#include "binary_heap.h"

template class binary_max_heap::heap< int >;


template<typename DiffType>
DiffType heapFirstChild(DiffType pos)
{
    return 2 * pos + 1;
}

template<typename DiffType>
DiffType heapSecondChild(DiffType pos)
{
    return 2 * pos + 2;
}

template<typename DiffType>
DiffType heapParent(DiffType pos)
{
    return (pos - 1) / 2;
}

template<class Heap>
bool isBinaryHeap(const Heap& h)
{
    const auto first = h.begin();
    const auto n = h.size();
    const auto comp = h.compare();

    for( auto i = n > 0 ? n - 1 : 0; i > 0; --i ) {
        const auto parent = heapParent(i);
        if( comp(*(first + parent), *(first + i)) ) {
            qWarning() << "Heap property violated at" << i << "( size" << n << ")";
            return false;
        }
    }

    return true;
}

// assumes h.compare() is a total order (!comp(a, b) && !comp(b, a) => a == b)
template<class Heap, typename U>
bool checkedPush(Heap& h, U&& value)
{
    const auto comp = h.compare();

    auto c_old = h.container();
    c_old.push_back(value);
    std::sort(c_old.begin(), c_old.end(), comp);

    h.push(std::forward<U>(value));

    if( ! isBinaryHeap(h) )
        return false;

    auto c_new = h.container();
    std::sort(c_new.begin(), c_new.end(), comp);

    return c_old == c_new;
}

// assumes h.compare() is a total order
template<class Heap>
bool checkedPop(Heap& h)
{
    const auto comp = h.compare();

    auto c_old = h.container();
    c_old.erase(c_old.begin());
    std::sort(c_old.begin(), c_old.end(), comp);

    h.pop();

    if( ! isBinaryHeap(h) )
        return false;

    auto c_new = h.container();
    std::sort(c_new.begin(), c_new.end(), comp);

    return c_old == c_new;
}


class binary_heap_debug_position_tracker {
public:
    template< typename Heap, typename T, typename DiffType >
    static void insert(const Heap& /*heap*/, const T& value, DiffType position)
    {
        qDebug() << value << "inserted to position" << position;
    }

    template< typename Heap, typename T, typename DiffType >
    static void move(const Heap& /*heap*/, const T& value,
                     DiffType oldPosition, DiffType newPosition)
    {
        qDebug() << value << "moved from" << oldPosition << "to" << newPosition;
    }

    template< typename Heap, typename T, typename DiffType >
    static void remove(const Heap& /*heap*/, const T& value, DiffType position)
    {
        qDebug() << value << "removed from position" << position;
    }
};


struct TestValue {
    TestValue(int64_t k = 0) : key(k), pos(new ptrdiff_t) {}
    TestValue(TestValue&&) = default;
    TestValue& operator= (TestValue&&) = default;

    bool operator< (const TestValue &rhs) const
    {
        return key < rhs.key;
    }

    int64_t key;
    std::unique_ptr<ptrdiff_t> pos;
};

class binary_heap_TestValue_position_tracker {
public:
    template< typename Heap >
    static void insert(const Heap& /*heap*/, const TestValue& value, ptrdiff_t position)
    {
        *(value.pos) = position;
    }

    template< typename Heap >
    static void move(const Heap& /*heap*/, const TestValue& value, ptrdiff_t, ptrdiff_t newPosition)
    {
        *(value.pos) = newPosition;
    }

    template< typename Heap, typename T, typename DiffType >
    static void remove(const Heap& /*heap*/, const T& /*value*/, DiffType /*position*/) {}
};

template<class Heap>
bool checkPosition(const Heap& h)
{
    const long n = h.size();

    for( auto i = 0l; i < n; ++i ) {
        if( *(h.at(i).pos) != i ) {
            qWarning() << "Position mismatch:" << *(h.at(i).pos) << ", but actual position is" << i;
            return false;
        }
    }

    return true;
}


class BinaryHeapTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testCase1()
    {
        binary_max_heap::heap<double, std::less<double>, binary_heap_debug_position_tracker> h;
        //binary_max_heap::heap<double> h;

        QVERIFY(checkedPush(h, 3));
        QVERIFY(checkedPush(h, 1));
        QVERIFY(checkedPush(h, 2));
        QVERIFY(checkedPush(h, 5));
        QVERIFY(checkedPush(h, 4));
        QVERIFY(checkedPush(h, 3));

        QVERIFY(checkedPop(h));
        QVERIFY(checkedPop(h));
        QVERIFY(checkedPop(h));
        QVERIFY(checkedPop(h));
        QVERIFY(checkedPop(h));
        QVERIFY(checkedPop(h));

        QVERIFY(h.empty());
    }

    void testCase2()
    {
        binary_max_heap::heap<TestValue, std::less<TestValue>, binary_heap_TestValue_position_tracker> h;

        h.push(3); QVERIFY(checkPosition(h));
        h.push(1); QVERIFY(checkPosition(h));
        h.push(2); QVERIFY(checkPosition(h));
        h.push(5); QVERIFY(checkPosition(h));
        h.push(4); QVERIFY(checkPosition(h));
        h.push(3); QVERIFY(checkPosition(h));

        h.pop(); QVERIFY(checkPosition(h));
        h.pop(); QVERIFY(checkPosition(h));
        h.pop(); QVERIFY(checkPosition(h));
        h.pop(); QVERIFY(checkPosition(h));
        h.pop(); QVERIFY(checkPosition(h));
        h.pop(); QVERIFY(checkPosition(h));

        QVERIFY(h.empty());
    }
};


QTEST_APPLESS_MAIN(BinaryHeapTest)

#include "tst_binaryheaptest.moc"
