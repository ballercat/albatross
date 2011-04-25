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

#include "server.h"

#include <cstdio>

LocalHost::LocalHost(void)
{
    mPhysicsPtr = &physics::Simulator::Instance();
    mInfo.ip = network::GetLocalIP();
    mInfo.name = "localhost";


}

LocalHost::~LocalHost(void)
{

}

int LocalHost::Run(void)
{
    sf::SocketTCP Listener;
    sf::IPAddress ClientAddress;
    sf::SocketTCP Client;
    char Buffer[256];
    size_t  Received;

    printf("Awaiting connections\n");
    Listener.Listen(2000);
    if(Listener.Accept(Client,&ClientAddress) != sf::Socket::Done)
    {
        printf("Connection accepted from: %s\n",ClientAddress.ToString().c_str());

        while(Client.Receive(Buffer,sizeof(Buffer), Received) != sf::Socket::Done)
        {

        }

    }
    return 0;
}

int LocalHost::Execute(void)
{
    if(mMessageQueue.Size())
    {
		int msgType = mMessageQueue.Read();
        switch(msgType)
        {
            case message::GMSG_NEWCLIENT:
            {/*
                message::msgNewClient* NewClient = (message::msgNewClient*)(msg);
                if(!NewClient->mClient)
                {
                    NewClient->mClient = new Client;
                    NewClient->mClient->mIP = NewClient->mIP;
                }

                if(NewClient->mClient->mIP == network::GetLocalIP())
                {
                    mAdmin = NewClient->mClient;
                }

                mClientList.push_back(NewClient->mClient);
				*/
                break;
            }
        }
        mMessageQueue.Pop();
    }
    return 0;
}
