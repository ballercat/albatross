#include "gameclient.h"
#include "SimpleIni.h"
#include <string>

////////////////////////////////////////////////////////////
/// Load & apply some settings
////////////////////////////////////////////////////////////
void MainClient::_loadGameSettings(void)
{
	CSimpleIniA ini;
	if(	ini.LoadFile("settings.ini") < 0 ){
		throw "Could not open the settings file!";
	}

	//Input .ini file settings
	info.debug			= ini.GetBoolValue("Settings", "debug");
	info.mapfile 		= ini.GetValue("Settings", "map");

	info.fullscreen 	= ini.GetBoolValue("Window", "fullscreen");
	info.window_width	= ini.GetLongValue("Window", "width");
	info.window_height	= ini.GetLongValue("Window", "height");
	info.cursorfile 	= ini.GetValue("Window", "cursor");

	info.fpslimit 		= ini.GetLongValue("Graphics", "fpslimit");
	info.frameskip 		= ini.GetLongValue("Graphics", "frameskip");

	/* Load the weapon list */
	CSimpleIniA weaponlist;
	weaponlist.LoadFile("assets/weaponlist.ini");

	CSimpleIniA::TNamesDepend keys;
	weaponlist.GetAllKeys("Weapons", keys);

	CSimpleIniA::TNamesDepend::const_iterator i;
	for(i = keys.begin(); i != keys.end(); ++i){
		//corresponds to the weapon select menu
		WeaponList.push_back(weaponlist.GetValue("Weapons", i->pItem));
	}

} //void MainClient::_loadGameSettings(void)

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void MainClient::_populateSprites(void)
{
	CSimpleIniA bullet;
	CSimpleIniA weapon;
	std::string path;

	size_t sz = WeaponList.size();

	mBulletTex.resize(sz);

	//Generate Textures
	glGenTextures(sz, &mBulletTex[0]);

	//Populate bullet sprites
	for(size_t k = 0; k < sz; k++){
		//Load weapons.ini
		weapon.LoadFile(WeaponList[k].c_str());

		path = WeaponList[k] + "/bullet.png";
		LoadTex(mBulletTex[k], path.c_str());

		path = WeaponList[k] + "/bullet.sprh";
		mBulletSprite.push_back(Sprite(path.c_str(), mBulletTex[k]));
	}
}
