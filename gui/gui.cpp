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


#include "gui.h"
#include "slider.h"
#include "panel.h"

GUIsystem::GUIsystem(sf::RenderWindow *window,Input *input,EventQueue* eq) :
	Pick(0)
{
    _eq = eq;
    out = window;
    in = input;

    menubar = sf::Shape::Rectangle(0,0,1024,14, GUICOLOR_BACKGROUND);
    menulinefull = sf::Shape::Line(0,14,1024,14,1, GUICOLOR_BORDER);
    popview = GUIBLANK_SHAPE;

    File = AddItem("File");
    popup = false;
    popcontain = false;
    block = false;

}
GUIsystem::~GUIsystem()
{
    blocks.clear();
    popblocks.clear();
}
bool
GUIsystem::Selected(size_t item,size_t option)
{
    return blocks[item].states[option];
}

///Add Item to the menu. Return item id
size_t
GUIsystem::AddItem(std::string name)
{
    sel_block Item;
    size_t c = blocks.size(); //count

    Item.box = sf::Rect<int>(c*40,0,(c+1)*40,13);
    Item.name = name;
    Item.size = 0;
    Item.contain = false;

    blocks.push_back(Item);
    popblocks.push_back(Item);

    return (c);
}

void
GUIsystem::AddOption(size_t item,unsigned id,std::string name,const char* altname)
{
    sel_block *Item = &blocks[item];
    size_t c = Item->states.size(); //options count

    Item->states.push_back(false);
    Item->labels.push_back(name);
    if(altname)
        Item->altlabel.push_back(altname);
    else
        Item->altlabel.push_back(name);

    Item->btns.push_back(sf::Rect<int>(item*40,(c+1)*13,item*40+70,(c+2)*13));
    Item->id.push_back(id);
    printf("option %s\n",Item->labels[Item->size].c_str());
    Item->size = (c+1);

    Item = &popblocks[item];
    Item->states.push_back(false);
    Item->labels.push_back(name);
    if(altname)
        Item->altlabel.push_back(altname);
    else
        Item->altlabel.push_back(name);
    Item->btns.push_back(sf::Rect<int>(0,0,0,0));
    Item->id.push_back(id);
    Item->size = (c+1);
}

size_t GUIsystem::AddSlider(int _x, int _y, size_t length)
{
    size_t id = _widgets.size();
    guiSlider* slider = new guiSlider(_widgets.size(),_x,_y,length);
    _widgets.push_back(slider);

    return id;
}

void GUIsystem::Update()
{
    int mx = out->GetInput().GetMouseX();
    int my = out->GetInput().GetMouseY();
    bool left = in->mState.mouse.left;

    drop = GUIBLANK_SHAPE;
    shade = GUIBLANK_SHAPE;
    popdrop = GUIBLANK_SHAPE;
    popshade = GUIBLANK_SHAPE;

    Pick = 0;

    //right click
    if(in->mState.mouse.right){
        //turn on right click menu
        //undo mouse state
        //flag for drawing
        sel_block *Item = NULL;
        popmenu = sf::Rect<int>(mx,my,mx+40,my+(13*blocks.size()));
        popview = sf::Shape::Rectangle(mx,my,mx+40,my+(13*blocks.size()),GUICOLOR_BACKGROUND);

        for(size_t k=0;k<popblocks.size();k++){
            Item = &popblocks[k];

            Item->box = sf::Rect<int>(popmenu.Left,popmenu.Top+(k*13),
                                      popmenu.Right,popmenu.Top+((k+1)*13));

            for(size_t j=0;j<Item->labels.size();j++){
                Item->btns[j] = sf::Rect<int>(Item->box.Right,Item->box.Top+(j*13),
                                              Item->box.Right+40,Item->box.Bottom+((j+1)*13));
            }
        }

        in->mState.mouse.right = false;
        popup = true;
    }

    block = false;
    updateMenu(left, mx, my);
    updatePopup(left, mx, my);
    if(!_widgets.empty()){
        for(size_t k = 0;k<_widgets.size();k++){
            if(_widgets[k]->update(in))
                break;
        }
    }
}

void GUIsystem::updateMenu(bool& left, int mx, int my)
{
    sf::Rect<int> obox;
    sel_block *Item = NULL;
    for(size_t k=0;k != blocks.size();k++)
    {
        Item = &blocks[k];
        if(Item->contain && !Item->box.Contains(mx,my)){
            obox = sf::Rect<int>(k*40,0,(k+1)*40,13*(Item->size+1));
            Item->contain = obox.Contains(mx,my);
        }else {
            Item->contain = Item->box.Contains(mx,my);
        }

        if(Item->contain){
            block = true;
            drop = sf::Shape::Rectangle(40*k,13,40*k+70,
                                        13+13*(Item->size+1),
                                        GUICOLOR_BACKDROP);

            for(size_t j=0;j<Item->labels.size();j++){
                if(Item->btns[j].Contains(mx,my)){
                    //shade aka backdrop for a menu item that is highlighted
                    shade = sf::Shape::Rectangle(k*40,(j+1)*13,
                                                 k*40+70,(j+2)*13,
                                                 GUICOLOR_SHADE);
                    //Check for item selections
                    if(left){
                        Item->states[j] = !Item->states[j];
                        Pick = Item->id[j];
                        //Reset mouse state because gui has input priority
                        in->mState.mouse.left = false;
                        _eq->push(evtgui_click::Create(&Pick));
                    }
                }
            }
        }
    }
}

