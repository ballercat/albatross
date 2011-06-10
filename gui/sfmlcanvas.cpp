#include "mmgui.h"

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
