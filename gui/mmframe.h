#ifndef MMFRAME_HEADER_GUARD
#define MMFRAME_HEADER_GUARD

#include"gui.h"
#include"mapmaker.h"
#include"polyprop.h"
#include"vertexprop.h"

////////////////////////////////////////////////////////////
/// Main wxFrame holding everything
////////////////////////////////////////////////////////////
class mmFrame : public wxFrame
{
public:
	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
	mmFrame(wxWindow *p_Parent);

public:
	//Event Handlers
	////////////////////////////////////////////////////////////
	/// On file open
	////////////////////////////////////////////////////////////
	void OnMenuFileOpen(wxCommandEvent &p_Event);

	////////////////////////////////////////////////////////////
	/// On file save
	////////////////////////////////////////////////////////////
	void OnMenuFileSave(wxCommandEvent &p_Event);

	////////////////////////////////////////////////////////////
	/// On exit
	////////////////////////////////////////////////////////////
	void OnMenuFileExit(wxCommandEvent &p_Event);

	////////////////////////////////////////////////////////////
	/// Edit
	////////////////////////////////////////////////////////////
	void OnMenuEditClearMode(wxCommandEvent&);
	void OnMenuDeletePoly(wxCommandEvent&);
	void OnMenuEditSelVert(wxCommandEvent &p_Event);
	void OnMenuEditSelPoly(wxCommandEvent &p_Event);

	////////////////////////////////////////////////////////////
	/// On Idle
	////////////////////////////////////////////////////////////
	void OnIdle(wxIdleEvent& p_Event);

protected:
	DECLARE_EVENT_TABLE();

private:
	MapMaker			*mm;
	wxMenuBar			*mMenuBar;
	wxMenu				*mFileMenu;
	wxMenu				*mEditMenu, *mViewMenu;
	wxMenu				*mSelectSubMenu;
	VertexPropWindow	*mVertexProp;
	PolyPropWindow		*pPolyProp;
};

#endif //#define MMFRAME_HEADER_GUARD
