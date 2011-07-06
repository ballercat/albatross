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
#include"collision.h"
#include<iostream>

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

////////////////////////////////////////////////////////////
/// Destructor
////////////////////////////////////////////////////////////
PhysicsSimulator::~PhysicsSimulator(void)
{
    cpSpaceFree(mySpace);
}

////////////////////////////////////////////////////////////
/// Step world by dt seconds
////////////////////////////////////////////////////////////
void PhysicsSimulator::Step(float TimeStep)
{
    cpSpaceStep(mySpace, TimeStep);
}

////////////////////////////////////////////////////////////
/// Add Collision Handlers
////////////////////////////////////////////////////////////
void PhysicsSimulator::AddCollision(int p_One, 
									int p_Two, 
									void *p_Data,
									cpCollisionBeginFunc p_Begin, 
									cpCollisionPreSolveFunc p_Pre,
									cpCollisionPostSolveFunc p_Post,
									cpCollisionSeparateFunc p_Sep)
{
	cpSpace *space = Simulator::Instance().GetWorldHandle();
			
	cpSpaceAddCollisionHandler(	space,p_One, p_Two, 
								p_Begin, p_Pre, p_Post, p_Sep, p_Data);
}

////////////////////////////////////////////////////////////
/// Add a static shape
////////////////////////////////////////////////////////////
int PhysicsSimulator::addStaticSegmentShape(glm::vec3& p_Vert0, glm::vec3& p_Vert1, int p_Type)
{
	cpShape* shape = cpSpaceAddShape(mySpace, cpSegmentShapeNew(mySpace->staticBody,
											cpv(p_Vert0.x,p_Vert0.y),
											cpv(p_Vert1.x,p_Vert1.y), 0.0f));

	if(!shape)
		return -1;

	shape->e = 1.0f;
	shape->u = 1.0f;
	shape->collision_type = p_Type;

	int shapeID = m_staticShapes.size();

	m_staticShapes.push_back(shape);

	return shapeID;
}

////////////////////////////////////////////////////////////
/// Remove a static shape
////////////////////////////////////////////////////////////
void PhysicsSimulator::remStaticShape(int p_ShapeID)
{
	//TODO: Something here would be nice...
}

////////////////////////////////////////////////////////////
/// Remove all static shapes
////////////////////////////////////////////////////////////
void PhysicsSimulator::remAllStaticShapes()
{
	std::list<cpShape*>::iterator	iter;
	cpShape *remShape = NULL;

	for(iter = m_staticShapes.begin(); iter != m_staticShapes.end(); ++iter){
		remShape = *(iter);

		cpSpaceRemoveShape(mySpace, remShape);
		cpShapeFree(remShape);
	}

	m_staticShapes.clear();
}

////////////////////////////////////////////////////////////
/// Simple raycast
////////////////////////////////////////////////////////////
bool PhysicsSimulator::raycastAB(glm::vec3 &p_A, glm::vec3 &p_B)
{
	cpVect a = cpv(p_A.x, p_A.y);
	cpVect b = cpv(p_B.x, p_B.y);

	cpSegmentQueryInfo info;

	if(cpSpaceSegmentQueryFirst(mySpace, a, b, CP_ALL_LAYERS, 1, &info))
		return true;

	return false;
}

////////////////////////////////////////////////////////////
/// Physics Object ctor
////////////////////////////////////////////////////////////
PhysicsObject::PhysicsObject(void) :
    myMass(0.0f),
    myBodyDef(NULL),
    myShapeDef(NULL),
    mWorldPtr(GenericSingleton<PhysicsSimulator>::Instance().GetWorldHandle()),
	mVelocityFunction(NULL),
	m_Spawn(false),
	mShapeData(NULL),
	mType(-1),
	mGroup(-1)
{

}

////////////////////////////////////////////////////////////
/// Kill object(remove shape from world
////////////////////////////////////////////////////////////
void PhysicsObject::Kill(void)
{
	cpSpaceRemoveShape(mWorldPtr, myShapeDef);
	cpShapeFree(myShapeDef);
	cpSpaceRemoveBody(mWorldPtr, myBodyDef);
	cpBodyFree(myBodyDef);

	m_Spawn = false;
}

////////////////////////////////////////////////////////////
/// dtor
////////////////////////////////////////////////////////////
PhysicsObject::~PhysicsObject(void)
{
	cpSpaceRemoveBody(mWorldPtr, myBodyDef);
	cpSpaceRemoveShape(mWorldPtr, myShapeDef);
	cpBodyFree(myBodyDef);
	cpShapeFree(myShapeDef);
	myBodyDef = NULL;
	myShapeDef = NULL;
}
