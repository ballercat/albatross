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

#include "physics.h"
#include <stdio.h>
#include "bullets.h"

using namespace physics;

////////////////////////////////////////////////////////////
///Physics Sim impl
////////////////////////////////////////////////////////////
PhysicsSimulator::PhysicsSimulator(void) :
    mySpace(NULL)
{
    cpInitChipmunk();

    cpResetShapeIdCounter();
    mySpace = cpSpaceNew();
    mySpace->iterations = 10;
    mySpace->gravity = cpv(0,-300);

}

PhysicsSimulator::~PhysicsSimulator(void)
{
    cpSpaceFreeChildren(mySpace);
    cpSpaceFree(mySpace);
}

void PhysicsSimulator::Step(float TimeStep)
{
    cpSpaceStep(mySpace, TimeStep);
}

////////////////////////////////////////////////////////////
/// Add Begin Collision Handler
////////////////////////////////////////////////////////////
void 
PhysicsSimulator::BCollisionAdd(int p_One, int p_Two,
								cpCollisionBeginFunc p_Func,
								void *p_Data )
{
	cpSpaceAddCollisionHandler( Simulator::Instance().GetWorldHandle(),
								p_One, p_Two, p_Func,
								NULL, NULL, NULL, p_Data );
}

////////////////////////////////////////////////////////////
/// Physics Object Impl
////////////////////////////////////////////////////////////
PhysicsObject::PhysicsObject(void) :
    myMass(0.0f),
    myBodyDef(NULL),
    myShapeDef(NULL),
    mWorldPtr(GenericSingleton<PhysicsSimulator>::Instance().GetWorldHandle()),
	m_Spawn(false)
{

}

void PhysicsObject::Kill(void)
{
	cpSpaceRemoveShape(mWorldPtr, myShapeDef);
	cpShapeFree(myShapeDef);
	m_Spawn = false;
}
	
PhysicsObject::~PhysicsObject(void)
{
	if(m_Spawn)
    	cpSpaceRemoveShape(mWorldPtr, myShapeDef);
    cpSpaceRemoveBody(mWorldPtr, myBodyDef);
    if(m_Spawn)
		cpShapeFree(myShapeDef);
    cpBodyFree(myBodyDef);
}
