/* Copyright 2015 Zeno Sebastian Endemann <zeno.endemann@googlemail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef BINARY_MAX_HEAP_H
#define BINARY_MAX_HEAP_H

#include <memory>
#include <vector>

namespace binary_max_heap {

/// Standard textbook binary heap algorithms.
/// The Heap template is expected to have a slightly augmented API compared to
/// std::vector. See the default heap implementation for an example usage.
template< class Heap >
struct algorithm {
    typedef typename Heap::value_type             value_type;
    typedef typename Heap::iterator               iterator;
    typedef typename Heap::difference_type        difference_type;
    typedef typename Heap::compare_type           compare_type;

    // Index helper

    static difference_type parent_index(const difference_type idx)
    {
        return (idx - 1) / 2;
    }

    static difference_type second_child_index(const difference_type idx)
    {
        return 2 * (idx + 1);
    }

    static difference_type first_child_index(const difference_type idx)
    {
        return second_child_index(idx) - 1;
    }

    static iterator first_leaf(Heap *heap)
    {
        const iterator first = heap->begin();
        const difference_type size = heap->end() - first;
        return first + (size / 2);
    }

    // Basic algorithms

    template< typename T >
    static difference_type up_heap(Heap *heap,
                                   difference_type holeIndex,
                                   const T& value,
                                   const difference_type topIndex = difference_type(0))
    {
        const compare_type comp = heap->compare();
        const iterator first = heap->begin();

        difference_type parent = holeIndex < 1 ? 0 : parent_index(holeIndex);
        while( holeIndex > topIndex && comp(*(first + parent), value) ) {
            heap->move_element(first, parent, holeIndex);
            holeIndex = parent;
            parent = parent_index(holeIndex);
        }

        return holeIndex;
    }

    template< typename T >
    static void adjust_heap(Heap *heap,
                            difference_type holeIndex,
                            T&& value,
                            const difference_type size,
                            const difference_type topIndex = difference_type(0))
    {
        const compare_type comp = heap->compare();
        const iterator first = heap->begin();

        difference_type child = second_child_index(holeIndex);
        while( child < size ) {
            if( comp(*(first + child), *(first + child - 1)) )
                --child;
            heap->move_element(first, child, holeIndex);
            holeIndex = child;
            child = second_child_index(holeIndex);
        }
        if( child == size ) {
            --child;
            heap->move_element(first, child, holeIndex);
            holeIndex = child;
        }

        holeIndex = up_heap(heap, holeIndex, value, topIndex);
        heap->insert_element(first, holeIndex, std::forward<T>(value));
    }

    template< typename T >
    static void heapify(Heap *heap,
                        difference_type idx,
                        T&& value)
    {
        const compare_type comp = heap->compare();
        const iterator first = heap->begin();
        const difference_type size = heap->end() - first;

        difference_type secondChild = second_child_index(idx);
        while( secondChild < size ) {
            const difference_type firstChild = secondChild - 1;
            difference_type maxIdx = idx;

            if( comp(value, *(first + secondChild)) ) {
                if( comp(*(first + secondChild), *(first + firstChild)) )
                    maxIdx = firstChild;
                else
                    maxIdx = secondChild;
            } else if( comp(value, *(first + firstChild)) ) {
                maxIdx = firstChild;
            }

            if( maxIdx == idx ) {
                heap->insert_element(first, idx, std::forward<T>(value));
                return;
            }

            heap->move_element(first, maxIdx, idx);
            idx = maxIdx;
            secondChild = second_child_index(idx);
        }
        if( secondChild == size ) {
            const difference_type firstChild = secondChild - 1;
            if( comp(value, *(first + firstChild)) ) {
                heap->move_element(first, firstChild, idx);
                idx = firstChild;
            }
        }
        heap->insert_element(first, idx, std::forward<T>(value));
    }

    // Higher level operations using the basic algorithms

    template< typename T >
    static void push(Heap *heap, T&& value)
    {
        heap->push_back({});

        const iterator first = heap->begin();
        const difference_type pos = up_heap(heap, heap->end() - first - 1, value);

        heap->insert_element(first, pos, std::forward<T>(value));
    }

    static void pop(Heap *heap)
    {
        const iterator first = heap->begin();

        heap->remove_element(first, difference_type(0), *first);
        fill_space(heap, difference_type(0));
    }

    static void eraze(Heap *heap, const iterator position)
    {
        const iterator first = heap->begin();
        const difference_type p = position - first;

        heap->remove_element(first, p, *position);
        fill_space(heap, p);
    }

    static void fill_space(Heap *heap, const difference_type holeIndex)
    {
        const iterator first = heap->begin();
        const difference_type len = heap->end() - first;
        const difference_type lastIdx = len - 1;

        if( holeIndex < lastIdx ) {
            value_type last = std::move(heap->back());
            heap->remove_element(first, lastIdx, last);
            adjust_heap(heap, holeIndex, std::move(last), lastIdx);
        }

        heap->pop_back();
    }

    static void make_heap(Heap *heap)
    {
        const iterator first = heap->begin();
        const difference_type size = heap->end() - first;

        for( auto i = (size / 2) - 1; i >= 0; --i ) {
            heap->remove_element(first, i, *(first + i));
            value_type value = std::move(*(first + i));
            heapify(heap, i, value);
        }
    }
};


/// This hook in the heap class can be useful for debugging, algorithm visualisation
/// and, most importantly, creating reverse lookup maps.
/// The default no-op implementation does nothing and is hopefully optimized away by
/// the compiler.
struct position_tracker_nop {
    template< typename Heap, typename T, typename DiffType >
    static void insert(const Heap& /*heap*/, const T& /*value*/, DiffType /*position*/) {}

    template< typename Heap, typename T, typename DiffType >
    static void move(const Heap& /*heap*/, const T& /*value*/,
                     DiffType /*oldPosition*/, DiffType /*newPosition*/) {}

    template< typename Heap, typename T, typename DiffType >
    static void remove(const Heap& /*heap*/, const T& /*value*/, DiffType /*position*/) {}
};


