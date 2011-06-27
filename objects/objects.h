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

#ifndef GAME_OBJECTS_HEADER
#define GAME_OBJECTS_HEADER

#include"physics.h"
#include<map>
#include<cstdio>
#include<glm/glm.hpp>

#define MAPPOLYGON	0x00
#define BULLET		0x01
#define MERC		0x02
#define PLAYER		0x02
#define WEAPON		0x03
#define EXPLOSIVE	0x04
#define EXPLOSION	0x05

////////////////////////////////////////////////////////////
/// Most basic object in the game
////////////////////////////////////////////////////////////
class GameObject;
struct ObjectCreator
{
    virtual GameObject* Create() const =0;
};


class GameObject
{
public:
	////////////////////////////////////////////////////////////
	/// Object status
	////////////////////////////////////////////////////////////
	struct Status
    {
	public:
		////////////////////////////////////////////////////////////
		/// default ctor
		////////////////////////////////////////////////////////////
		Status(void):
			pDest(glm::vec3()),
			pPos(glm::vec3()),
			pVelocity(glm::vec3()),
			pValue(0),
			pAirborne(false),
			pDoubleJump(false),
			pLastUpdate(0.0f),
			pHealth(100.0f),
			u(0.0f),
			pShape(NULL),
			pGroundNormal(cpvzero),
			pGroundShapes(NULL)
		{

		}

		////////////////////////////////////////////////////////////
		/// copy ctor
		////////////////////////////////////////////////////////////
		Status(const Status &p_Copy):
			pDest(p_Copy.pDest),
			pPos(p_Copy.pPos),
			pValue(p_Copy.pValue),
			pVelocity(p_Copy.pVelocity),
			u(p_Copy.u),
			pShape(p_Copy.pShape),
			pGroundNormal(p_Copy.pGroundNormal),
			pGroundShapes(p_Copy.pGroundShapes),
			pAirborne(p_Copy.pAirborne),
			pDoubleJump(p_Copy.pDoubleJump),
			pLastUpdate(p_Copy.pLastUpdate),
			pHealth(p_Copy.pHealth)
		{
			//void
		}

	public:
		////////////////////////////////////////////////////////////
		/// Copy operator
		////////////////////////////////////////////////////////////
		Status& operator=(const Status& p_Copy)
		{
			pDest 		= p_Copy.pDest;
			pPos		= p_Copy.pPos;
			pVelocity	= p_Copy.pVelocity;
			pValue		= p_Copy.pValue;
			pAirborne	= p_Copy.pAirborne;
			pDoubleJump	= p_Copy.pDoubleJump;
			pLastUpdate	= p_Copy.pLastUpdate;
			pHealth		= p_Copy.pHealth;
			u			= p_Copy.u;
			pShape		= p_Copy.pShape;
			pGroundNormal	= p_Copy.pGroundNormal;
			pGroundShapes	= p_Copy.pGroundShapes;

			return *this;
		}

	public:
		////////////////////////////////////////////////////////////
		/// Status values
		////////////////////////////////////////////////////////////
        enum
        {
            Unknown = 0,
            Active,
            Sleep,
            Static,
            Dead
        };

	public:
		////////////////////////////////////////////////////////////
		/// Data
		////////////////////////////////////////////////////////////
		glm::vec3    	pDest; ///> destination
        glm::vec3    	pPos; ///> postion
		int         	pValue; ///> status value
		glm::vec3		pVelocity;
        bool 			pDoubleJump;
		bool 			pAirborne;
        float       	pLastUpdate; ///>last update
		float			pHealth;
        cpFloat 		u;
        cpShape 		*pShape;
        cpVect 			pGroundNormal;
        cpArray 		*pGroundShapes;
    };

public:
	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
	GameObject():
		rot_dAngle(0.0f),
		myStatus(Status())
	{
		//void
	}

	////////////////////////////////////////////////////////////
	/// copy ctor
	////////////////////////////////////////////////////////////
	GameObject(const GameObject &p_Copy) :
		rot_dAngle(p_Copy.rot_dAngle),
		myStatus(p_Copy.myStatus)
	{
		//void
	}

public:
	////////////////////////////////////////////////////////////
	///return a type of gameobject
   	////////////////////////////////////////////////////////////  
	static inline GameObject*   Create(int type)
    {
        return (*GameObject::CreatorsMap()[type]).Create();
    }

public:
	////////////////////////////////////////////////////////////
    ///> Update internal state, return status 
	////////////////////////////////////////////////////////////
	virtual GameObject::Status& Update(float dt) = 0;
    
	////////////////////////////////////////////////////////////	
	///> Initializer
	//////////////////////////////////////////////////////////// 
	virtual void Initialize(void) = 0;

	////////////////////////////////////////////////////////////
	// Spawn object
	////////////////////////////////////////////////////////////
    inline void Spawn(int x, int y) {Spawn(glm::vec3(x,y,0));}

	////////////////////////////////////////////////////////////
	/// Spawn object
	////////////////////////////////////////////////////////////	
	inline virtual void Spawn(glm::vec3 p_Pos)
    {
        myStatus.pPos = p_Pos;
        myStatus.pValue = Status::Unknown;
    }

	////////////////////////////////////////////////////////////
	/// Damage Object
	////////////////////////////////////////////////////////////
	virtual void Damage(const float	&p_Damage)
	{
		myStatus.pHealth -= p_Damage;
	}

	////////////////////////////////////////////////////////////
	/// Kill object
	////////////////////////////////////////////////////////////
	virtual void Kill(void)
	{
		//void
	}

public:
    ///Getters & Setters
    inline const GameObject::Status& GetStatus(void) const
    {
        return myStatus;
    }
    inline int& GetStatusValue(void)
    {
        return myStatus.pValue;
    }
    inline const glm::vec3& GetPosition() const
    {
        return myStatus.pPos;
    }

public:
    virtual void Move(glm::vec3 pos,float time) = 0;
	virtual void Impulse(glm::vec3 p_Imp, float p_x, float p_y) =0;
	
protected:
    static inline std::map<int,ObjectCreator*>& CreatorsMap()
    {
        static std::map<int,ObjectCreator*> CreatorsMap;
        return CreatorsMap;
    }

public:
    ////////////////////////////////////////////////////////////
    // Data
    ////////////////////////////////////////////////////////////
	float 				rot_dAngle; //rotation angle(degrees)
	GameObject::Status	myStatus;///< Internal object status
};

#endif //GAME_OBJECTS_HEADER

