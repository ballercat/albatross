#ifndef AIAGENT_HEADER_GUARD
#define AIAGENT_HEADER_GUARD

#include"messages.h"

namespace ai
{
	////////////////////////////////////////////////////////////
	/// Agent definition
	////////////////////////////////////////////////////////////
	class AgentDef{
	public:
		enum{
			NEUTRAL = 0,
			AGGRESSIVE,
			FAIR,
			TYPECOUNT
		};

		AgentDef() :
		behavior(NEUTRAL)
		{}

		AgentDef(const AgentDef &p_Copy):
		behavior(p_Copy.behavior)
		{}


	public:
		int	behavior;
	};

	class Agent;

	////////////////////////////////////////////////////////////
	/// Agent state
	////////////////////////////////////////////////////////////
	class State{
	public:
		virtual ~State() {}
		virtual void Enter(Agent*) = 0;
		virtual void Exec(Agent*) = 0;
		virtual void Exit(Agent*) = 0;
	};

	////////////////////////////////////////////////////////////
	/// Agent class
	////////////////////////////////////////////////////////////
	class Agent
	{
	public:
		virtual ~Agent(){}

		void ChangeState(State *p_NewState)
		{
			mCurrentState->Exit(this);

			mCurrentState = p_NewState;
			mCurrentState->Enter(this);
		}

	public:
		message::Queue	Messages;

	protected:
		State	*mCurrentState;
	};

}

#endif
