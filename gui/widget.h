#ifndef GUIWIDGET_HEADER_GUARD
#define GUIWIDGET_HEADER_GUARD

#include<SFML/Graphics.hpp>
#include<vector>
#include<glm/glm.hpp>

#include "input.h"

////////////////////////////////////////////////////////////
/// Base class for a gui widget
////////////////////////////////////////////////////////////
struct guiWidget
{
public:
	////////////////////////////////////////////////////////////
	/// Update function
	////////////////////////////////////////////////////////////
	virtual bool update(Input* _in) = 0;

	////////////////////////////////////////////////////////////
	/// Set a OnClick Event callback
	////////////////////////////////////////////////////////////
    virtual void SetOnClickCallback(void (*p_CallBack)(guiWidget*,void*),void *p_Data)
    {
		OnClick.CallBack 	= p_CallBack;
		OnClick.Data		= p_Data;
    }

public:
	//Visual Data
    std::vector<sf::Shape*>		pShapes;
    std::vector<sf::String*> 	pStrings;

public:
	//Location Data
	glm::vec2	pLoc;
	glm::vec2	pDim;

    int x, y, w, h;

protected:
	//Private Data
	guiWidget*		mParent;
    sf::Rect<int> 	mArea;
    bool 			mSelected;

protected:
	//On click callback data
	struct OnClickEvent{
		void (*CallBack)(guiWidget*, void*);
		void*	Data;
	} OnClick;
};

#endif //GUIWIDGET_HEADER_GUARD
