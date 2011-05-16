#ifndef GUIWIDGETPANEL_HEADER_GUARD
#define GUIWIDGETPANEL_HEADER_GUARD

#include "widget.h"


////////////////////////////////////////////////////////////
/// Panel Widget
////////////////////////////////////////////////////////////
struct guiPanel : public guiWidget
{
public:
	guiPanel(size_t id, int p_X, int p_Y, size_t p_Length, guiWidget* p_Parent = NULL);
	virtual bool update(Input* in);
	virtual ~guiPanel();

public:
	////////////////////////////////////////////////////////////
	/// Widget Adding methods
	////////////////////////////////////////////////////////////
	size_t addSlider(int p_X, int p_Y, size_t p_Length);
	size_t addButton(glm::vec2 p_Loc, std::string p_Text);
	size_t addTextInput(glm::vec2 p_Loc, glm::vec2 p_Dim, const char* p_Text=NULL);

public:
	////////////////////////////////////////////////////////////
	/// Set Callbacks
	////////////////////////////////////////////////////////////
	void SetWidgetCallback(size_t p_WID, void (*Callback)(guiWidget*,void*), void *p_Data);

public:
	//Movement data
	glm::vec2	pLastLoc;

private:
	sf::Rect<int>			mArea;
	std::vector<guiWidget*>	mChild;
};

#endif //GUIWIDGETPANLE_HEADER_GUARD
