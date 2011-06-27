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
	typedef struct spriteInfo_t{
		spriteInfo_t(void) :
			w(0),
			h(0),
			pos(glm::vec3(0.0f)),
			angle(glm::vec3(0.0f)),
			pivot(glm::vec2(0.5f, 0.5f)),
			scale(glm::vec3(1.0f, 1.0f, 1.0f)),
			color(glm::vec4(1,1,1,1)),
			imgWidth(0.0f),
			imgHeight(0.0f),
			txrId(-1),
			animated(false),
			aniSpeed(0.0f),
			aniPosition(0),
			aniLength(0),
			aniUpdate(0.0f),
			vboIndex(0),
			vboVertex(0),
			vaoVertex(0),
			shdProgram(NULL),
			shdColor_loc(0),
			shdAniStep_loc(0)
			{

			}

		//data
		GLint	 w, h;
		glm::vec3 pos;
		glm::vec3 angle;
		glm::vec2 pivot;
		glm::vec3 scale;
		glm::vec4 color;
		float	imgWidth, imgHeight;
		int		txrId;
		bool	animated;
		float	aniSpeed;
		int		aniPosition;
		int		aniLength;
		float	aniUpdate;

		GLuint	vboIndex;
		GLuint	vboVertex;
		GLuint	vaoVertex;

		Shader* shdProgram;
		GLuint	shdColor_loc;
		GLuint	shdAniStep_loc;
	}Info;

public:
	////////////////////////////////////////////////////////////
	///Default constructor
	////////////////////////////////////////////////////////////
	Sprite() :
	Builder(NULL),
	Renderer(NULL),
	mTime(&timing::GlobalTime::Instance())
	{

	}

	////////////////////////////////////////////////////////////
	///Constructor, from .sprh file and a texture id
	////////////////////////////////////////////////////////////
    Sprite(const char *fpath, GLuint texid);

	////////////////////////////////////////////////////////////
	/// ctor without a sprh file, default settings, no animation
	////////////////////////////////////////////////////////////
	Sprite(GLuint &p_TextureID);

	////////////////////////////////////////////////////////////
	/// Copy Constructor
	////////////////////////////////////////////////////////////
	Sprite(const Sprite& p_Copy) :
	pInfo(p_Copy.pInfo),
	Builder(p_Copy.Builder),
	Renderer(p_Copy.Renderer),
	mTime(&timing::GlobalTime::Instance())
	{

	}

	////////////////////////////////////////////////////////////
	/// Asignment operator
	////////////////////////////////////////////////////////////
	inline Sprite& operator=(const Sprite& p_Copy)
	{
		this->pInfo			= p_Copy.pInfo;
		this->Builder		= p_Copy.Builder;
		this->Renderer		= p_Copy.Renderer;
		this->mTime			= &timing::GlobalTime::Instance();

		return *this;
	}

public:
	////////////////////////////////////////////////////////////
	///Step trough frames
	////////////////////////////////////////////////////////////
	virtual void Step(void);

	////////////////////////////////////////////////////////////
	///Draw
	////////////////////////////////////////////////////////////
	void Draw();

public:
	Info		pInfo;
	void		(*Builder)(Sprite *);
	void		(*Renderer)(Sprite *);

protected:
	////////////////////////////////////////////////////////////
	///Parse .spr file for sprite data
	////////////////////////////////////////////////////////////
	void _parseInfo(const char *fpath);

protected:
	///Data
	sf::Clock*	mTime;
};


#endif //SPRITE_HEADER_GUARD
