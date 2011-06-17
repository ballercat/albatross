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

	mm.pWorkingDir	= wxGetCwd();
	mm.loadMap("assets/maps/test0.bgmf");
	mm.Init();
}

////////////////////////////////////////////////////////////
/// dtor
////////////////////////////////////////////////////////////
wxSFMLCanvas::~wxSFMLCanvas()
{

}

BEGIN_EVENT_TABLE(wxSFMLCanvas, wxControl)
	EVT_IDLE(wxSFMLCanvas::OnIdle)
	EVT_PAINT(wxSFMLCanvas::OnPaint)
	EVT_ERASE_BACKGROUND(wxSFMLCanvas::OnEraseBackground)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void wxSFMLCanvas::OnIdle(wxIdleEvent&)
{
	Refresh();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void wxSFMLCanvas::OnPaint(wxPaintEvent&)
{
	wxPaintDC Dc(this);

	mm.Step();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void wxSFMLCanvas::OnEraseBackground(wxEraseEvent&)
{

}
