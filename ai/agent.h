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
