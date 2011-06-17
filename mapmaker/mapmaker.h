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

#ifndef MAPMAKER_HEADER_GUARD
#define MAPMAKER_HEADER_GUARD

#include <vector>
#include <glm/glm.hpp>
#include <cstdio>
#include <string>
#include "map.h"
#include "fixed_pipeline.h"
#include "input.h"

struct Poly{
    glm::vec3 v0,v1,v2;
};

struct MapMaker
{
public:
	MapMaker();

public:
	struct change_struct;

	void Step();
    bool Pick(change_struct *ch, size_t range);
    void ApplyChange(change_struct *ch, bool leftdown);
	void Init();

public:
	////////////////////////////////////////////////////////////
	/// Save map file
	////////////////////////////////////////////////////////////
    bool saveMap(const char *fpath)
    {
        bgmfsave(map, fpath);

        return true;
    }

	////////////////////////////////////////////////////////////
	/// Load map file
	////////////////////////////////////////////////////////////
	bool loadMap(const char *fpath)
    {
        if(map){
            bgmfdelete(map);
            map = NULL;
        }
        map = bgmfopen(fpath);
        if(map){
            printf("Loaded %s\n",fpath);
            printf("Poly count: %d\n",map->header.pc);
        }


        return true;
    }

	////////////////////////////////////////////////////////////
	/// This will find a vertex in 10px by 10px area
    ////////////////////////////////////////////////////////////
	inline glm::vec3* findVertex(float& mx, float &my,glm::vec3* ignore=NULL)
    {
        static float area = 10.0f;
        glm::vec3 *v = NULL;
        glm::vec3 *final = NULL;
        sf::Rect<float> pick;

        glm::vec3 a;
        for(bgmf_poly *p = &map->poly[0];
            p != &map->poly[map->header.pc]; p++)
        {
            a = p->data[0];
            pick = sf::Rect<float>(a.x-area,a.y-area,a.x+area,a.y+area);
            if(pick.Contains(mx,my))
                v = &p->data[0];;

            a = p->data[1];
            pick = sf::Rect<float>(a.x-area,a.y-area,a.x+area,a.y+area);
            if(pick.Contains(mx,my))
                v = &p->data[1];;

            a = p->data[2];
            pick = sf::Rect<float>(a.x-area,a.y-area,a.x+area,a.y+area);
            if(pick.Contains(mx,my))
                v = &p->data[2];;

            if(ignore != v)
                final = v;
        }

        return final;
    }

	////////////////////////////////////////////////////////////
	/// Change property struct
	////////////////////////////////////////////////////////////
    struct change_struct{
	public:
		////////////////////////////////////////////////////////////
		/// ctor
		////////////////////////////////////////////////////////////
		change_struct() { this->Clear(); }

	public:
		////////////////////////////////////////////////////////////
		/// Clear all the data
		////////////////////////////////////////////////////////////
		inline void Clear(void)
		{
			poly		= NULL;
			vertex		= NULL;
			vertex_color = NULL;
			spawn		= NULL;
			pPolygon	= NULL;
			move		= false;
			color		= false;
			remove		= false;
			pick_vertex	= false;
			pick_poly	= false;
			select		= false;
			picked		= false;
			spawna		= false;
			spawnb		= false;
			index		= 0;
		}

	public:
		////////////////////////////////////////////////////////////
		/// data
		////////////////////////////////////////////////////////////
        bgmf_poly   	*poly;
        glm::vec3   	*vertex;
        glm::vec4   	*vertex_color;
		glm::vec2		*spawn;
		bgmf_vert		pVert;
		bgmf_poly_view	*pPolygon;
        bool 			move;
        bool 			color;
        bool 			remove;
        bool 			pick_vertex;
        bool 			pick_poly;
        bool 			pick_color;
		bool			select;
		bool 			picked;
		bool 			spawna;
		bool 			spawnb;

        size_t      	index;
    };

public:
	////////////////////////////////////////////////////////////
	/// Texture handling
	////////////////////////////////////////////////////////////
	void loadTextureData();

	inline void FixTC(bgmf_poly_tex *pTc, size_t pTexID){
		float tsz		= 1.0f / map->texpath.size();
		float left		= pTexID * tsz;
		float right		= left + tsz;

		pTc->data[0] = glm::vec2(left, 0.0f);
		pTc->data[1] = glm::vec2(left, 1.0f);
		pTc->data[2] = glm::vec2(right, 1.0f);
	}

public:
    /// Data
	glm::vec3 grid[128*76];
	int count;
	bool drawgrid;
    bool load;
    bool vertex_snap;
	std::vector<glm::vec3> vertex;
    glm::vec3 mouse;
    glm::vec3 lastmouse;
	std::string		pWorkingDir;
    bgmf *map;

public:
	/// Graphics data
	struct GfxData {
		GLuint	Texture;
		std::vector<bgmf_poly_tex>	tc;
		std::vector<Sprite>	Scenery;
	} gfx;

public:
	/// Change structs
    change_struct   move_poly;
    change_struct   move_vertex;
    change_struct   color_vertex;
    change_struct   color_poly;
    change_struct   remove_poly;
	change_struct	pick_vertex;
	change_struct	pick_polygon;
	change_struct	spawn;
	change_struct*	change;

public:
	/// Events & IO
    Input				*input;
    gfx::FixedPipeline	*display;
	EventQueue			pEvents;

};



#endif //MAPMAKER_HEADER_GUARD
