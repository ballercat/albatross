#include "mmgui.h"
#include "inputhandler.h"
#include <cstdio>

////////////////////////////////////////////////////////////
/// ctor
////////////////////////////////////////////////////////////
wxSFMLCanvas::wxSFMLCanvas(wxWindow* p_Parent,
							wxWindowID p_Id,
							const wxPoint& p_Position,
							const wxSize& p_Size,
							long p_Style) :
wxControl(p_Parent, p_Id, p_Position, p_Size, p_Style)

{
	#ifdef __WXGTK__
		gtk_widget realize(m_wxwindow);
		gtk_widget_set_double_buffered(m_wxwindow, false);
		GdkWindow* Win = GTK_PIZZA(m_wxwindow)->bin_window;
		XFlush(GDK_WINDOW_XDISPLAY(Win));
		mContext = new gfx::FixedPipeline(GDK_WINDOW_XWINDOW(Win));
	#else
		mContext = new gfx::FixedPipeline(GetHandle());
	#endif

	LoadTextures("textures.ini", mContext->Texture);

	mInput = new InputHandlerMixin<Input>(&mMessageQueue);
	mInput->Window = mContext->Window;

	mm.display = mContext;
	mm.input = mInput;

	mm.loadMap("assets/maps/test.bgmf");
}

////////////////////////////////////////////////////////////
/// dtor
////////////////////////////////////////////////////////////
wxSFMLCanvas::~wxSFMLCanvas()
{

}


