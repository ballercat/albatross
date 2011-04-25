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


#ifndef _INPUT_HEADER_
#define _INPUT_HEADER_

#include "types.h"
#include "gamedef.h"
#include<queue>
#include<vector>
#include<algorithm>
#include<glm/glm.hpp>

//Unlicke the sf events these are a level higher
struct Event{
    //event type
    unsigned type;
    //events can block/filter other events
    bool block;
    bool blockall;
    //blocks are stacked by level
    unsigned level;
    //event struct pointer
    void *data;
};

struct EventQueue{
public:
    EventQueue(){
        _block[0] = false;
        _block[1] = false;
        _block[2] = false;
    }
    ~EventQueue(){
        if(!_events.empty())
        {
            std::list<Event*>::iterator i;
            for(i = _events.begin();i!=_events.end();i++)
                delete *i;
        }
    }
    virtual void push(Event* ev)
    {
        if(!ev){
            //TODO raise an error....
            return;
        }
        //some sanity checks before pushing to events
        if(ev->level> 2){
            //raise an error
            return;
        }
        else {
            //this event level is being blocked
            if(_block[ev->level])
                return;
        }

        if(ev->blockall && ev->level == 2)
            _setblock(3, true);
        else if(ev->block)
            _setblock(ev->level, true);


        //Done! Push event to the queue
        _events.push_back(ev);
    }
    virtual Event* read()
    {
        return _events.back();
    }
    virtual void pop()
    {
        Event *_ev = _events.front();
        if(_ev->block)
            _setblock(_ev->level, false);
        else if(_ev->blockall && _ev->level == 2)
            _setblock(3, false);

        std::list<Event*>::iterator i;
        _events.pop_back();

        for(i = _events.begin();i != _events.end();i++)
        {
            _ev = *i;
            if(_ev->blockall && _ev->level == 2)
                _setblock(3, true);
            else if(_ev->block)
                _setblock(_ev->level, true);
        }
    }
    inline size_t size()
    {
        return _events.size();
    }
protected:
    inline void _setblock(size_t lvl,bool state)
    {
        //level 3 doesnt actually exist
        //this is a blockall lvl 2
        if(lvl == 3){
            _block[0] = state;
            _block[1] = state;
            _block[2] = state;
        }
        else if(lvl == 2){
            _block[0] = state;
            _block[1] = state;
        }
        else if(lvl == 1){
            _block[0] = state;
        }
    }
    std::list<Event*> _events;
    bool _block[3];
};

class Input
{
    public:
        Input(void)
        {
            //void
            mState.mouse.lastup = mState.mouse.timer.GetElapsedTime();
        }

        /// Copied from SFML's Event.hpp
        struct Key{
            enum Code
            {
                A = 'a',
                B = 'b',
                C = 'c',
                D = 'd',
                E = 'e',
                F = 'f',
                G = 'g',
                H = 'h',
                I = 'i',
                J = 'j',
                K = 'k',
                L = 'l',
                M = 'm',
                N = 'n',
                O = 'o',
                P = 'p',
                Q = 'q',
                R = 'r',
                S = 's',
                T = 't',
                U = 'u',
                V = 'v',
                W = 'w',
                X = 'x',
                Y = 'y',
                Z = 'z',
                Num0 = '0',
                Num1 = '1',
                Num2 = '2',
                Num3 = '3',
                Num4 = '4',
                Num5 = '5',
                Num6 = '6',
                Num7 = '7',
                Num8 = '8',
                Num9 = '9',
                Escape = 256,
                LControl,
                LShift,
                LAlt,
                LSystem,      ///< OS specific key (left side) : windows (Win and Linux), apple (MacOS), ...
                RControl,
                RShift,
                RAlt,
                RSystem,      ///< OS specific key (right side) : windows (Win and Linux), apple (MacOS), ...
                Menu,
                LBracket,     ///< [
                RBracket,     ///< ]
                SemiColon,    ///< ;
                Comma,        ///< ,
                Period,       ///< .
                Quote,        ///< '
                Slash,        ///< /
                BackSlash,
                Tilde,        ///< ~
                Equal,        ///< =
                Dash,         ///< -
                Space,
                Return,
                Back,
                Tab,
                PageUp,
                PageDown,
                End,
                Home,
                Insert,
                Delete,
                Add,          ///< +
                Subtract,     ///< -
                Multiply,     ///< *
                Divide,       ///< /
                Left,         ///< Left arrow
                Right,        ///< Right arrow
                Up,           ///< Up arrow
                Down,         ///< Down arrow
                Numpad0,
                Numpad1,
                Numpad2,
                Numpad3,
                Numpad4,
                Numpad5,
                Numpad6,
                Numpad7,
                Numpad8,
                Numpad9,
                F1,
                F2,
                F3,
                F4,
                F5,
                F6,
                F7,
                F8,
                F9,
                F10,
                F11,
                F12,
                F13,
                F14,
                F15,
                Pause,

