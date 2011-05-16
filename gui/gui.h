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

#ifndef GUI_HEADER_GUARD
#define GUI_HEADER_GUARD

#include<SFML/Graphics.hpp>
#include<glm/glm.hpp>
#include<vector>
#include<string>

#include "widget.h"
#include "input.h"

struct sel_block
{
public:
    size_t size;
    std::string    name;
    std::vector<std::string> labels;
    std::vector<std::string> altlabel;
    std::vector<bool>   states;
    std::vector<sf::Rect<int> > btns;
    std::vector<size_t> id;
    sf::Rect<int>    box;
    bool contain;
};

#define GUIBLANK_SHAPE sf::Shape::Rectangle(0,0,0,0,sf::Color(0,0,0))
#define GUICOLOR_BACKGROUND sf::Color(232,232,232)
#define GUICOLOR_FOREGROUND sf::Color(234,234,234)
#define GUICOLOR_BORDER sf::Color(255,255,255)
#define GUICOLOR_BACKDROP sf::Color(160,160,160)
#define GUICOLOR_SHADE sf::Color(200,200,200)
#define GUICOLOR_BLACK sf::Color(0,0,0)

//Event queue defines
#define EVENT_GUI_CLICK 0

#define ONCLICK_CALLBACK 0

struct evtgui_click{
    static Event* Create(size_t* itemid){
        Event *event = new Event;
        event->type = EVENT_GUI_CLICK;
        event->level = 1;
        event->block = true;
        event->data = itemid;

        return event;
    }
};


////////////////////////////////////////////////////////////
///Menu bar
////////////////////////////////////////////////////////////
struct guiMenu : public guiWidget
{
public:
    guiMenu(guiWidget* parent = NULL);
    virtual ~guiMenu();

    size_t AddItem(std::string name);
    void AddOption(size_t item,unsigned id,std::string name,const char *altname=NULL);

    virtual bool update(Input* in);
    void Reset();

    size_t Pick;
    size_t FileID;
private:
    void _updateMenu(bool& left, int mx, int my);
    void _updateRMenu(bool& left, int mx, int my);

    struct _item_stc{
        std::string name;
        std::vector<std::string> labels;
        std::vector<std::string> altlabels;
        std::vector<sf::Rect<int> > areas;
        std::vector<bool> states;
        std::vector<size_t> id;
        sf::Rect<int> area;
        bool contain;
    };

    std::vector<_item_stc> _items;
    std::vector<_item_stc> _ritems;
    //gfx shapes
    sf::Shape   menubar;
    sf::Shape   menulinefull;
    sf::Shape   drop,shade,popdrop,popshade;
    sf::Rect<int> popmenu;
    sf::Shape   popview;
};

////////////////////////////////////////////////////////////
/// GUI system
////////////////////////////////////////////////////////////
class GUIsystem
{
public:
	//Constructor & Destructors
	////////////////////////////////////////////////////////////
	/// Default constructor
	////////////////////////////////////////////////////////////
    GUIsystem(sf::RenderWindow *window,Input *input,EventQueue *eq);

	////////////////////////////////////////////////////////////
	/// Default destructor
	////////////////////////////////////////////////////////////
	virtual ~GUIsystem();

	//I dont even remember what this is for
	bool Selected(size_t item,size_t option);

public:
	//Menu handling

	////////////////////////////////////////////////////////////
    /// Add Item to the menu. Return item id
    ////////////////////////////////////////////////////////////
	size_t AddItem(std::string name);

	////////////////////////////////////////////////////////////
	/// Add a menu option to an existing menu item
	////////////////////////////////////////////////////////////
    void AddOption(size_t item,unsigned id,std::string name,const char *altname=NULL);

public:
	//Widgets
	////////////////////////////////////////////////////////////
	/// Add a slider widget
	////////////////////////////////////////////////////////////
	size_t AddSlider(int _x, int _y, size_t length);


	////////////////////////////////////////////////////////////
	/// Set a widgets callback function
	////////////////////////////////////////////////////////////
    void SetWidgetCallback(size_t id, size_t type, void (*Callback)(guiWidget*,void*),void *data)
    {
        _widgets[id]->SetOnClickCallback(Callback, data);
    }

	//Managment

	////////////////////////////////////////////////////////////
	/// General update
	////////////////////////////////////////////////////////////
	void Update();

	////////////////////////////////////////////////////////////
	/// Reset all menu states
	////////////////////////////////////////////////////////////
	inline void resetStates()
    {
        popup = false;
        popcontain = false;

        Pick = 0;
        for(size_t k = 0;k<blocks.size();k++){
            for(size_t j = 0;j<blocks[k].states.size();j++){
                blocks[k].states[j] = false;
                popblocks[k].states[j] = false;
            }
        }
    }

public:
    //Drawing functions

	void drawMenu();
    void drawPopupMenu();
    void drawWidgets();

    size_t File;
    size_t Pick;
    bool block;
private:
	////////////////////////////////////////////////////////////
	/// Internal update Menu bara
	////////////////////////////////////////////////////////////
    void updateMenu(bool& left, int mx, int my);

	////////////////////////////////////////////////////////////
	/// Internal update popup menu
	////////////////////////////////////////////////////////////
	void updatePopup(bool& left, int mx, int my);

private:
	////////////////////////////////////////////////////////////
	/// Private Data: io
	////////////////////////////////////////////////////////////
	sf::RenderWindow* out;	//> output
    Input *in;				//> input

private:
	////////////////////////////////////////////////////////////
	/// Private Data: Global GUI shapes
	////////////////////////////////////////////////////////////
	sf::Shape   menubar;
    sf::Shape   menulinefull;
    sf::Shape   drop,shade,popdrop,popshade;
    sf::Shape   popview;

private:
	////////////////////////////////////////////////////////////
	/// Private Data
	////////////////////////////////////////////////////////////
	sf::Rect<int> popmenu;
    bool        popup;
	bool		popcontain;
    std::vector<sel_block>  blocks;
    std::vector<sel_block>  popblocks;
    std::vector<guiWidget*> _widgets;

    EventQueue* _eq;
};



#endif //GUI_HEADER_GUARD
