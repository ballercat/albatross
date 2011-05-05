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

#ifndef MESSAGES_HEADER_GUARD
#define MESSAGES_HEADER_GUARD

#include"generics.h"
#include<queue>

namespace message
{

////////////////////////////////////////////////////////////
/// Message types
////////////////////////////////////////////////////////////

enum Code
{
    GMSG_DEFAULT = 0,
    GMSG_QUITGAME,
    GMSG_SPAWNOBJECT,
    GMSG_SPAWNBULLET,
    GMSG_DELETEOBJECT,
    GMSG_DELETEBULLET,
    GMSG_NEWCLIENT,
    GMSG_JUMPBEGIN,
	GMSG_JUMPFINISH,
    GMSG_MOVE_LEFT,
    GMSG_MOVE_RIGHT,
    GMSG_MOVE_JETS,
    GMSG_FIRE,
	GMSG_THROW,
	GMSG_PICKW0,
	GMSG_PICKW1,
    COUNT
};



////////////////////////////////////////////////////////////
/// Message Queue handler
////////////////////////////////////////////////////////////
class Queue
{
public:
    inline void Push(int msg)
    {
        mMessages.push(msg);
    }

    inline int& Read(void)
    {
        return mMessages.front();
    }

    void inline Pop(void)
    {
        mMessages.pop();
    }
    inline size_t Size(void) const
    {
        return mMessages.size();
    }

    void inline Clear(void)
    {
        mMessages.empty();
    }
private:
    std::queue<int>    mMessages;
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

} //namespace message






#endif //MESSAGES_HEADER_GUARD




