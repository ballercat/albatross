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


#include <GL/glew.h>
#include "assets.h"
#include "sprite.h"
#include "renderhooks.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SimpleIni.h"
#include <iostream>
#include <ostream>
#include <cassert>

////////////////////////////////////////////////////////////
/// ctor with no sprh file
////////////////////////////////////////////////////////////
Sprite::Sprite(GLuint &p_TextureID) :
Builder(gfx::Link::Instance().spriteBuild),
Renderer(gfx::Link::Instance().spriteDraw),
mTime(&timing::GlobalTime::Instance())
{

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, p_TextureID);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &pInfo.w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &pInfo.h);
	glDisable(GL_TEXTURE_2D);

	pInfo.imgWidth = pInfo.w;
	pInfo.imgHeight = pInfo.h;

	pInfo.txrId = p_TextureID;
	pInfo.aniUpdate = mTime->GetElapsedTime();

	Builder(this);
}

////////////////////////////////////////////////////////////
///Generic sprite loader
////////////////////////////////////////////////////////////
Sprite::Sprite(const char *fpath, GLuint texid) :
Builder(gfx::Link::Instance().spriteBuild),
Renderer(gfx::Link::Instance().spriteDraw),
mTime(&timing::GlobalTime::Instance())
{
	/* File path is NOT to the image but to
	 * the .spr text file holding the sprite data
	 * this really cuts down on having to compile
	 * in all the sprite information every time */
	_parseInfo(fpath);

	pInfo.txrId = texid;
	pInfo.aniUpdate = mTime->GetElapsedTime();

	Builder(this);
}

////////////////////////////////////////////////////////////
/// Parse a .sprh file(Sprite header)
////////////////////////////////////////////////////////////
void Sprite::_parseInfo(const char *fpath)
{
	CSimpleIniA sprh;
	if(sprh.LoadFile(fpath) < 0){
		std::cerr << "Error reading " << fpath << " sprite header\n";
	}

	pInfo.animated = sprh.GetBoolValue("Sprite", "animated");

	pInfo.w = sprh.GetDoubleValue("Sprite", "width");
	pInfo.h = sprh.GetDoubleValue("Sprite", "height");
	//off.x = sprh.GetDoubleValue("Offset", "x");
	//off.y = sprh.GetDoubleValue("Offset", "y");

	pInfo.pivot.x = sprh.GetDoubleValue("Pivot", "x");
	pInfo.pivot.y = sprh.GetDoubleValue("Pivot", "y");

	pInfo.color.r = sprh.GetDoubleValue("Color", "red");
	pInfo.color.g = sprh.GetDoubleValue("Color", "green");
	pInfo.color.b = sprh.GetDoubleValue("Color", "blue");
	pInfo.color.a = sprh.GetDoubleValue("Color", "alpha");

	pInfo.scale.x = float(sprh.GetDoubleValue("Scale", "x"));
	pInfo.scale.y = sprh.GetDoubleValue("Scale", "y");
	pInfo.scale.z = sprh.GetDoubleValue("Scale", "z");

	pInfo.imgWidth = pInfo.w;
	pInfo.imgHeight = pInfo.h;

	if(pInfo.animated){
		pInfo.imgWidth = sprh.GetDoubleValue("Image", "width");
		pInfo.imgHeight = sprh.GetDoubleValue("Image", "height");
		pInfo.aniLength = sprh.GetLongValue("Animation","length");
		pInfo.aniSpeed = sprh.GetDoubleValue("Animation","speed");
	}
}

////////////////////////////////////////////////////////////
/// Step trough the frames
////////////////////////////////////////////////////////////
void Sprite::Step(void)
{
	//Don't waste time with static sprites
	if(!pInfo.aniLength)
		return;

	float t		= mTime->GetElapsedTime();

	//Global Time can be reset, need to account for that so..
	if( t < pInfo.aniUpdate )
		pInfo.aniUpdate = t - pInfo.aniSpeed;

	float dt 	= t - pInfo.aniUpdate;

	if( dt >= pInfo.aniSpeed )
    {
        pInfo.aniPosition+=int(dt/pInfo.aniSpeed);
        if(pInfo.aniPosition > pInfo.aniLength){
			pInfo.aniPosition = 0;
		}
		pInfo.aniUpdate = t;
    }

}

void Sprite::Draw()
{
	Renderer(this);
}

