#ifndef WEAPON_HEADER_GUARD
#define WEAPON_HEADER_GUARD

#include "objects.h"
#include "bullets.h"

namespace object
{
	////////////////////////////////////////////////////////////
	// Weapons
	////////////////////////////////////////////////////////////
	class Weapon : public GameObject
	{
	public:
		struct Creator:
				public ObjectCreator
		{
			Creator()
			{
				GameObject::CreatorsMap()[WEAPON] = this;
			}
			virtual inline Weapon* Create(void) const
			{
				return new Weapon();
			}
		};

	public:
		struct Info
		{
			static void Load(const char* fpath, Info *p_Info);

			inline Info& operator=(const Info& p_Copy)
			{
				this->Name 		= p_Copy.Name;
				this->Clip 		= p_Copy.Clip;
				this->Ammo 		= p_Copy.Ammo;
				this->Rate		= p_Copy.Reload;
				this->Rate 		= p_Copy.Rate;
				this->Speed 	= p_Copy.Speed;
				this->Dmg 		= p_Copy.Dmg;
				this->Explosive = p_Copy.Explosive;
				this->Type 		= p_Copy.Type;

				return *this;
			}

			std::string Name;
			int			Clip;
			int			Ammo;
			float		Rate;
			float		Reload;
			float		Speed; // bullet speed
			float		Dmg;
			bool 		Explosive;

			//Sprite info
			int			Type;
		};

	public:
		////////////////////////////////////////////////////////////
		/// Constructor
		////////////////////////////////////////////////////////////
		Weapon(void);
		Weapon(const Info& p_Info);

	public:
		////////////////////////////////////////////////////////////
		// Update state
		////////////////////////////////////////////////////////////
		virtual GameObject::Status& Update(float unused);

		////////////////////////////////////////////////////////////
		// Initialize self
		////////////////////////////////////////////////////////////
		virtual void Initialize(void);

		////////////////////////////////////////////////////////////
		// Spawn objects
		////////////////////////////////////////////////////////////
		virtual void Spawn(glm::vec3 pos);

		inline void Kill(void)
		{
			delete m_phBody;
			m_phBody = NULL;
		}

	public:
		///////////////////////////////////////////////////////////
		// Move to a location
		////////////////////////////////////////////////////////////
		virtual void Move(glm::vec3 p_Pos, float p_Time)
		{
		}

		////////////////////////////////////////////////////////////
		// Apply Impulse
		////////////////////////////////////////////////////////////
		void Impulse(glm::vec3 p_Imp);
		virtual void Impulse(glm::vec3 p_Imp, float p_x, float p_y);

	public:
		////////////////////////////////////////////////////////////
		/// Return apropriate bullet type
		////////////////////////////////////////////////////////////
		Bullet* Shoot(void);

	public:
		////////////////////////////////////////////////////////////
		/// Public data
		////////////////////////////////////////////////////////////
		Weapon::Info	pInfo;

	private:
		physics::Rectangle *m_phBody;
	};
}

#endif //#define WEAPON_HEADER_GUARD
