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


#ifndef SPRITE_HEADER_GUARD
#define SPRITE_HEADER_GUARD
#include "assets.h"
#include "timing.h"
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <cstring>

////////////////////////////////////////////////////////////
///Sprite class
////////////////////////////////////////////////////////////
struct Sprite
{
public:
	////////////////////////////////////////////////////////////
	///Constructor, from .sprh file and a texture id
	////////////////////////////////////////////////////////////
    Sprite(const char *fpath, GLuint texid);

	////////////////////////////////////////////////////////////
	///Default constructor
	////////////////////////////////////////////////////////////
	Sprite();

	////////////////////////////////////////////////////////////
	/// ctor with only a .sprh file
	////////////////////////////////////////////////////////////		
	Sprite(const char *p_SPRH);
	
	////////////////////////////////////////////////////////////
	/// Copy Constructor
	////////////////////////////////////////////////////////////
	Sprite(const Sprite& p_Copy) :
		height(p_Copy.height),
		width(p_Copy.width),
		imgd(p_Copy.imgd),
		angle(p_Copy.angle),
		pos(p_Copy.pos),
		pivot(p_Copy.pivot),
		off(p_Copy.off),
		scale(p_Copy.scale),
		xstart(p_Copy.xstart),
		color(p_Copy.color),
		mLength(p_Copy.mLength),
		mSpeed(p_Copy.mSpeed),
		mPosition(p_Copy.mPosition),
		mLastUpdate(p_Copy.mLastUpdate),
		textureid(p_Copy.textureid),
		texdata(p_Copy.texdata),
		vertdata(p_Copy.vertdata),
		mTime(&timing::GlobalTime::Instance())
		{

		}

	////////////////////////////////////////////////////////////
	/// Asignment operator
	////////////////////////////////////////////////////////////
	inline Sprite& operator=(const Sprite& p_Copy)
	{
		this->height 		= p_Copy.height;
		this->width 		= p_Copy.width;
		this->imgd			= p_Copy.imgd;
		this->angle			= p_Copy.angle;
		this->pos			= p_Copy.pos;
		this->pivot			= p_Copy.pivot;
		this->off			= p_Copy.off;
		this->scale			= p_Copy.scale;
		this->xstart		= p_Copy.xstart;
		this->color			= p_Copy.color;
		this->mLength		= p_Copy.mLength;
		this->mSpeed		= p_Copy.mSpeed;
		this->mPosition		= p_Copy.mPosition;
		this->mLastUpdate	= p_Copy.mLastUpdate;
		this->textureid		= p_Copy.textureid;
		this->mTime			= &timing::GlobalTime::Instance();

		memcpy(this->texdata, p_Copy.texdata, sizeof(glm::vec2)*6);
		memcpy(this->vertdata, p_Copy.vertdata, sizeof(glm::vec3)*6);

		return *this;
	}

    inline const void* getPixelpointer(void)
    {
        return NULL;
    }

public:
	////////////////////////////////////////////////////////////
	///Build vertex & texture coordinates
	////////////////////////////////////////////////////////////
	void Build();

	////////////////////////////////////////////////////////////
	///Step trough frames
	////////////////////////////////////////////////////////////
	virtual void Step(void);

	////////////////////////////////////////////////////////////
	///Draw
	////////////////////////////////////////////////////////////
	void Draw();

public:
    //Sprite & animation information
    uint32_t    height,width;
	glm::vec2	imgd;
    glm::vec3   angle;
    glm::vec3   pos;
    glm::vec3   pivot;
    glm::vec3   off;
    glm::vec3   scale;
    float       xstart;
    glm::vec4   color;

#ifdef SHADER_PIPELINE
public:
	glm::mat4	mvp,proj,model,view;
	GLuint		_mvp;
#endif

public:
    ///Animation controll
    size_t  mLength;
    float   mSpeed;
    size_t  mPosition;
	float   mLastUpdate;
    int		textureid;

	glm::vec2 texdata[6];
	glm::vec3 vertdata[6];

protected:
	////////////////////////////////////////////////////////////
	///Parse .spr file for sprite data
	////////////////////////////////////////////////////////////
	void _parseInfo(const char *fpath);

protected:
	///Data
	sf::Clock*	mTime;
#ifdef SHADER_PIPELINE
	glm::vec2 texcoord[6];
	glm::vec3 vcoord[6];
	VertexArray _quad;
#endif
};


#endif //SPRITE_HEADER_GUARD
