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

	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
    MapMaker()
    {
        load = false;
        vertex_snap = false;

        mode_string = "Mode: Normal";
        map = NULL;

        move_poly.move = true;
        move_poly.pick_poly = true;
        move_vertex.move = true;
        move_vertex.pick_vertex = true;
        color_vertex.color = true;
        color_vertex.pick_color = true;
        color_poly.color = true;
        color_poly.pick_poly = true;
        remove_poly.remove = true;
        remove_poly.pick_poly = true;

		pick_vertex.pick_vertex = true;

		change = NULL;
    }

	void Run();
	void Step();

    bool saveMap(const char *fpath)
    {
        bgmfsave(map, "assets/maps/test.bgmf");

        return true;
    }

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

    //This will find a vertex in 10px by 10px area
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

    struct change_struct{
        change_struct()
        {
            poly = NULL;
            vertex = NULL;
            vertex_color = NULL;
            move = false;
            color = false;
            remove = false;
            pick_vertex = false;
            pick_poly = false;
            pick_color = false;
			picked = false;
            index = 0;
        }
        bgmf_poly   *poly;
        glm::vec3   *vertex;
        glm::vec4   *vertex_color;
		bgmf_vert	pVert;
        bool move;
        bool color;
        bool remove;
        bool pick_vertex;
        bool pick_poly;
        bool pick_color;
		bool picked;

        size_t      index;
    };

    bool Pick(change_struct *ch, size_t range);
    void ApplyChange(change_struct *ch, bool leftdown);

    //Data
    bool load;
    bool vertex_snap;

    glm::vec3 mouse;
    glm::vec3 lastmouse;

    std::string mode_string;

    bgmf *map;

    std::vector<glm::vec3> vertex;

    change_struct   move_poly;
    change_struct   move_vertex;
    change_struct   color_vertex;
    change_struct   color_poly;
    change_struct   remove_poly;
	change_struct	pick_vertex;
	change_struct	pick_polygon;

	change_struct*	change;

    float colorR,colorG,colorB;

    Input* input;
    gfx::FixedPipeline* display;
	EventQueue	pEvents;

	//message::Queue *MessageQueue;
};



#endif //MAPMAKER_HEADER_GUARD
