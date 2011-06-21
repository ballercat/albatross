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
	MapMaker		*mm;
};

#endif //#define POLYPROP_HEADER_GUARD
