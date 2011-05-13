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

#ifndef GAME_PHYSICS_HEADER
#define GAME_PHYSICS_HEADER

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include"generics.h"
#include<chipmunk/chipmunk.h>
#include<vector>
#include<glm/glm.hpp>
#include<cassert>

// TODO : Finish adding comments

////////////////////////////////////////////////////////////
/// Helper macros to declare collision handlers
////////////////////////////////////////////////////////////

namespace physics
{
////////////////////////////////////////////////////////////
/// Type Declarations
////////////////////////////////////////////////////////////
class PhysicsObject;
class PhysicsSimulator;
template<class T> class CircleMixin;
template<class T> class RectangleMixin;

typedef CircleMixin<PhysicsObject>      Circle;
typedef RectangleMixin<PhysicsObject>   Rectangle;
typedef GenericSingleton<PhysicsSimulator> Simulator;

////////////////////////////////////////////////////////////
/// Physics Simulator
////////////////////////////////////////////////////////////
class PhysicsSimulator
{
public:
    ////////////////////////////////////////////////////////////
    /// Default Constructor
    ///
    ////////////////////////////////////////////////////////////
    PhysicsSimulator();

    ////////////////////////////////////////////////////////////
    /// Destructor
    ////////////////////////////////////////////////////////////
    virtual ~PhysicsSimulator();

    ////////////////////////////////////////////////////////////
    /// Simulate the World
    /// \param TimeStep:    timestep
    ///
    ////////////////////////////////////////////////////////////
    void Step(float TimeStep);

    ////////////////////////////////////////////////////////////
    /// Get World Handle, usefull in binding new objects
    ///
    ////////////////////////////////////////////////////////////
    inline cpSpace* GetWorldHandle() const
    {
        return mySpace;
    };

public:
	////////////////////////////////////////////////////////////
	/// Add collision handler: 
	/// AddCollision(type1, type2); <-- clear all handlers
	////////////////////////////////////////////////////////////		
	static void AddCollision(	int p_One, 
								int p_Two, 
								void *p_Data=NULL,
								cpCollisionBeginFunc p_Begin=NULL, 
								cpCollisionPreSolveFunc p_Pre=NULL,
								cpCollisionPostSolveFunc p_Post=NULL,
								cpCollisionSeparateFunc p_Sep=NULL);

private:
    ////////////////////////////////////////////////////////////
    // Member Data
    ////////////////////////////////////////////////////////////
    cpSpace     *mySpace;
};

////////////////////////////////////////////////////////////
/// Static objects
////////////////////////////////////////////////////////////
struct Static
{
public:
	struct Object
	{
		public:
		////////////////////////////////////////////////////////////
		/// Constructor
		////////////////////////////////////////////////////////////
		Object(void) : m_phShape(NULL) 
		{
			pos = glm::vec3(0,0,0);
		}

		public:
		////////////////////////////////////////////////////////////
		/// Destructor
		////////////////////////////////////////////////////////////
		virtual ~Object(void)
		{
			assert(m_phShape!=NULL);
			cpSpace *space = Simulator::Instance().GetWorldHandle();
			cpSpaceRemoveShape(space,m_phShape);
			cpShapeFree(m_phShape);
		}
		
		public:
		inline void SetShapeData(void *p_Data)
		{
			assert(m_phShape!=NULL);

			m_phShape->data = p_Data;
		}

		inline void SetCollisionType(int p_Type)
		{
			assert(m_phShape!=NULL);

			m_phShape->collision_type = p_Type;
		}

		inline void SetGroup(int p_Group)
		{
			assert(m_phShape!=NULL);

			m_phShape->group = p_Group;
		}

		public:
		////////////////////////////////////////////////////////////
		/// Static circle
		////////////////////////////////////////////////////////////
		static Object* Circle(glm::vec3 p_Pos, float p_Radius)
		{
			cpSpace *space = Simulator::Instance().GetWorldHandle();
			cpBody *body = &space->staticBody;

			Object *shape = new Object;
			shape->m_phShape = cpSpaceAddShape(space, 
											   cpCircleShapeNew(body, 
																p_Radius, 
																cpv(p_Pos.x,p_Pos.y)));
			shape->m_phShape->e = 1.0f; shape->m_phShape->u = 1.0f;
			
			return shape;	
		}

		public:
		glm::vec3 pos;

		//Don't modify manualy
		cpShape *m_phShape;
	};
};

////////////////////////////////////////////////////////////
/// Physics wrapper for game objects
/// \note : It's of type 'NonCopyable' whatever uses it
///         should only ever need one of it either way.
////////////////////////////////////////////////////////////
class PhysicsObject : public NonCopyable
{
public:
    ////////////////////////////////////////////////////////////
    /// Default Constructor
    ///
    ////////////////////////////////////////////////////////////
    PhysicsObject(void);

    ////////////////////////////////////////////////////////////
    /// Destructor
    ////////////////////////////////////////////////////////////
    virtual ~PhysicsObject(void);

	////////////////////////////////////////////////////////////		
    // Kill object(remove from space)	
	////////////////////////////////////////////////////////////	 
	void Kill(void);
		

    ////////////////////////////////////////////////////////////
    /// Move to a location in a time period.
    /// In screen coordinates.
    /// \param x: x location
    /// \param y: y location
    /// \param t: time delta(float)
    ////////////////////////////////////////////////////////////
    inline void Move(int x, int y, float t)
    {
        cpBodySlew(myBodyDef,cpv(x,y),t);
    }
    inline void Move(glm::vec3 loc, float t)
    {
        cpBodySlew(myBodyDef,cpv(loc.x,loc.y),t);
    }
    ////////////////////////////////////////////////////////////
    /// Getter & Setter functions
    ////////////////////////////////////////////////////////////
    //GetLocation is missing and needs to be implemented in
    //the per shape template classes:
    //inline GenVec3d GetLocation() const;

