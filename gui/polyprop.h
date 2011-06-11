#ifndef POLYPROP_HEADER_GUARD
#define POLYPROP_HEADER_GUARD

#include"gui.h"
#include"mapmaker.h"

////////////////////////////////////////////////////////////
/// Polygon Properties Window
////////////////////////////////////////////////////////////
class PolyPropWindow : public wxMiniFrame
{
public:
	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
	PolyPropWindow(wxWindow *p_Parent);

protected:

	DECLARE_EVENT_TABLE();

public:
	////////////////////////////////////////////////////////////
	/// Events
	////////////////////////////////////////////////////////////
	void OnSelectVerts(wxCommandEvent &p_Event);
	void OnSelectTexture(wxCommandEvent &);
	void OnPaint(wxPaintEvent &p_Event);

public:
	wxPanel			*pMain;
	wxButton		*pSelTextureBtn;
	wxButton		*pSelVertsBtn;
	wxImage			pTexture;
	bgmf_poly		pPoly;
	MapMaker::change_struct	*pChange;
	MapMaker		*mm;
};

#endif //#define POLYPROP_HEADER_GUARD
