#include "libuvheapadaptor.h"

extern "C" {
#include "heap-inl.h"
}

struct Node {
    heap_node n;
    QTimerInfo data;
};

int less_adaptor(const struct heap_node* a,
                const struct heap_node* b)
{
    const Node *aa = reinterpret_cast<const Node *>(a);
    const Node *bb = reinterpret_cast<const Node *>(b);

    return aa->data < bb->data;
}

struct heap* hp(LibuvHeapAdaptor::HeapData &d)
{
    return reinterpret_cast<struct heap *>(&d);
}

LibuvHeapAdaptor::LibuvHeapAdaptor()
{
    heap_init(hp(d));
    idToNode.reserve(10000);
}

LibuvHeapAdaptor::~LibuvHeapAdaptor()
{
    for( auto it = idToNode.begin(), end = idToNode.end(); it != end; ++it )
        delete reinterpret_cast<Node *>(*it);
}

int LibuvHeapAdaptor::registerTimer(int interval, int64_t current)
{
    Node *n = new Node;
    const int id = m_nextId++;
    n->data.create(id, interval, current);
    idToNode.append(n);
    heap_insert(hp(d), reinterpret_cast<heap_node *>(n), less_adaptor);
    return id;
}

void LibuvHeapAdaptor::unregisterTimer(int timerId)
{
    if( timerId < 0 || timerId >= idToNode.size() )
        return;
    void *ptr = idToNode.at(timerId);
    heap_remove(hp(d), reinterpret_cast<heap_node *>(ptr), less_adaptor);
    delete reinterpret_cast<Node *>(ptr);
    idToNode[timerId] = 0;
}

void LibuvHeapAdaptor::activate()
{
    Node *n = reinterpret_cast<Node *>(hp(d)->min);
    TimeSpec t = n->data.timeoutRef();
    do {
        heap_dequeue(hp(d), less_adaptor);
        n->data.advance();
        heap_insert(hp(d), reinterpret_cast<heap_node *>(n), less_adaptor);
        n = reinterpret_cast<Node *>(hp(d)->min);
    } while( n->data.timeoutRef() == t );
}

long LibuvHeapAdaptor::currentTopTime() const
{
    Node *n  = reinterpret_cast<Node *>(reinterpret_cast<const struct heap *>(&d)->min);
    return n->data.time();
}

