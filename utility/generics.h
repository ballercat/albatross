/*
    ********************************************
    *   Albatross - A 2D multiplayer shooter   *
    ********************************************
    Copyright (C) 2011  Arthur Buldauskas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef GENERICS_HEADER_GUARD
#define GENERICS_HEADER_GUARD

#include<memory>
#include<list>
#include<cstdio>
#include"assert.h"

class NonCopyable
{
public:
    NonCopyable(){}
    virtual ~NonCopyable(){}
private:
    NonCopyable(const NonCopyable& copy);
    NonCopyable& operator =(const NonCopyable& copy);
};

template<class SingletonClass>
class GenericSingleton : public SingletonClass
{
public:
        /* Instance */
        static inline SingletonClass & Instance()
        {
            if(myInstance.get() == NULL)
            {
                myInstance = std::auto_ptr<GenericSingleton<SingletonClass> >(new GenericSingleton);
                assert(myInstance.get() != NULL);
            }

            return *myInstance;
        }
protected:
    static std::auto_ptr<GenericSingleton<SingletonClass> > myInstance;
};

template<class SingletonClass>
std::auto_ptr<GenericSingleton<SingletonClass> > GenericSingleton<SingletonClass>::myInstance;

//A simple wrapper for std::list
template<class T>
struct GenericHeap
{
public:
    typedef typename std::list<T*>::iterator Iter;
    inline Iter& Spot(void) { return _iterator; }
    inline Iter& Begin(void)
    {
        _iterator = liveList.begin();
        return _iterator;
    }
    inline const Iter Fetch(void)
    {
        return ++_iterator;
    }
    inline const Iter End(void)
    {
        return liveList.end();
    }
    inline void Add(T* data)
    {
        liveList.push_back(data);
    }
    inline void Delete(void)
    {
        deadList.push_back(*_iterator);
    }
    inline T* Get(void)
    {
        return (*_iterator);
    }
    void Flush(void)
    {
        if(deadList.size())
        {
            Iter    diter;
            diter = deadList.begin();
            for(;diter != deadList.end();++diter)
            {
                liveList.remove(*diter);
                delete *diter;
                *diter = NULL;
            }

            deadList.clear();
        }
        _iterator = liveList.begin();
    }

    inline const size_t& Size(void) { return liveList.size(); }

    Iter            _iterator;
    std::list<T*>   liveList;
    std::list<T*>   deadList;
};

#endif //GENERICS_HEADER_GUARD