/// Default heap implementation using std::vector.
template< typename T,
          class Compare = std::less<T>,
          class PositionTracker = position_tracker_nop,
          class Alloc = std::allocator<T> >
class heap {
    friend struct algorithm<heap<T, Compare, PositionTracker, Alloc>>;
    typedef algorithm<heap<T, Compare, PositionTracker, Alloc>> alg;

public:
    typedef T                                               value_type;
    typedef std::vector<T, Alloc>                           container_type;
    typedef typename container_type::const_iterator         const_iterator;
    typedef typename container_type::const_reference        const_reference;
    typedef typename container_type::size_type              size_type;
    typedef typename container_type::difference_type        difference_type;
    typedef Compare                                         compare_type;
    typedef typename container_type::allocator_type         allocator_type;

    heap() = default;

    explicit heap(const container_type& ctnr, const Compare& comp = {})
        : d(ctnr, comp) { alg::make_heap(this); }
    explicit heap(container_type&& ctnr, const Compare& comp = {})
        : d(std::move(ctnr), comp) { alg::make_heap(this); }
    heap(std::initializer_list<value_type> il)
        : d(il) { alg::make_heap(this); }

    heap(const heap& other) = default;
    heap(heap&& other) = default;

    heap(const container_type& ctnr, const allocator_type& alloc, const Compare& comp = {})
        : d(ctnr, comp, alloc) { alg::make_heap(this); }
    heap(container_type&& ctnr, const allocator_type& alloc, const Compare& comp = {})
        : d(std::move(ctnr), comp, alloc) { alg::make_heap(this); }

    heap(const heap& other, const allocator_type& alloc)
        : d(other.d.c, other.d, alloc) {}
    heap(heap&& other, const allocator_type& alloc)
        : d(std::move(other.d.c), other.d, alloc) {}


    heap& operator=(const heap& other) = default;
    heap& operator=(heap&& other) = default;
    heap& operator=(std::initializer_list<value_type> il)
    {
        d.c = il;
        alg::make_heap(this);
        return *this;
    }


    // std::priority_queue API

    bool empty() const { return d.c.empty(); }
    size_t size() const { return d.c.size(); }
    const T& top() const { return d.c.front(); }

    template< typename U >
    void push(U&& value) { alg::push(this, std::forward<U>(value)); }

    void pop() { alg::pop(this); }


    // Additional API

    T pop_top() { return take(begin()); }

    void erase(const_iterator position)
    {
        const iterator first = begin();
        const difference_type p = position - first;

        remove_element(first, p, *position);
        alg::fill_space(this, p);
    }

