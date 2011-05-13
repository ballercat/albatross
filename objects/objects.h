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
        enum
        {
            Unknown = 0,
            Active,
            Sleep,
            Static,
            Dead
        };
        glm::vec3    des; ///> destination
        glm::vec3    pos; ///> postion
        int         val; ///> status value
		glm::vec3	v; ///> velocity

        cpFloat u;
        cpShape *shape;
        cpVect groundNormal;
        cpArray *groundShapes;

        ///Default constructor
        Status(void)
        {
            val = Unknown;
        }

        bool Airborne;
        bool DoubleJump;
        float       lastup; ///>last update
        bool        sleep;
        float       SleepTimer;
    };

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
	inline void Spawn(glm::vec3 pos)
    {
        myStatus.pos = pos;
        myStatus.val = Status::Unknown;
    }

public:
    ///Getters & Setters
    inline const GameObject::Status& GetStatus(void) const
    {
        return myStatus;
    }
    inline int& GetStatusValue(void)
    {
        return myStatus.val;
    }
    inline float& GetSleepTimer(void)
    {
        return myStatus.SleepTimer;
    }
    inline void SetSleepTimer(float& t)
    {
        mySleepTimer = t;
    }
    inline const glm::vec3& GetPosition() const
    {
        return myStatus.pos;
    }

public:
    virtual void Move(glm::vec3 pos,float time) = 0;
	virtual void Impulse(glm::vec3 p_Imp, float p_x, float p_y) =0;
	
    inline int& CurrentAction(void)
    {
        return mAction;
    }

	float rot_dAngle; //rotation angle(degrees)
    bool Airborne;
    unsigned int id;
	GameObject::Status      myStatus;///< Internal object status
	
protected:
    static inline std::map<int,ObjectCreator*>& CreatorsMap()
    {
        static std::map<int,ObjectCreator*> CreatorsMap;
        return CreatorsMap;
    }
    ////////////////////////////////////////////////////////////
    // Member Data
    ////////////////////////////////////////////////////////////
    float                   mySleepTimer;       ///< Sleep in seconds
    int mAction;
};

#endif //GAME_OBJECTS_HEADER

