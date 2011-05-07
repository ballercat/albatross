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

#ifndef PLAYER_HEADER_GUARD
#define PLAYER_HEADER_GUARD

#include "merc.h"
#include "bullets.h"
#include "assets.h"
#include "weapon.h"
#include <vector>
#include <iostream>

class Player
{
public:
    ////////////////////////////////////////////////////////////
	/// Default ctor
	////////////////////////////////////////////////////////////			
	Player();

	////////////////////////////////////////////////////////////
	/// ctor with location and texture pointer
	////////////////////////////////////////////////////////////		
	Player(glm::vec3 p_Pos, GLuint* p_Texture);

public:
    void Step(glm::vec3& cursor, float& p_Time);

public:
    void Draw(float interpolate);

public:
    void Jet(void);
    void Left(void);
    void Right(void);
    void jumpBegin();
    void jumpEnd();
    Bullet* Shoot(glm::vec3& p_Dest);
    void Throw();
	void Pickup(object::Weapon weapon);
    void PickWeapon(object::Weapon::Info& p_Info, int p_ID);

private:
    void _updatePositions(glm::vec3& npos);

public:
	////////////////////////////////////////////////////////////
	/// Player Timing
	////////////////////////////////////////////////////////////
	
	//Action timer
	struct ATimer{
		ATimer() : Stamp(0.0f), Timer(0.0f) {}
		float Stamp; //timestamp
		float Timer;
	};

	//Timing struct
	struct Timing{
		float	Current; //current time
		ATimer	Jump;
		ATimer	Move;
		ATimer	Jet;
		ATimer	Shoot;

		inline float Diff(float& p_Timer)
		{
			return (Current - p_Timer);
		}
	};

	//Global Timing
	Timing	pTime;

public:
	glm::vec3	pWeaponPos;
	glm::vec3	pBarrelPos;
	glm::vec3	pAngle;
	glm::vec3	pPos;
	glm::vec3	pIPos;

	int				pWeaponID;
	object::Weapon	pWeapon;
	bool			pHasWeapon;

private:
	int			mFlip;
	glm::vec3	mVelocity;
	float		mAngleCursor;
	float		mAngleR;
	
	//flags
	struct ActState{
		
		enum ID{
			IDLE,
			RUNNING,
			JUMPING,
			JETTING,
			SHOOTING,
			STATECOUNT
		};
		
		ActState()
		{
			Current	= IDLE;
			mStateData[ActState::IDLE] 		= true;
			mStateData[ActState::RUNNING]	= false;
			mStateData[ActState::JUMPING]	= false;
			mStateData[ActState::JETTING]	= false;
			mStateData[ActState::SHOOTING]	= false;
		}
			
		inline int& operator()(void)
		{
			Current = ActState::IDLE;
			
			if(mStateData[ActState::RUNNING]){
				Current = ActState::RUNNING;
			}
			else if(mStateData[ActState::JETTING]){
				Current = ActState::JETTING;
			}
			
			mStateData[ActState::RUNNING] 	= false;
			mStateData[ActState::JETTING]	= false;
			
			Swap = Current;
			Current = ActState::IDLE;
			
			return Swap;
		}
		
		inline bool& operator[](int& p_ID)
		{
			return mStateData[p_ID];
		}
		
		inline bool& operator[](ActState::ID p_ID)
		{
			return mStateData[p_ID];
		}
		
		int		Current;
		int		Swap;
		bool	mStateData[ActState::STATECOUNT];
	};
	
	ActState	pAction;
	bool		mShoot;
	bool		mJumpState;
	bool		mRunning;

private:
	Sprite		mRunningSprite;
	Sprite		mIdleSprite;
	Sprite		mJetSprite;
	Sprite		mJetFlameSprite;

	Sprite*		mBody;
	Sprite*		mSparks;

private:
	Merc		MercObject;

	Sprite*     jetflame;
	std::vector<float>  jflame_alpha;
	std::vector<glm::vec3>  jflame_pos;
	size_t  jflame_count;
	float   jflame_eraser;
    float   sparktimer;
    float   sparkcounter;
	float 	flametimer;

/*
public:
    Sprite *torso;
    Sprite *legs;
    Sprite *weapon;
    Sprite *sparks;

    std::vector<Sprite*> rawsprite;

    //Physical data
    glm::vec3 wps;
    glm::vec3 barrel;
    glm::vec3 angle;
    glm::vec3 ps;
    glm::vec3 ipos;
    bool      running;

    //Weapon
    int     WeaponID;
    object::Weapon  *Weapon;
    bool        pHasWeapon;

private:
	Merc	MercObject;
    float   Time;
    float   flametimer;
    float   movetimer;
    float   jettimer;
    float   sparktimer;
    float   sparkcounter;

    //jumps
    float   jumptimer;
    float   jumptime;
    float   jumpcounter;
    float   jumpstate;

    int     flip;
    glm::vec3   mVelocity;
    float   angle2cursor;
    float   angleR;
    bool    shoot;

    Sprite*     jetflame;
    std::vector<float>  jflame_alpha;
    std::vector<glm::vec3>  jflame_pos;
    size_t  jflame_count;
    float   jflame_eraser;

    object::Weapon *m_Weapon;
*/
};

#endif //PLAYER_HEADER_GUARD
