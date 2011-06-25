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
#include "timing.h"
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
	Player(GLuint* p_Texture);

	inline void Spawn(glm::vec3 p_Pos)
	{
		pWeaponPos		= p_Pos;
		pBarrelPos		= p_Pos;
		pPos			= p_Pos;
		pIPos			= p_Pos;
		MercObject.Spawn(p_Pos);
		pHealth = 100.0f;
		pJetCounter = 0;
		ResetTiming();
		pTime.Jet.Stamp = -2.0f;
		pWeapon.pInfo.Clip = 0;
		pTime.Reload.Stamp = pTime();
		pSpawned = true;
	}

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
	void Damage(float damage);
	void Kill(void);
	inline void SetJetAmmount(int p_Amt)
	{
		mJetAmmount = p_Amt;
		pJetCounter = p_Amt;
	}

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

		inline void Reset(void)
		{
			Stamp	= 0.0f;
			Timer	= 0.0f;
		}
	};

	//Timing struct
	struct Timing{
		float	Current; //current time
		ATimer	Jump;
		ATimer	Move;
		ATimer	Jet;
		ATimer	Flame;
		ATimer	Shoot;
		ATimer	Reload;
		sf::Clock *pTIMER;

		Timing()
		{
			pTIMER	= &timing::GlobalTime::Instance();
		}

		inline float Diff(float& p_Timer)
		{
			return (Current - p_Timer);
		}

		inline float operator()(void)
		{
			return pTIMER->GetElapsedTime();
		}
	};

	//Global Timing
	Timing	pTime;

public:

	inline void ResetTiming(void)
	{
		pTime.Current = pTime.pTIMER->GetElapsedTime();
		pTime.Flame.Reset();
		pTime.Jet.Reset();
		pTime.Shoot.Reset();
		pTime.Move.Reset();
		pTime.Jump.Reset();
		pTime.Reload.Reset();
	}

public:
	glm::vec3	pWeaponPos; //weapon position
	glm::vec3	pBarrelPos;	//gun barrel position
	glm::vec3	pAngle;		//angle to the cursor
	glm::vec3	pPos;		//physical position
	glm::vec3	pIPos;		//interpolated position
	bool		pSpawned;
	float		pHealth;
	int			pJetCounter;

public:
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
			DEAD,
			STATECOUNT
		};

		////////////////////////////////////////////////////////////
		/// default ctor
		////////////////////////////////////////////////////////////
		ActState()
		{
			Current	= IDLE;
			mStateData[ActState::IDLE] 		= true;
			mStateData[ActState::RUNNING]	= false;
			mStateData[ActState::JUMPING]	= false;
			mStateData[ActState::JETTING]	= false;
			mStateData[ActState::SHOOTING]	= false;
		}

		////////////////////////////////////////////////////////////
		/// Active state
		////////////////////////////////////////////////////////////
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

		////////////////////////////////////////////////////////////
		/// Acess
		////////////////////////////////////////////////////////////
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

	Sprite		mFlameSprite[10];
	struct	FlameData{
		FlameData()
		{
			Alpha 	= 0.0f;
			Pos		= glm::vec3(0.f,0.f,0.f);
		}

		FlameData(float p_Alpha, glm::vec3 p_Pos)
		{
			Alpha = p_Alpha;
			Pos	  = p_Pos;
		}

		inline FlameData& operator=(const FlameData& p_Copy)
		{
			Alpha = p_Copy.Alpha;
			Pos	  = p_Copy.Pos;

			return *this;
		}

		inline void Update(float p_AlphaDelta, float p_PosDelta)
		{
			Alpha 	-= p_AlphaDelta;
			Pos.y	-= p_PosDelta;
		}

		float		Alpha;
		glm::vec3	Pos;
	};

	int		mJetAmmount;
	std::vector<FlameData>	mJetFlames;
	size_t	mFC; //flame counter

    float   sparktimer;
    float   sparkcounter;


};

#endif //PLAYER_HEADER_GUARD
