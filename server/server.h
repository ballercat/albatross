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

#ifndef GAME_SERVER_HEADER
#define GAME_SERVER_HEADER

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "types.h"
#include "physics.h"
#include "client.h"
#include "network.h"
#include <string>
#include <vector>


////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////
class Server
{
public:

    ////////////////////////////////////////////////////////////
    /// Execute the main loop of a server
    ////////////////////////////////////////////////////////////
    virtual int Run(void) = 0;

    inline const std::string& GetServerName(void) const
    {
        return mInfo.name;
    }
    inline const std::string& GetServerIP(void) const
    {
        return mInfo.ip;
    }

    struct ServerInfo
    {
        std::string     ip;
        std::string     name;
        std::string     owner;
        std::string     mapname;
    };

    message::Queue  mMessageQueue;
protected:
    ////////////////////////////////////////////////////////////
    //  Member data
    ////////////////////////////////////////////////////////////
    std::vector<Client*>    mClientList;
    ServerInfo              mInfo;
    physics::PhysicsSimulator*       mPhysicsPtr;
};

class LocalHost : public Server
{
public:
    /// This will spawn a local server
    /// , process the config file and create
    /// a super user character in it
    LocalHost(void);
    ~LocalHost();

    virtual int Run(void);
    int Execute(void);

private:
    Client*     mAdmin; //Super User
};
#endif //GAME_SERVER_HEADER
