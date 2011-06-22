#include "weapon.h"
#include "types.h"
#include "SimpleIni.h"

static object::Weapon::Creator _weaponcreator;

using namespace object;

void Weapon::Info::Load(const char *p_Path, Weapon::Info *p_Info)
{
	CSimpleIniA ini;
	ini.LoadFile(p_Path);

	p_Info->Name	= ini.GetValue("Weapon", "name");
	p_Info->Dmg 	= ini.GetLongValue("Weapon", "damage");
	p_Info->Speed 	= ini.GetDoubleValue("Weapon", "speed");
	p_Info->Reload	= ini.GetDoubleValue("Weapon", "reload");
	p_Info->Ammo	= ini.GetLongValue("Weapon", "ammo");
	p_Info->Clip	= p_Info->Ammo;
	p_Info->Rate	= ini.GetDoubleValue("Weapon", "rate");
	p_Info->Explosive = ini.GetBoolValue("Weapon", "explosive");
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
Weapon::Weapon()
{
	m_phBody = NULL;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
Weapon::Weapon(const Weapon::Info& p_Info)
{
	pInfo = p_Info;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
GameObject::Status& Weapon::Update(float unused)
{
	myStatus.pPos = m_phBody->GetLocation();
    rot_dAngle = R2D(m_phBody->GetBodyDef().a);

	return myStatus;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Weapon::Initialize(void)
{

}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Weapon::Spawn(glm::vec3 p_Pos)
{
	delete m_phBody;
	m_phBody = NULL;

	m_phBody = new physics::Rectangle;
	m_phBody->BuildRect(15.0f, 5.0f, 0.01f, myStatus.pPos.x, myStatus.pPos.y);
	myStatus.pValue = GameObject::Status::Active;
	m_phBody->Spawn(p_Pos);
	m_phBody->SetShapeData(this);
	m_phBody->SetGroup(0x02);
	m_phBody->SetCollisionType(WEAPON);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Weapon::Impulse(glm::vec3 p_Imp)
{
	cpBodyApplyImpulse(&m_phBody->GetBodyDef(), cpv(p_Imp.x,p_Imp.y), cpv(1,1));
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Weapon::Impulse(glm::vec3 p_Imp, float p_x, float p_y)
{
	cpBodyApplyImpulse(&m_phBody->GetBodyDef(), cpv(p_Imp.x, p_Imp.y), cpv(p_x, p_y));
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
Bullet*	Weapon::Shoot(void)
{
	if(pInfo.Clip == 0)
		return NULL;

	Bullet *bullet = NULL;

	if(!pInfo.Explosive){
		bullet = new BulletPlain();
	}
	else{
		bullet = new Explosive();
	}

	bullet->pDamage = pInfo.Dmg;
	bullet->pSpeed = pInfo.Speed;

	pInfo.Clip--;

	return bullet;
}
