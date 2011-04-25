#include "player.h"

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
template <class T>
Player<T>::Player(glm::vec3 loc,GLuint *Texture)
{
	T::Spawn(loc);
	T::Initialize();
	T::id = 2;

	rawsprite.push_back(mercsprite::Make::Still(Texture[PLAYER_TORSO]));
	rawsprite.push_back(mercsprite::Make::Run(Texture[PLAYER_LEGS]));
	rawsprite.push_back(mercsprite::Make::Jets(Texture[PLAYER_JETS]));
	rawsprite.push_back(ak47sprite::Make::AK47(Texture[AK47]));

	jetflame = mercsprite::Make::JetFlame(Texture[JET_FLAME]);

	torso = rawsprite[0];
	legs = rawsprite[1];
	weapon = rawsprite[3];

	running = false;
	flametimer = -1.0f;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
template <class T>
Player<T>::~Player()
{
	if(rawsprite.size())
	{
		for(size_t k;k < rawsprite.size();k++){
			delete rawsprite[k];
			rawsprite[k] = NULL;
		}
	}
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
template <class T>
void Player<T>::Step(glm::vec3& cursor, float interpolate)
{
	// Update internal state, get position
	GameObject::Status stat = T::Update().pos;
	ps = stat.pos;
	//Interpolate
	if(interpolate > 0.0f)
		ps += stat.v * interpolate;

	ps.z = 0;
	// Adjust sprite angles
	angle = glm::vec3(0,0,0);
	int flip = 1;

	angle.z = glm::degrees(-glm::atan(cursor.y,cursor.x));
	if(cursor.x > 0){
		angle.x = 180;
		flip = -1;
	} else {
		float d = glm::distance(glm::vec3(0,0,0),cursor);
		float x = cursor.x + d*2;
		angle.z = (glm::degrees(-glm::atan(cursor.y,x)));
	}
	legs->angle.x = angle.x;
	torso->angle.x = angle.x;
	weapon->angle.x = angle.x;

	ps.x = int(ps.x);
	ps.y = int(ps.y);

	//Adjust position
	weapon->pos = ps; weapon->pos.x += 5*flip; weapon->pos.y += 2;
	legs->pos = ps; legs->pos.y += 3;
	torso->pos = ps; torso->pos.y += 3;

	flametimer -= timer.GetElapsedTime() - flametick;
	flametick = timer.GetElapsedTime();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
template <class T>
void Player<T>::Draw()
{
	if(!ftimer.empty()){
		float t = timer.GetElapsedTime();
		float dt = t - _time;

		std::vector<size_t> del;

		for(size_t i = 0; i < ftimer.size(); i++){
			ftimer[i] -= dt;
			if(ftimer[i] > 0.0f){
				jetflame->color.a = ftimer[i];
				jetflame->pos = fpos[i];
				jetflame->Draw();
			}
			else {
				del.push_back(i);
			}
		}

		if(!del.empty()){
			for(size_t i = 0;i < del.size(); i++){
				ftimer.erase(ftimer.begin()+del[i]);
				fpos.erase(fpos.begin()+del[i]);
			}

			del.clear();
		}

		_time = t;
	}

	if(running)
		legs->Draw();
	else
		torso->Draw();

	running = false;
	weapon->Draw();

	//Reset sprite pointers
	rawsprite[1]->pos = legs->pos;
	legs = rawsprite[1];
}

////////////////////////////////////////////////////////////
///Overloaded function for sprite switching
////////////////////////////////////////////////////////////
template <class T>
void Player<T>::Jet(void)
{
	T::Jet();
	rawsprite[2]->pos = legs->pos;
	legs = rawsprite[2];
	running = true;
	legs->Step();

	if(flametimer < 0.0f){
		flametimer = 0.04f;
		flametick = timer.GetElapsedTime();
		ftimer.push_back(1.0f);
		fpos.push_back(glm::vec3(ps.x,ps.y-15,0.0f));
	}
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
template <class T>
void Player<T>::Right(void)
{
	T::Right();
	legs->Step();
	running = true;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
template <class T>
void Player<T>::Left(void)
{
	T::Left();
	legs->Step();
	running = true;
}
