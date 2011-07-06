#ifndef HUMANOID_HEADER_GUARD
#define HUMANOID_HEADER_GUARD

#include"agent.h"
#include"player.h"
#include"generics.h"

namespace ai
{
	class MercIdleState;
	class MercMoveState;

	typedef GenericSingleton<MercIdleState>	IdleState;
	typedef GenericSingleton<MercMoveState> MoveState;

	////////////////////////////////////////////////////////////
	/// Generic humanoid agent
	////////////////////////////////////////////////////////////
	class MercAgent : public Agent
	{
	public:
		MercAgent();
		MercAgent(Player *p_Object);

	public:
		void Act(float &p_Time);

	public:
		Player	*mObject;
		Player 	*Target;
		glm::vec3	look;
		State *Idle;
		State *Move;
	};

	////////////////////////////////////////////////////////////
	/// States
	////////////////////////////////////////////////////////////
	class MercIdleState : public State
	{
		virtual void Enter(Agent *p_Merc)
		{

		}

		virtual void Exec(Agent *p_Merc)
		{
			static MercAgent *merc = NULL;
			merc = static_cast<MercAgent*>(p_Merc);
			if(merc->Target){
				if(!physics::Simulator::Instance().raycastAB(merc->mObject->pBarrelPos, merc->Target->pIPos))
					p_Merc->Messages.Push(message::GMSG_FIRE);
				else
					merc->ChangeState(merc->Move);
			}
		}

		virtual void Exit(Agent *p_Merc)
		{
			static MercAgent *merc = static_cast<MercAgent*>(p_Merc);
		}
	};

	class MercMoveState : public State
	{
		virtual void Enter(Agent *p_Merc)
		{

		}

		virtual void Exec(Agent *p_Merc)
		{
			static MercAgent *merc = static_cast<MercAgent*>(p_Merc);
			if(merc->Target){
				if(!physics::Simulator::Instance().raycastAB(merc->mObject->pBarrelPos, merc->Target->pIPos))
					merc->ChangeState(merc->Idle);
			}
		}

		virtual void Exit(Agent *p_Merc)
		{
			static MercAgent *merc = static_cast<MercAgent*>(p_Merc);
		}
	};
}


#endif
