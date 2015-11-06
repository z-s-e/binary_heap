#include <QString>
#include <QtTest>

#include "qptrlistadaptor.h"
#include "stdvalpqadaptor.h"
#include "stdptrpqadaptor.h"
#include "myheapadaptor.h"

#define TEST_ADDITIONAL

#ifdef TEST_ADDITIONAL
#include "additional.h"
#endif


// Generic test loop

static const int s_loopCount = 100000;
static const int s_tmpTimerInterval = 32;
long s_lastTime = 0;
long s_expectedLast = 0;

template< class Queue >
void perfTest()
{
    Queue timerQueue;
    int tmpId = -1; // for temporary timer

    // some permanent timer
    timerQueue.registerTimer(16);
    timerQueue.registerTimer(33);
    timerQueue.registerTimer(50);
    timerQueue.registerTimer(97);
    timerQueue.registerTimer(250);

#ifdef TEST_ADDITIONAL
    timerQueue.registerTimer(1000);
    timerQueue.registerTimer(1000);
    timerQueue.registerTimer(1234);
    timerQueue.registerTimer(3000);
    timerQueue.registerTimer(10000);
    timerQueue.registerTimer(17);
    timerQueue.registerTimer(25);
    timerQueue.registerTimer(35);
    timerQueue.registerTimer(74);
    timerQueue.registerTimer(500);
    timerQueue.registerTimer(987);
    timerQueue.registerTimer(1333);
    timerQueue.registerTimer(4711);
    timerQueue.registerTimer(2000);
    timerQueue.registerTimer(9999000);
#endif

    // the loop

    for( int i = 0; i < s_loopCount; ++i ) {
        timerQueue.activate();

        if( i % s_tmpTimerInterval == 0 ) {
            timerQueue.unregisterTimer(tmpId);
            tmpId = timerQueue.registerTimer(100, timerQueue.currentTopTime());
        }
    }

    s_lastTime = timerQueue.currentTopTime();
}



class PriorityQueueBench : public QObject
{
    Q_OBJECT

public:
    PriorityQueueBench();

private Q_SLOTS:
    void qListPtr();
    void stdPQValue();
    void myHeap();
    void stdPQPtr();
    void myHeapPtr();

#ifdef TEST_ADDITIONAL
    void myHeapPtr2();
    void myHeapPtr3();
    void qListValue();
    void qMultiMap();
    void boostMultiIdx();
    void libuvHeap();
    void boostHeap();
#endif
};

PriorityQueueBench::PriorityQueueBench()
{
    perfTest<QPtrListAdaptor>();
    s_expectedLast = s_lastTime;
}

void PriorityQueueBench::qListPtr()
{
    QBENCHMARK {
        perfTest<QPtrListAdaptor>();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::stdPQValue()
{
    QBENCHMARK {
        perfTest<StdValPQAdaptor>();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::myHeap()
{
    QBENCHMARK {
        perfTest<MyHeapAdaptor>();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::stdPQPtr()
{
    QBENCHMARK {
        perfTest<StdPtrPQAdaptor>();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::myHeapPtr()
{
    QBENCHMARK {
        perfTest<MyHeapAdaptorPtr1>();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

#ifdef TEST_ADDITIONAL
void PriorityQueueBench::myHeapPtr2()
{
    QBENCHMARK {
        perfTest<MyHeapAdaptorPtr2>();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::myHeapPtr3()
{
    QBENCHMARK {
        perfTest<MyHeapAdaptorPtr3>();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::qListValue()
{
    QBENCHMARK {
        perfTest<TimerQueueWrapper<QListAdaptor> >();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::qMultiMap()
{
    QBENCHMARK {
        perfTest<TimerQueueWrapper<QMultiMapAdaptor> >();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::boostMultiIdx()
{
    QBENCHMARK {
        perfTest<TimerQueueWrapper<BoostMultiIndexAdaptor> >();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::libuvHeap()
{
    QBENCHMARK {
        perfTest<LibuvHeapAdaptor>();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}

void PriorityQueueBench::boostHeap()
{
    QBENCHMARK {
        perfTest<TimerQueueWrapper<BoostHeapBinomialAdaptor> >();
    }
    QCOMPARE(s_lastTime, s_expectedLast);
}
#endif

QTEST_APPLESS_MAIN(PriorityQueueBench)

#include "tst_priorityqueuebench.moc"