void GUIsystem::updatePopup(bool& left, int mx, int my)
{
    if(!popup)
        return;

    sf::Rect<int> mini;
    sf::Rect<int> opt;
    sel_block *Pop = NULL;

    for(size_t k=0;k<popblocks.size();k++){
        Pop = &popblocks[k];

        //Check if mouse is howering over the menu
        if(Pop->contain && !Pop->box.Contains(mx,my)){
            //Check if the mouse was last over popup menu
            //and then over the mini popup menu
            mini = sf::Rect<int>(popmenu.Right,popmenu.Top+(k*13),
                                 popmenu.Right+70,popmenu.Top+(k*13)+(Pop->size*13));

            Pop->contain = mini.Contains(mx,my);
        }
        else {
            Pop->contain = Pop->box.Contains(mx,my);
        }
        if(Pop->contain){
            block = true;
            //Build a popup menu backdrop
            popdrop = sf::Shape::Rectangle( popmenu.Right,popmenu.Top+(13*k),
                                            popmenu.Right+70,popmenu.Top+(13*k)+(13*Pop->size),
                                            GUICOLOR_BACKDROP );

            for(size_t j=0;j<Pop->labels.size();j++){
                //Mouse inside popup box
                if(Pop->btns[j].Contains(mx,my)){
                    opt = Pop->btns[j];
                    //Build a shade for the popup option
                    popshade = sf::Shape::Rectangle( opt.Right,opt.Top,
                                                     opt.Left,opt.Top+13,
                                                     GUICOLOR_SHADE );
                    if(left){
                        Pop->states[j] = !Pop->states[j];
                        Pick = Pop->id[j];
                        in->mState.mouse.left = false;
                        _eq->push(evtgui_click::Create(&Pick));
                    }
                }
            }
        }
    }


    if(left && !popmenu.Contains(mx,my)){
        popmenu = sf::Rect<int>(0,0,0,0);
        popup = false;
    }
}

void GUIsystem::drawPopupMenu()
{
    if(!popup)
        return;

    out->Draw(popview);
    out->Draw(popdrop);
    out->Draw(popshade);

    sf::String pLabel;
    pLabel.SetSize(11);
    size_t k = 0;

    for(sel_block *Pop = &popblocks[0];Pop != &popblocks[popblocks.size()];Pop++)
    {
        pLabel.SetColor(GUICOLOR_BLACK);
        if(Pop->contain){
            for(size_t j=0;j<Pop->labels.size();j++){
                pLabel.SetText(Pop->labels[j]);
                pLabel.SetPosition(Pop->btns[j].Left,Pop->btns[j].Top);
                if(Pop->states[j]){
                    pLabel.SetColor(sf::Color(100,150,100));
                    pLabel.SetText(Pop->altlabel[j]);
                }
                out->Draw(pLabel);
                pLabel.SetColor(GUICOLOR_BLACK);
            }
        }
        pLabel.SetText(Pop->name);
        pLabel.SetPosition(popmenu.Left,popmenu.Top+(13*k)); k++;
        out->Draw(pLabel);
    }
}

void GUIsystem::drawMenu()
{
    out->Draw(menubar);
    out->Draw(menulinefull);
    out->Draw(drop);
    out->Draw(shade);

    //Draw Menu items
    sf::String mLabel;
    mLabel.SetSize(11);
    mLabel.SetColor(sf::Color(0,0,0));
    size_t k = 0;

    for(sel_block *Item = &blocks[0];Item != &blocks[blocks.size()];Item++)
    {
        if(Item->contain){
            //Draw menu options
            for(size_t j=0;j<Item->labels.size();j++){
                mLabel.SetText(Item->labels[j]);
                mLabel.SetPosition(3 + 40*k, 13 *(j+1));
                if(Item->states[j]){
                    mLabel.SetColor(sf::Color(100,150,100));
                    mLabel.SetText(Item->altlabel[j]);
                }
                out->Draw(mLabel);
                mLabel.SetColor(sf::Color(0,0,0));
            }
        }
        //Draw the item label
        mLabel.SetText(Item->name);
        mLabel.SetPosition(3 + 40*k,0);k++;
        out->Draw(mLabel);
    }
}

void GUIsystem::drawWidgets()
{
    if(!_widgets.empty()){
        guiWidget *widget;
        for(size_t k=0;k<_widgets.size();k++){
            widget = _widgets[k];
            if(widget->pShapes.size()){
                for(size_t j=0;j<widget->pShapes.size();j++){
                    out->Draw(*(widget->pShapes[j]));
                }
            }
        }
    }
}
