#ifndef MAPPROP_HEADER_GUARD
#define MAPPROP_HEADER_GUARD

#include"gui.h"
#include"mapmaker.h"
#include<wx/listctrl.h>
#include<wx/propdlg.h>

class MapSettingsDialog : public wxPropertySheetDialog
{
DECLARE_CLASS(MapSettingsDialog)
public:
	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
	MapSettingsDialog(wxWindow *p_Parent, MapMaker *p_MM);

protected:
	DECLARE_EVENT_TABLE();

public:
	////////////////////////////////////////////////////////////
	/// Events
	////////////////////////////////////////////////////////////
	void OnAddTexture(wxCommandEvent&);
	void OnRemoveTexture(wxCommandEvent&);

public:
	wxListCtrl	*TextureList;
	wxButton	*pAdd, *pRemove;
	MapMaker	*mm;
};

#endif
