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
Weapon::Weapon(const char *fpath)
{
	CSimpleIniA config;
	config.LoadFile(fpath);

	pDamage = config.GetLongValue("Weapon", "damage");
	pSpeed = config.GetLongValue("Weapon", "speed");
	pRate = config.GetLongValue("Weapon", "rate");
	pIsExplosive = config.GetBoolValue("Weapon", "explosive");

	m_phBody = NULL;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
GameObject::Status& Weapon::Update(float unused)
{
	myStatus.pos = m_phBody->GetLocation();
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
	m_phBody->BuildRect(15.0f, 5.0f, 0.01f, myStatus.pos.x, myStatus.pos.y);
	myStatus.val = GameObject::Status::Active;
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
	Bullet *bullet = NULL;
	if(!pIsExplosive){
		bullet = new BulletPlain();
	}
	else{
		bullet = new Explosive();
	}

	bullet->pDamage = pDamage;
	bullet->pSpeed = pSpeed;


	return bullet;
}
