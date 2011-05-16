#ifndef GUISLIDER_HEADER_GUARD
#define GUISLIDER_HEADER_GUARD

#include "widget.h"

////////////////////////////////////////////////////////////
/// Slider widget
////////////////////////////////////////////////////////////
struct guiSlider : public guiWidget
{
public:
	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
    guiSlider(size_t id, int _x, int _y,size_t p_Length, guiWidget* p_Parent=NULL)
	{
		//Set Parent
		mParent	= p_Parent;

		x = _x;
		y = _y;
		if(mParent){
			x = mParent->x + x;
			y = mParent->y + x;
		}

		w = 20;
		h = p_Length;

		mBackgroundShape 	= sf::Shape::Rectangle(x, y, x+w, y+h, GUICOLOR_BACKDROP);
		mDialShape 			= sf::Shape::Rectangle(x, y+h, x+w, y+h-10, GUICOLOR_FOREGROUND);

		mArea 				= sf::Rect<int>(x,y,x+w,y+h);
		mDialArea 			= sf::Rect<int>(x,y,x+w,y+h);

		pShapes.push_back(&mBackgroundShape);
		pShapes.push_back(&mDialShape);
    }

	////////////////////////////////////////////////////////////
	/// dtor
	////////////////////////////////////////////////////////////
    virtual ~guiSlider()
	{
		pShapes.clear();
		pStrings.clear();
	}

	////////////////////////////////////////////////////////////
	/// Update self
	////////////////////////////////////////////////////////////
	virtual bool update(Input* in)
	{
		int mx = in->GetInput().GetMouseX();
		int my = in->GetInput().GetMouseY();
		bool left = in->IsMouseButtonDown(Input::Mouse::Left);

		if(mArea.Contains(mx,my) && left){

			int dh = my - mArea.Bottom;

			mDialArea 	= sf::Rect<int>(x, y+dh, x+w, y+dh-10);
			mDialShape 	= sf::Shape::Rectangle(x, y+h+dh, x+w, y+h+dh-10, GUICOLOR_FOREGROUND);

			// Reset mouse state
			in->mState.mouse.left = false;
			val = -dh;

			if(OnClick.CallBack){
				(*OnClick.CallBack)(this, OnClick.Data);
			}

			return true;
		}
		return false;
	}

    size_t val;
private:
    size_t		mLength;
    sf::Shape 	mBackgroundShape;
    sf::Shape 	mDialShape;
    sf::String  mName;

    sf::Rect<int> mDialArea;
};

#endif //#define GUISLIDER_HEADER_GUARD
