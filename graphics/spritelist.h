#ifndef SPRITE_LIST_HEADER_GUARD
#define SPRITE_LIST_HEADER_GUARD

#include "sprite.h"

namespace mercsprite
{
struct Make
{
	static Sprite* Still(GLuint texid)
	{
		Sprite* sp = new Sprite();
		sp->width = 50;
		sp->height = 50;
		sp->imgd = glm::vec2(50,50);
		sp->off = glm::vec3(0,0,0);
		//sp->scale = glm::vec3(1.5,1.5,0);
		sp->color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
		sp->xstart = 0;
		sp->textureid = texid;
		sp->Build();

		return sp;
	}
	static Sprite* Run(GLuint texid)
	{
		Sprite* sp = new Sprite();
		sp->width = 50;
		sp->height = 50;
		sp->imgd = glm::vec2(300,50);
		sp->off = glm::vec3(0,0,0);
		//sp->scale = glm::vec3(1.5,1.5,0);
		sp->color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
		sp->xstart = 0;
		sp->textureid = texid;
		sp->mLength = 5;
		sp->mSpeed = 0.2f;
		sp->Build();

		return sp;
	}
    static Sprite* Torso(GLuint texid)
    {
        Sprite *sp = new Sprite();
        sp->width = 20;
        sp->height = 20;
		sp->imgd = glm::vec2(20,20);
        sp->off = glm::vec3(0,0,0);
        sp->scale = glm::vec3(1,1,0);
        sp->color = glm::vec4(0.4f,0.4f,0.5f,1.0f);
        sp->textureid = texid;
		sp->Build();

        return sp;
    }
    static Sprite* Legs(GLuint texid)
    {
        Sprite *sp = new Sprite();
        sp->width = 50;
        sp->height = 50;
		sp->imgd = glm::vec2(200,50);
        sp->off = glm::vec3(0,0,0);
        sp->scale = glm::vec3(1,1,0);
        sp->xstart = 0;
        sp->textureid = texid;
        sp->mLength = 7;
        sp->mSpeed = 0.10f;
		sp->Build();

        return sp;
    }
    static Sprite* Jets(GLuint texid)
    {
        Sprite *sp = new Sprite();
        sp->width = 50;
        sp->height = 50;
		sp->imgd = glm::vec2(200,50);
		sp->off = glm::vec3(0,0,0);
		sp->color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
		sp->mLength = 3;
		sp->mSpeed = 0.15f;
		sp->xstart = 0;
        sp->textureid = texid;
		sp->Build();

        return sp;
    }

	static Sprite* JetFlame(GLuint texid)
	{
		Sprite *sp = new Sprite();
		sp->width = 10;
		sp->height = 10;
		sp->imgd = glm::vec2(10,10);
		sp->off = glm::vec3(0,0,0);
		sp->color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
		sp->mLength = 0;
		sp->xstart = 0;
		sp->textureid = texid;
		sp->Build();

		return sp;
	}
};
}

namespace bulletsprite
{
struct Make
{
    static Sprite* Test(GLuint texid)
    {
        Sprite *sp = new Sprite();
        sp->width = 21;
        sp->height = 3;
        //sp->pivot = glm::vec3(0,0,0);
        sp->scale = glm::vec3(2,1,1);
        sp->textureid = texid;
		sp->imgd = glm::vec2(21,3);
		sp->Build();

        return sp;
    }
};
}

namespace ak47sprite
{
struct Make
{
    static Sprite *AK47(GLuint texid)
    {
        Sprite *sp = new Sprite();
        sp->width = 33;
        sp->height = 20;
        sp->textureid = texid;
		sp->imgd = glm::vec2(33,20);
		sp->scale = glm::vec3(2.0f,2.0f,0.0f);
		sp->Build();

        return sp;
    }
};
}

namespace treesprite
{
struct Make
{
	static Sprite* Tree(GLuint texid)
	{
		Sprite *sp = new Sprite();
		sp->width = 100;
		sp->height = 200;
		sp->textureid = texid;
		sp->imgd = glm::vec2(100,200);
		sp->Build();

		return sp;
	}
};
}

#endif //SPRITE_LIST_HEADER_GUARD