    inline cpBody& GetBodyDef(void)
    {
        return *myBodyDef;
    }
    inline void SetLayers(unsigned int bitmask)
    {
        myShapeDef->layers = bitmask;
    }
    inline void SetGroup(unsigned int group)
    {
        myShapeDef->group = group;
    }
    inline void SetCollisionType(unsigned int tp)
    {
        myShapeDef->collision_type = tp;
    }
    inline void SetShapeData(void *data)
    {
        myShapeDef->data = data;
    }
    inline const float& GetMass() const
    {
        return myMass;
    }
    inline const glm::vec3 GetVelocity() const
    {
        cpVect v = cpBodyGetVel(myBodyDef);
        return glm::vec3(v.x,v.y,0.0f);
    }
    inline cpSpace* __dbg_get_space() const
    {
        return mWorldPtr;
    }
    inline cpShape* __dbg_get_shape() const
    {
        return myShapeDef;
    }
protected:
    ////////////////////////////////////////////////////////////
    // Member Data
    ////////////////////////////////////////////////////////////
    float       myMass;
    cpBody*     myBodyDef;
    cpShape*    myShapeDef;
    cpSpace*    mWorldPtr; ///Physics World pointer
	bool		m_Spawn; //is object added to the space
};

////////////////////////////////////////////////////////////
/// Mixin class to create a circle shaped physics object
////////////////////////////////////////////////////////////
template<class Obj>
struct CircleMixin : public Obj
{
public:
    ////////////////////////////////////////////////////////////
    /// Buid a circle
    /// \param radius: radius, float
    /// \param mass: mass, float
    /// \param x: x location(world)
    /// \param y: y location(world)
    ////////////////////////////////////////////////////////////
    void BuildCircle(float radius, float mass, int x, int y)
    { //I think having Build<shape type> name makes for easier to understand final code
        Obj::myMass = mass;//INFINITY;
        m_Radius = radius;
        Obj::myBodyDef = cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, radius,cpvzero));
        Obj::myBodyDef->p = cpv(x,y);
        //Obj::myBodyDef->v = cpv(0,0);
		cpSpaceAddBody(Obj::mWorldPtr, Obj::myBodyDef);
	}
	

	////////////////////////////////////////////////////////////
	// Spawn the object at (x,y)
	////////////////////////////////////////////////////////////			
	void Spawn(glm::vec3 p_Pos)
	{
		//Don't do anything if the body already exists
		if(Obj::m_Spawn)
			return;
		
		Obj::myBodyDef->p = cpv(p_Pos.x, p_Pos.y);
		Obj::myShapeDef = cpSpaceAddShape(Obj::mWorldPtr, cpCircleShapeNew(Obj::myBodyDef, m_Radius, cpvzero));
		Obj::myShapeDef->e = 0.0f; Obj::myShapeDef->u = 0.0f;
		myCircle = (cpCircleShape*)Obj::myShapeDef;

		Obj::m_Spawn = true;
	}

	inline const float& GetRadius() const
    {
        return m_Radius;
    }
    inline glm::vec3 GetLocation() const
    {
        cpVect loc = myCircle->tc;
        return glm::vec3(loc.x,loc.y, 0);
    }
protected:
    float m_Radius;
    cpCircleShape *myCircle;
};


////////////////////////////////////////////////////////////
/// Mixin class to create a Generic Rectangle Object
////////////////////////////////////////////////////////////
template<class Obj>
struct RectangleMixin : public Obj
{
public:
    inline void BuildRect(float w,float h,float mass,glm::vec3 loc)
    {
        BuildRect(w,h,mass,loc.x,loc.y);
    }
    void BuildRect(float w,float h,float mass,int x, int y)
    {
        m_Verts[0] = cpv(-w/2,-h/2);
        m_Verts[1] = cpv(-w/2,h/2);
        m_Verts[2] = cpv(w/2,h/2);
        m_Verts[3] = cpv(w/2,-h/2);
        
		Obj::myMass = mass;
        myWidth = w;
        myHeight = h;

        Obj::myBodyDef = cpBodyNew(mass, cpMomentForPoly(mass, 4, m_Verts, cpvzero));
        Obj::myBodyDef->p = cpv(x,y);
        Obj::myBodyDef->v = cpv(0,0);

        cpSpaceAddBody(Obj::mWorldPtr, Obj::myBodyDef); 
    }

	////////////////////////////////////////////////////////////
	// Spawn the object at (x,y)
	////////////////////////////////////////////////////////////			
	void Spawn(glm::vec3 p_Pos)
	{		
		Obj::myBodyDef->p = cpv(p_Pos.x, p_Pos.y);
		//Don't do anything if the body already exists
		if(Obj::m_Spawn)
			return;
		Obj::myShapeDef = cpSpaceAddShape(Obj::mWorldPtr, 
										  cpPolyShapeNew(Obj::myBodyDef, 4, m_Verts, cpvzero));
		Obj::myShapeDef->e = 0.0f; Obj::myShapeDef->u = 0.0f;

		Obj::m_Spawn = true;
	}

	inline glm::vec3 GetLocation() const
    {
        return glm::vec3(Obj::myBodyDef->p.x,Obj::myBodyDef->p.y,0);
    }
protected:
	cpVect m_Verts[4];
    float myWidth;
    float myHeight;
};

} //namespace physics

#endif // GAME_PHYSICS_HEADER
