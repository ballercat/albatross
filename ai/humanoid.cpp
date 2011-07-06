#include "humanoid.h"

using namespace ai;

MercAgent::MercAgent() :
mObject(NULL),
Target(NULL),
look(glm::vec3(0)),
Idle(&IdleState::Instance()),
Move(&MoveState::Instance())
{
	mCurrentState = Idle;
}

MercAgent::MercAgent(Player *p_Object) :
mObject(p_Object),
Target(NULL),
look(glm::vec3(0)),
Idle(&IdleState::Instance()),
Move(&MoveState::Instance())
{
	mCurrentState = Idle;
}

void MercAgent::Act(float &p_Time)
{
	look = glm::vec3(1);
	if(Target)
		look = Target->pIPos - mObject->pIPos;

	mCurrentState->Exec(this);

	mObject->Step(look, p_Time);
}
