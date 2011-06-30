#ifndef MMGUI_HEADER_GUARD
#define MMGUI_HEADER_GUARD

#include"gui.h"
#include"mmframe.h"
#include"messages.h"

////////////////////////////////////////////////////////////
/// Canvas to enable OpenGL/SFML
////////////////////////////////////////////////////////////
class wxSFMLCanvas : public wxControl
{
public:
	wxSFMLCanvas(wxWindow* p_Parent = NULL,
				 wxWindowID p_Id = -1,
				 const wxPoint& p_Position = wxDefaultPosition,
				 const wxSize& p_Size = wxDefaultSize,
				 long p_Style = 0);

	virtual ~wxSFMLCanvas();

protected:
	DECLARE_EVENT_TABLE()

private:
	void OnIdle(wxIdleEvent&);
	void OnPaint(wxPaintEvent&);
	void OnEraseBackground(wxEraseEvent&);

public:
	MapMaker	mm;

private:
	gfx::Core*			mContext;
	Input*				mInput;
	message::Queue		mMessageQueue;
};

////////////////////////////////////////////////////////////
/// Main App
////////////////////////////////////////////////////////////
class mmApplication : public wxApp
{
	virtual bool OnInit()
	{
		wxInitAllImageHandlers();

		mmFrame* MapMakerFrame = new mmFrame(GetTopWindow());
		MapMakerFrame->Show();

		return true;
	}
};

DECLARE_APP(mmApplication);

#endif //#define MMGUI_HEADER_GUARD
