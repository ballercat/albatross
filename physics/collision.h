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

#ifndef COLLISION_HEADER_GUARD
#define COLLISION_HEADER_GUARD

#include "physics.h"

#define BEGINVARS	cpArbiter *arb, cpSpace *space, void *data
#define PRESOLVEVARS BEGINVARS
#define SEPARATEVARS BEGINVARS

namespace collision
{

	//Begin collision handlers
	struct Begin
	{
		//Projectiles
		static cpBool BulletWorld(BEGINVARS);
		static cpBool ExplosiveWorld(BEGINVARS);
		static cpBool ExplosionObject(BEGINVARS);

		//Weapon
		static cpBool WeaponWorld(BEGINVARS);
		static cpBool WeaponPlayer(BEGINVARS);

		//Player
		static cpBool PlayerWorld(BEGINVARS);
	};
	
	struct PreSolve
	{
		//Player
		static cpBool PlayerWorld(PRESOLVEVARS);
	};

	struct Separate
	{	
		//Player
		static void PlayerWorld(SEPARATEVARS);
	};

} //namespace collision

#endif //COLLISION_HEADER_GUARD
