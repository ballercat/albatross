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

#ifndef MMFRAME_HEADER_GUARD
#define MMFRAME_HEADER_GUARD

#include"gui.h"
#include"mapmaker.h"
#include"polyprop.h"
#include"vertexprop.h"
#include"mapprop.h"

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
	virtual ~mmFrame()
	{
		//void
	}

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
	void OnMenuMapProperties(wxCommandEvent &);
	void OnMenuVertexSnapCheck(wxCommandEvent &);
	void ShowGrid(wxCommandEvent&);
	void AlphaSpawn(wxCommandEvent&);
	void BravoSpawn(wxCommandEvent&);

	////////////////////////////////////////////////////////////
	/// On Idle
	////////////////////////////////////////////////////////////
	void OnIdle(wxIdleEvent& p_Event);

protected:
	DECLARE_EVENT_TABLE();

private:
	MapMaker			*mm;
	VertexPropWindow	*mVertexProp;
	PolyPropWindow		*pPolyProp;
};

#endif //#define MMFRAME_HEADER_GUARD
