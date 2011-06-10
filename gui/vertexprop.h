#ifndef VERTEXPROP_HEADER_GUARD
#define VERTEXPROP_HEADER_GURAD

#include"gui.h"
#include"mapmaker.h"

////////////////////////////////////////////////////////////
/// Vertex properties window
////////////////////////////////////////////////////////////
class VertexPropWindow : public wxMiniFrame
{
public:
	////////////////////////////////////////////////////////////
	/// Helper class for Vertex Color values
	////////////////////////////////////////////////////////////
	class VColorTxt: public wxTextCtrl
	{
	public:
		VColorTxt(wxWindow *p_Parent, wxWindowID p_ID, const wxPoint &p_Pos) :
			wxTextCtrl(	p_Parent,
						p_ID,
						wxT(""),
						p_Pos,
						wxSize(30,20))
		{
			//void
		}
	};

public:
	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
	VertexPropWindow(wxWindow* p_Parent);

public:
	////////////////////////////////////////////////////////////
	/// Events
	////////////////////////////////////////////////////////////
	void OnChangeColorButton(wxCommandEvent &p_Event);
	void OnRedChange(wxCommandEvent &p_Event);
	void OnGreenChange(wxCommandEvent &p_Event);
	void OnBlueChange(wxCommandEvent &p_Event);
	void OnAlphaChange(wxCommandEvent&);

protected:

	DECLARE_EVENT_TABLE();

public:
	wxPanel		*mMain;
	wxButton	*mColorButton;
	VColorTxt	*pRed, *pGreen, *pBlue, *pAlpha;

	bgmf_vert	pVert;
};

#endif //#define VERTEXPROP_HEADER_GUARD
