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
	VertexPropWindow(wxWindow* p_Parent, MapMaker *p_MM);

public:
	////////////////////////////////////////////////////////////
	/// Display Window
	////////////////////////////////////////////////////////////
	void Display(bool p_Show = true);

public:
	////////////////////////////////////////////////////////////
	/// Events
	////////////////////////////////////////////////////////////
	void XValUp(wxCommandEvent&);
	void XValDown(wxCommandEvent&);
	void YValUp(wxCommandEvent&);
	void YValDown(wxCommandEvent&);
	void XChange(wxCommandEvent&);
	void YChange(wxCommandEvent&);
	void OnRedChange(wxCommandEvent &p_Event);
	void OnGreenChange(wxCommandEvent &p_Event);
	void OnBlueChange(wxCommandEvent &p_Event);
	void OnAlphaChange(wxCommandEvent&);

protected:

	DECLARE_EVENT_TABLE();

public:
	wxPanel		*mMain;
	VColorTxt	*pRed, *pGreen, *pBlue, *pAlpha;
	wxTextCtrl	*pXValue, *pYValue;

	MapMaker	*mm;
};

#endif //#define VERTEXPROP_HEADER_GUARD
