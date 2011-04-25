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


#ifndef GAME_GAMEDEF_HEADER
#define GAME_GAMEDEF_HEADER

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include "generics.h"


class GameDefinition;
typedef GenericSingleton<GameDefinition>   GameSingleton;
////////////////////////////////////////////////////////////
/// Game Definition
///
////////////////////////////////////////////////////////////
struct GameDefinition
{
    ////////////////////////////////////////////////////////////
    /// Game Globals
    ////////////////////////////////////////////////////////////
    sf::RenderWindow    *App;      ///< SFML app window
    sf::Clock           Clock;     ///< Clock/Timer
    int                 State;     ///< Game State

}; //GameDefinition

#endif //GAME_GAMEDEF_HEADER
