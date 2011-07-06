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

#include "humanoid.h"

using namespace ai;

MercAgent::MercAgent() :
mObject(NULL),
Target(NULL),
look(glm::vec3(0)),
Idle(&IdleState::Instance()),
Move(&MoveState::Instance())
{
	mCurrentState = Idle;
}

MercAgent::MercAgent(Player *p_Object) :
mObject(p_Object),
Target(NULL),
look(glm::vec3(0)),
Idle(&IdleState::Instance()),
Move(&MoveState::Instance())
{
	mCurrentState = Idle;
}

void MercAgent::Act(float &p_Time)
{
	look = glm::vec3(1);
	if(Target)
		look = Target->pIPos - mObject->pIPos;

	mCurrentState->Exec(this);

	mObject->Step(look, p_Time);
}
