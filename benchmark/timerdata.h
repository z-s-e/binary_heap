#ifndef TIMERDATA_H
#define TIMERDATA_H

#include <sys/time.h>
typedef struct timespec TimeSpec;

#include <assert.h>

#include <memory>


// Qt timer structure (copied & simplified from corelib/kernel/qtimerinfo_unix_p.h)

struct QTimerInfo {
    int id = -2;                // - timer identifier
    int interval = 0;           // - timer interval
    int timerType = 0;          // - timer type
    TimeSpec timeout = {0, 0};  // - when to actually fire
    void *obj = nullptr;        // - object to receive event
    QTimerInfo **activateRef = nullptr; // - ref from activateTimers

    void create(int id_, int interval_, int64_t current)
    {
        id = id_;
        interval = interval_;
        assert(interval > 0);
        timeout.tv_nsec = current;
    }

    void advance()
    {
        timeout.tv_nsec += interval; // just some arbitrary scale, doesn't matter here
    }

    int Id() const
    {
        return id;
    }

    long time() const
    {
        return timeout.tv_nsec;
    }

    const TimeSpec &timeoutRef() const { return timeout; }
};

inline bool operator<(const TimeSpec &t1, const TimeSpec &t2)
{
    return t1.tv_sec < t2.tv_sec
            || (t1.tv_sec == t2.tv_sec && t1.tv_nsec < t2.tv_nsec);
}

inline bool operator>(const TimeSpec &t1, const TimeSpec &t2)
{
    return t1.tv_sec > t2.tv_sec
            || (t1.tv_sec == t2.tv_sec && t1.tv_nsec > t2.tv_nsec);
}

inline bool operator==(const TimeSpec &t1, const TimeSpec &t2)
{
    return t1.tv_sec == t2.tv_sec && t1.tv_nsec == t2.tv_nsec;
}

inline bool operator<(const QTimerInfo &t1, const QTimerInfo &t2)
{
    return t1.timeout < t2.timeout;
}

inline bool operator>(const QTimerInfo &t1, const QTimerInfo &t2)
{
    return t1.timeout > t2.timeout;
}

// Copyable (raw) pointer type, acts as a value - this is a dangerous hack of course
struct QTimerInfoPtr {
    QTimerInfo *info = nullptr;

    void create(int id_, int interval_, int64_t current)
    {
        info = new QTimerInfo;
        info->create(id_, interval_, current);
    }

    void advance()
    {
        info->advance();
    }

    void destroy()
    {
        delete info;
    }

    int Id() const
    {
        return info->id;
    }

    long time() const
    {
        return info->time();
    }

    const TimeSpec &timeoutRef() const
    {
        return info->timeoutRef();
    }
};

inline bool operator<(const QTimerInfoPtr &t1, const QTimerInfoPtr &t2)
{
    return *(t1.info) < *(t2.info);
}

inline bool operator>(const QTimerInfoPtr &t1, const QTimerInfoPtr &t2)
{
    return *(t1.info) > *(t2.info);
}

// As QTimerInfoPtr, but store a copy of timeout inline to avoid a pointer indirection when using operator<
struct QTimerInfoPtr2 {
    TimeSpec timeout;
    QTimerInfo *info = nullptr;

    void create(int id_, int interval_, int64_t current)
    {
        info = new QTimerInfo;
        info->create(id_, interval_, current);
        timeout = info->timeout;
    }

    void advance()
    {
        info->advance();
        timeout = info->timeout;
    }

    void destroy()
    {
        delete info;
    }

    int Id() const
    {
        return info->id;
    }

    long time() const
    {
        return timeout.tv_nsec;
    }

    const TimeSpec &timeoutRef() const
    {
        return timeout;
    }
};

inline bool operator<(const QTimerInfoPtr2 &t1, const QTimerInfoPtr2 &t2)
{
    return (t1.timeout) < (t2.timeout);
}

inline bool operator>(const QTimerInfoPtr2 &t1, const QTimerInfoPtr2 &t2)
{
    return (t1.timeout) > (t2.timeout);
}


// As QTimerInfoPtr, but using a safe unique_ptr instead
typedef std::unique_ptr<QTimerInfo> QTimerInfoPtr3;

inline bool operator<(const QTimerInfoPtr3 &t1, const QTimerInfoPtr3 &t2)
{
    return (t1->timeout) < (t2->timeout);
}

inline bool operator>(const QTimerInfoPtr3 &t1, const QTimerInfoPtr3 &t2)
{
    return (t1->timeout) > (t2->timeout);
}

#endif // TIMERDATA_H