                Count // Keep last -- total number of keyboard keys
            };
        };

        /// Mouse definitions
        struct Mouse
        {
            Mouse() : buttondown(false) {}

            glm::vec3 pos;
            glm::vec3 diff;
            int wheel;
            bool buttondown;
            unsigned left,right,middle;
            sf::Clock timer;
            float lastup;
            enum Button
            {
                Left = sf::Mouse::Left,
                Right = sf::Mouse::Right,
                Middle = sf::Mouse::Middle
            };

        };

        /// Input state
        struct State
        {
            State() : keysdown(false) {}
            std::queue<int> releasedkey;
            Mouse   mouse;
            bool    keysdown;
            bool    StaticKey[Key::Count];
        };

        /// Wrapper
        inline bool IsMouseButtonDown(Mouse::Button button) const
        {
            return Window->GetInput().IsMouseButtonDown(sf::Mouse::Button(button));
        }

        /// Wrapper
        inline bool IsKeyDown(Key::Code key) const
        {
            return Window->GetInput().IsKeyDown(sf::Key::Code(key));
        }

        /// Game Input state
        inline const State&   InputState(void) const
        {
            return mState;
        }

        inline const sf::Input& GetInput() const
        {
            return Window->GetInput();
        }
        /// Call once/frame
        int Parse(){
            sf::Event event;
            mState.mouse.buttondown = false;
            mState.mouse.wheel = 0;
            mState.keysdown = false;

            while(Window->GetEvent(event)){
                switch(event.Type){
                case sf::Event::Closed:
                    return -1;
                case sf::Event::KeyPressed:
                    mState.keysdown = true;
                    mState.StaticKey[event.Key.Code] = true;
                    break;
                case sf::Event::MouseMoved:
                    mState.mouse.pos.x = event.MouseMove.X;
                    mState.mouse.pos.y = event.MouseMove.Y;
                    break;
                case sf::Event::MouseButtonPressed:
                {
                    float t = mState.mouse.timer.GetElapsedTime();
                    if(t - mState.mouse.lastup > 0.25f){
                        if(event.MouseButton.Button == sf::Mouse::Left)
                            mState.mouse.left = true;
                        if(event.MouseButton.Button == sf::Mouse::Right)
                            mState.mouse.right = true;
                        if(event.MouseButton.Button == sf::Mouse::Middle)
                            mState.mouse.middle = true;
                        mState.mouse.lastup = t;
                    }

                    mState.mouse.buttondown = true;
                    mState.mouse.pos.x = event.MouseButton.X;
                    mState.mouse.pos.y = event.MouseButton.Y;
                    break;
                }
                case sf::Event::MouseButtonReleased:
                    mState.mouse.buttondown = false;
                    if(event.MouseButton.Button == sf::Mouse::Left)
                        mState.mouse.left = false;
                    if(event.MouseButton.Button == sf::Mouse::Right)
                        mState.mouse.right = false;
                    if(event.MouseButton.Button == sf::Mouse::Middle)
                        mState.mouse.middle = false;
                    break;
                case sf::Event::Resized:
                case sf::Event::LostFocus:
                case sf::Event::GainedFocus:
                case sf::Event::TextEntered:
                    break;
                case sf::Event::KeyReleased:
                    mState.StaticKey[event.Key.Code] = false;
                    break;
                case sf::Event::MouseWheelMoved:
                    mState.mouse.wheel = event.MouseWheel.Delta;
                    break;
                case sf::Event::MouseEntered:
                case sf::Event::MouseLeft:
                case sf::Event::JoyButtonPressed:
                case sf::Event::JoyButtonReleased:
                case sf::Event::JoyMoved:
                case sf::Event::Count:
                    break;
                }
            }

            return 0;
        }


    sf::RenderWindow *Window;
    State           mState;

};

#endif
