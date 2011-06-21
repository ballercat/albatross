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
	void OnAddSprite(wxCommandEvent&);
	void OnRemSprite(wxCommandEvent&);
	void OnPlaceSprite(wxCommandEvent&);

public:
	wxListCtrl	*TextureList, *SpriteList;
	wxButton	*pAdd, *pRemove;
	MapMaker	*mm;
};

#endif
