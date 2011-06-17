#ifndef POLYPROP_HEADER_GUARD
#define POLYPROP_HEADER_GUARD

#include"gui.h"
#include"mapmaker.h"
#include<wx/choice.h>
#include<string>
#include<vector>

////////////////////////////////////////////////////////////
/// Polygon Properties Window
////////////////////////////////////////////////////////////
class PolyPropWindow : public wxMiniFrame
{
public:
	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
	PolyPropWindow(wxWindow *p_Parent, MapMaker* p_MM);

public:
	////////////////////////////////////////////////////////////
	/// Display(aka Show the window)
	////////////////////////////////////////////////////////////
	void Display(bool p_Show=true);

protected:

	DECLARE_EVENT_TABLE();

public:
	////////////////////////////////////////////////////////////
	/// Events
	////////////////////////////////////////////////////////////
	void OnSelectVerts(wxCommandEvent &p_Event);
	void OnTextureSelect(wxCommandEvent &);
	void SetHollowMask(wxCommandEvent&);

public:
	wxPanel			*pMain;
	wxChoice		*pTextureChoice;
	wxCheckBox 		*HollowChk;
	wxStaticText	*pPolyID;
	std::vector<std::string>	*pTextureNames;
	wxButton		*pSelVertsBtn;
	MapMaker::change_struct	*pChange;
	MapMaker		*mm;
};

#endif //#define POLYPROP_HEADER_GUARD
