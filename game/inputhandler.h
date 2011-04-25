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


#ifndef InputHandlerMixin_HEADER_
#define InputHandlerMixin_HEADER_

#include<vector>
#include<map>
#include<utility>
#include<cmath>

#include "messages.h"

template<class Super>
class InputHandlerMixin : public Super
{
public:
	InputHandlerMixin(message::Queue *QueuePtr){

	    mMessages = QueuePtr;

		this->_key_handler[Super::Key::Q] = &InputHandlerMixin::_handle_K_q;
		this->_key_handler[Super::Key::Escape] = &InputHandlerMixin::_handle_K_esc;
		this->_key_handler[Super::Key::K] = &InputHandlerMixin::_handle_K_k;

		/* Controlls */
		this->_key_handler[Super::Key::W] = &InputHandlerMixin::_handle_K_w;

        //this->_static_key[Super::Key::W] = &InputHandlerMixin::_handle_K_w;
		this->_key_handler[Super::Key::D] = &InputHandlerMixin::_handle_K_d;
		this->_key_handler[Super::Key::A] = &InputHandlerMixin::_handle_K_a;

        Super::mState.StaticKey[Super::Key::W] = false;
        Super::mState.StaticKey[Super::Key::D] = false;
        Super::mState.StaticKey[Super::Key::A] = false;
	}
	virtual ~InputHandlerMixin(){}

	void HandleKeyboard(){
		//Handle Key input
		Keys code;
		typename std::map<Keys,void(InputHandlerMixin::*)(bool)>::iterator i;

		//Key events
		for(i = this->_key_handler.begin();i != this->_key_handler.end(); ++i){
			code = (*i).first;
			if( Super::IsKeyDown(code) ){
				(this->*_key_handler[code])(true);
			} else {
				(this->*_key_handler[code])(false);
			}
		}


		//static key handler
		for(i = this->_static_key.begin();i != this->_static_key.end(); ++i)
		{
		    code = (*i).first;
		    if( Super::mState.StaticKey[code] )
		    {
		        (this->*_static_key[code])(true);
		        Super::mState.StaticKey[code] = false;
		    }
		}
		/* Flawless Victory */


        //if(Super::Window->GetInput().IsKeyDown(sf::Key::W))
          //  mMessages->Push(message::GMSG_JUMPBEGIN);
	}

	void HandleMouse()
	{
		if(Super::IsMouseButtonDown(Super::Mouse::Middle))
		{

		}
		if(Super::IsMouseButtonDown(Super::Mouse::Left))
		{
            mMessages->Push(message::GMSG_FIRE);
		}
		if(Super::IsMouseButtonDown(Super::Mouse::Right))
		{
		    mMessages->Push(message::GMSG_MOVE_JETS);
		}
	}

protected:
	void _handle_K_q(bool down){
		if(down)
			mMessages->Push(message::GMSG_QUITGAME);
	}
	void _handle_K_esc(bool down){
	}
	void _handle_K_k(bool down){
	}
	void _handle_K_d(bool down){
		if(down)
			mMessages->Push(message::GMSG_MOVE_RIGHT);
	}
	void _handle_K_a(bool down){
		if(down)
			mMessages->Push(message::GMSG_MOVE_LEFT);
	}
	void _handle_K_w(bool down){
		if(down){
			mMessages->Push(message::GMSG_JUMPBEGIN);
		}
		else {
			mMessages->Push(message::GMSG_JUMPFINISH);
		}
	}

private:
    typedef typename Super::Key::Code    Keys;
	std::map<Keys, void(InputHandlerMixin::*)(bool)>	_key_handler;
	std::map<Keys, void(InputHandlerMixin::*)(bool)>    _static_key;

	message::Queue          *mMessages;
	sf::Clock   mTimer;
};


#endif