    T take(const_iterator position)
    {
        const iterator first = begin();
        const difference_type p = position - first;

        T value = std::move(*(first + p));
        remove_element(first, p, value);
        alg::fill_space(this, p);
        return value;
    }

    template< typename U >
    void update(const_iterator position, U&& newValue)
    {
        update_value(position, std::forward<U>(newValue));
    }

    // If the user knows that the updated value has increased or decreased
    // compared to before, a usually more efficient algorithm can be used

    /// Like update, but assumes newValue has not decreased (w.r.t. compare()
    /// and the old value at position).
    /// Behavior is undefined when this precondition does not hold.
    template< typename U >
    void increase(const_iterator position, U&& newValue)
    {
        increase_value(position, std::forward<U>(newValue));
    }

    /// Like update, but assumes newValue has not increased (w.r.t. compare()
    /// and the old value at position).
    /// Behavior is undefined when this precondition does not hold.
    template< typename U >
    void decrease(const_iterator position, U&& newValue)
    {
        decrease_value(position, std::forward<U>(newValue));
    }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
    const_iterator cbegin() const { return d.c.cbegin(); }
    const_iterator cend() const { return d.c.cend(); }

    const_reference operator[] ( size_type n ) const { return at(n); }
    const_reference at( size_type n ) const { return d.c.at(n); }
    const_reference front() const { return d.c.front(); }
    const_reference back() const { return d.c.back(); }

    void clear() noexcept { d.c.clear(); }

    size_type capacity() const noexcept { return d.c.capacity(); }
    void reserve(size_type n) { d.c.reserve(n); }
    void shrink_to_fit() { d.c.shrink_to_fit(); }

    Compare compare() const { return d; }
    void set_compare(const Compare &compare)
    {
        static_cast<Compare&>(d) = compare;
        alg::make_heap(this);
    }

    container_type container() const { return d.c; }
    container_type take_container()
    {
        container_type tmp = std::move(d.c);
        d.c = {};
        return tmp;
    }

    Alloc get_allocator() const { return d.c.get_allocator(); }

private:
    // API needed by algorithm (besides compare() which is public)

    typedef typename container_type::iterator               iterator;

    iterator begin() { return d.c.begin(); }
    iterator end() { return d.c.end(); }

    void push_back(int) { d.c.push_back({}); }
    void pop_back() { d.c.pop_back(); }
    T &back() { return d.c.back(); }

    void remove_element(iterator, difference_type idx, const T& value)
    {
        PositionTracker::template remove(*this, value, idx);
    }

    void move_element(iterator first, difference_type from, difference_type to)
    {
        *(first + to) = std::move(*(first + from));
        PositionTracker::template move(*this, *(first + to), from, to);
    }

    template< typename U >
    void insert_element(iterator first, difference_type to, U&& value)
    {
        *(first + to) = std::forward<U>(value);
        PositionTracker::template insert(*this, *(first + to), to);
    }

    // Specialized algorithm for improved performance

    template< typename U >
    void update_value(const_iterator position, U&& newValue)
    {
        const iterator first = begin();
        const difference_type p = position - first;

        remove_element(first, p, *position);
        alg::adjust_heap(this, p, std::forward<U>(newValue), end() - first);
    }

    template< typename U >
    void increase_value(const_iterator position, U&& newValue)
    {
        const iterator first = begin();
        const difference_type p = position - first;

        remove_element(first, p, *position);
        auto pos = alg::up_heap(this, p, newValue);
        insert_element(first, pos, std::forward<U>(newValue));
    }

    template< typename U >
    void decrease_value(const_iterator position, U&& newValue)
    {
        const iterator first = begin();
        const difference_type p = position - first;

        remove_element(first, p, *position);
        alg::heapify(this, p, std::forward<U>(newValue));
    }

    // Data member

    struct Data : public Compare {
        Data() = default;

        template< typename S >
        Data(S&& container, const Compare& comp)
            : Compare(comp), c(std::forward<S>(container))
        {}

        template< typename S >
        Data(S&& container, const Compare& comp, const Alloc& alloc)
            : Compare(comp), c(std::forward<S>(container), alloc)
        {}

        Data(std::initializer_list<value_type> il)
            : c(il)
        {}

        container_type c;
    };

    Data d;
};


} // namespace binary_max_heap

#endif // BINARY_MAX_HEAP_H
