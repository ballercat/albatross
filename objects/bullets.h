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

#ifndef BULLETS_HEADER_GUARD
#define BULLETS_HEADER_GUARD

#include "objects.h"
#include "physics.h"
#include "sprite.h"

////////////////////////////////////////////////////////////
/// Status
////////////////////////////////////////////////////////////
struct BulletStatus
{
public:
	////////////////////////////////////////////////////////////
	///Default constructor
	////////////////////////////////////////////////////////////
	BulletStatus(void)
	{
		val = Unknown;
	}

public:
	////////////////////////////////////////////////////////////
	/// Data
	////////////////////////////////////////////////////////////
	enum
	{
		Unknown = 0,
		Active,
		Inactive,
		Dead
	};
	glm::vec3   des; ///> destination
	glm::vec3   pos; ///> postion
	int         val; ///> status value
	int         parentid;
	float       lastup;
};

class Bullet
{
public:
	////////////////////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////////////////////
    virtual ~Bullet() {};

public:
	virtual const BulletStatus& Update(void) = 0;

public:
	////////////////////////////////////////////////////////////
	/// Init
	////////////////////////////////////////////////////////////
	virtual void Initialize(void) = 0;

public:
	////////////////////////////////////////////////////////////
	/// Getteers & setters
	////////////////////////////////////////////////////////////
    inline void SetStatusValue(int status)
    {
        myStatus.val = status;
    }

public:
	////////////////////////////////////////////////////////////
	/// Hit position of the bullet
	////////////////////////////////////////////////////////////
	struct HitSpot{
	public:
		////////////////////////////////////////////////////////////
		/// Constructor
		////////////////////////////////////////////////////////////
		HitSpot(){
			Pos = glm::vec3();
			Timestamp = 0.0f;
			Drawstamp = 0.0f;
		}

		HitSpot(glm::vec3& p_Pos, float p_Time, int& p_Type)
		{
			Pos = p_Pos;
			Timestamp = p_Time;
			Drawstamp = p_Time;
			Type = p_Type;
		}

		//Data
		glm::vec3 	Pos;
		float		Timestamp;
		float 		Drawstamp;
		int			Type;
	};

	//Hit position
	HitSpot Hit;

public:
	////////////////////////////////////////////////////////////
	/// Position Data
	////////////////////////////////////////////////////////////
    glm::vec3 pos;
    glm::vec3 lastpos;
    glm::vec3 des;
	glm::vec3 pVelocity;
	glm::vec3 startV;

public:
	////////////////////////////////////////////////////////////
	/// Bullet data
	////////////////////////////////////////////////////////////
	float 		pAngle;
    float		pDamage;
	float       pSpeed;
	int       	Status;
	int 		Type;
	int			pID;

protected:
    BulletStatus myStatus;
	float       myDuration;
    float       myDamage;
};

////////////////////////////////////////////////////////////
/// Plain non-explosive Bullet
////////////////////////////////////////////////////////////
class BulletPlain :
    public Bullet
{
public:
	////////////////////////////////////////////////////////////
	/// Constructor
	////////////////////////////////////////////////////////////
    BulletPlain();

public:
	////////////////////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////////////////////
    virtual ~BulletPlain(void);

public:
    virtual void Initialize(void);
    virtual const BulletStatus& Update(void);

private:
    physics::Rectangle  *myBody;
};

////////////////////////////////////////////////////////////
/// Explosive
////////////////////////////////////////////////////////////
class Explosive : public Bullet
{
public:
	////////////////////////////////////////////////////////////
	/// Constructor
	////////////////////////////////////////////////////////////
	Explosive();

	/*Explosive(const Explosive &p_Copy):
	explosion_pos(p_Copy.explosion_pos),
	m_phProjectile(p_Copy.m_phProjectile),
	m_phExplosion(p_Copy.m_phExplosion),
	m_isExploded(p_Copy.m_isExploded)
	{

	}

	Explosive& operator=(const Explosive &p_Copy){
		explosion_pos = p_Copy.explosion_pos;
		m_phProjectile = p_Copy.m_phProjectile;
		m_phExplosion = p_Copy.m_phExplosion;
		m_isExploded = p_Copy.m_isExploded;

		return *this;
	}*/

public:
	////////////////////////////////////////////////////////////	
	/// Destructor	
	////////////////////////////////////////////////////////////	
	~Explosive(void);

public:
	////////////////////////////////////////////////////////////		
	/// Initializer
	////////////////////////////////////////////////////////////		
	virtual void Initialize(void);

	////////////////////////////////////////////////////////////	
	/// Update function
	////////////////////////////////////////////////////////////	
	virtual const BulletStatus& Update(void);

	void Explode(void);

public:
	glm::vec3 explosion_pos;

public:
	////////////////////////////////////////////////////////////	
	/// Physics body	
	////////////////////////////////////////////////////////////
	physics::Rectangle *m_phProjectile; //Projectile object
	physics::Static::Object		*m_phExplosion; //Explosion

	//Explosion state
	bool m_isExploded;
};

#endif //BULLETS_HEADER_GUARD
