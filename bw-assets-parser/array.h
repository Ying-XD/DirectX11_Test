// Slice-able array
// Author:   If
// Birthday: 20140620
// License:  public domain

#ifndef CPPUTILS_ARRAY_H_INCLUDED
#define CPPUTILS_ARRAY_H_INCLUDED

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <iterator>
#include <algorithm>

template<typename T> 
class Array {
public:
    // types
    typedef T                                     value_type;            
    typedef T*                                    iterator;              
    typedef const T*                              const_iterator;        
    typedef std::reverse_iterator<iterator>       reverse_iterator;      
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef T&                                    reference;             
    typedef const T&                              const_reference;       
    typedef std::size_t                           size_type;             
    typedef std::ptrdiff_t                        difference_type;       

public:
    Array():memory_(nullptr) {}
    Array(size_t size):memory_(nullptr)
    {
        memory_ = new MemBlock;
        memory_->data = new T[size];
        memory_->size = size;
        memory_->refcnt = 1;
        memory_->parent = nullptr;
    }
    Array(const_iterator begin, const_iterator end):memory_(nullptr)
    {
        assign(begin, end);
    }
    /*
    Array(std::initializer_list<T> const& li):memory_(nullptr)
    {
        assign(li.begin(), li.end());
    }
    */
    Array(Array const& rhs):memory_(rhs.memory_) // shallow copy by default
    {
        if( memory_ )
            MemBlock::incRef(memory_);
    }
    Array& operator=(Array const& rhs) // shallow copy by default
    {
        memory_ = rhs_.memory_;
        if( memory_ )
            MemBlock::incRef(memory_);
    }
    virtual ~Array()
    {
        MemBlock::decRef(memory_);
    }

public:
    template <class Iterator>
    void assign(Iterator begin, Iterator end)
    {
        if(memory_) {
            MemBlock::decRef(memory_);
        }
        memory_ = new MemBlock;
        memory_->data = new T[end-begin];
        memory_->size = end-begin;
        memory_->refcnt = 1;
        memory_->parent = nullptr;
        std::copy(begin, end, memory_->data);
    }

    iterator                begin()        { return memory_->data; }
    const_iterator          begin()  const { return memory_->data; }
    iterator                end()          { return memory_->data+memory_->size; }
    const_iterator          end()    const { return memory_->data+memory_->size; }
    reverse_iterator        rbegin()       { return reverse_iterator(end()); }
    const_reverse_iterator  rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator        rend()         { return reverse_iterator(begin()); }
    const_reverse_iterator  rend()   const { return const_reverse_iterator(begin()); }

    bool                    empty()  const { return memory_ == nullptr || memory_->size == 0; }
    size_t                  size()   const { if(empty()) return 0;  return memory_->size; }
    void*                   data()         { if(memory_ == nullptr) return nullptr; return memory_->data; }
    void const *            data()   const { if(memory_ == nullptr) return nullptr; return memory_->data; }

    reference operator[](size_t idx) {
        assert(idx < size());
        return memory_->data[idx];
    }
    const_reference operator[](size_t idx) const {
        assert(idx < size());
        return memory_->data[idx];
    }
    reference front() {
        return this->operator[](0);
    }
    const_reference front() const {
        return this->operator[](0);
    }
    reference back() {
        return this->operator[](size()-1);
    }
    const_reference back() const {
        return this->operator[](size()-1);
    }

    void swap(Array &rhs) {
        std::swap(this->memory_, rhs_->memory_);
    }

    // no resize(..) because arr.swap(Array(newSize)) can make side effects more clear to see

public:
    void wrap(T* begin, size_t size)
    {
        if( memory_ != nullptr )
            MemBlock::decRef( memory_ );
        memory_ = new MemBlock;
        memory_->data = begin;
        memory_->size = size;
        memory_->refcnt = 0;
        memory_->parent = nullptr;
    }

    Array slice(size_t start, size_t length)
    {
        assert(memory_ != nullptr);
        assert(start < memory_->size);
        assert(length + start <= memory_->size);

        Array child;
        MemBlock::incRef(memory_);
        child.memory_ = new MemBlock;
        child.memory_->data = memory_->data + start;
        child.memory_->size = length;
        child.memory_->refcnt = memory_->refcnt==0 ? 0 : 1;
        child.memory_->parent = memory_;

        return child;
    }

private:
    struct MemBlock {
        T*        data;
        size_t    size;
        size_t    refcnt; // number of slices + 1 or 0 which means borrowed memory we shouldn't care
        MemBlock *parent; // not nullptr if this is a slice

        static void incRef(MemBlock *& this_) { if (this_->refcnt != 0) { ++this_->refcnt; } }
        static void decRef(MemBlock *& this_)
        {
            if (this_ == nullptr)
                return;
            if (this_->refcnt != 0) {
                if (--this_->refcnt == 0) {
                    if(this_->parent)
                        decRef(this_->parent);
                    else
                        delete this_->data;
                    delete this_;
                    this_ = nullptr;
                }
            } else { // borrowed
                delete this_;
                this_ = nullptr;
            }
        }
    };
    MemBlock     *memory_;
};

template <class T>
inline bool operator==(Array<T> const& lhs, Array<T> const& rhs)
{
    if( lhs.empty() && rhs.empty() )
        return true;
    else if ( lhs.size() != rhs.size() )
        return false;
    for(Array<T>::const_iterator i1 = lhs.begin(), i2 = rhs.begin();
        i1 != lhs.end() && i2 != rhs.end();
        ++i1, ++i2)
        if( *i1 != *i2 )
            return false;
    return true;
}

template <class T>
typename Array<T>::iterator begin(Array<T>& arr) {
    return arr.begin();
}

template <class T>
typename Array<T>::iterator end(Array<T>& arr) {
    return arr.end();
}

template <class T>
typename Array<T>::const_iterator begin(Array<T> const& arr) {
    return arr.begin();
}

template <class T>
typename Array<T>::const_iterator end(Array<T> const& arr) {
    return arr.end();
}

#endif

