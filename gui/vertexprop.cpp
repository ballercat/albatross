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

#include "vertexprop.h"

enum {
	mmID_CHANGECOLORBTN = 9000,
	mmID_REDCOLVERTVAL,
	mmID_GREENCOLVERTVAL,
	mmID_BLUECOLVERTVAL,
	mmID_ALPHACOLVERTVAL,
	mmID_XVALUE,
	mmID_YVALUE
};

////////////////////////////////////////////////////////////
/// ctor
////////////////////////////////////////////////////////////
VertexPropWindow::VertexPropWindow(wxWindow* p_Parent, MapMaker *p_MM) :
	wxMiniFrame(p_Parent,
				wxID_ANY,
				"Vertex Prop",
				wxDefaultPosition,
				wxSize(150,200),
				wxCAPTION | wxCLOSE_BOX )
{
	mm = p_MM;

	//Main Panel
	mMain = new wxPanel(this);{
		//Color value text controls
		{
			pRed	= new VColorTxt(mMain, mmID_REDCOLVERTVAL, wxPoint(50,5));
			pGreen	= new VColorTxt(mMain, mmID_GREENCOLVERTVAL, wxPoint(50, 25));
			pBlue	= new VColorTxt(mMain, mmID_BLUECOLVERTVAL, wxPoint(50,45));
			pAlpha	= new VColorTxt(mMain, mmID_ALPHACOLVERTVAL, wxPoint(50,65));

			//Color labels
			{
				wxStaticText *RedLabel 		= new wxStaticText(mMain, wxID_ANY, wxT("Red"), wxPoint(5,5));
				wxStaticText *GreenLabel	= new wxStaticText(mMain, wxID_ANY, wxT("Green"), wxPoint(5,25));
				wxStaticText *BlueLabel		= new wxStaticText(mMain, wxID_ANY, wxT("Blue"), wxPoint(5,45));
				wxStaticText *AlphaLabel	= new wxStaticText(mMain, wxID_ANY, wxT("Alpha"), wxPoint(5, 65));
			}
		}

		{
			pXValue = new wxTextCtrl(mMain, mmID_XVALUE, _(""), wxPoint(5, 90), wxSize(80, 20));
			pYValue	= new wxTextCtrl(mMain, mmID_YVALUE, _(""), wxPoint(5, 110), wxSize(80, 20));
		}

	}

	Centre();
}

////////////////////////////////////////////////////////////
/// Display
////////////////////////////////////////////////////////////
void VertexPropWindow::Display(bool p_Show)
{
	Show(p_Show);
	if(!p_Show)
		return;

	{
		wxString Rstr, Gstr, Bstr, Astr;

		Rstr.Printf(_("%d"), int(mm->change.vertex_color->r*256));
		Gstr.Printf(_("%d"), int(mm->change.vertex_color->g*256));
		Bstr.Printf(_("%d"), int(mm->change.vertex_color->b*256));
		Astr.Printf(_("%d"), int(mm->change.vertex_color->a*256));

		pRed->SetValue(Rstr);
		pGreen->SetValue(Gstr);
		pBlue->SetValue(Bstr);
		pAlpha->SetValue(Astr);
	}

	{
		wxString Xstr, Ystr;

		Xstr.Printf(_("%f"), mm->change.vertex->x);
		Ystr.Printf(_("%f"), mm->change.vertex->y);

		pXValue->SetValue(Xstr);
		pYValue->SetValue(Ystr);
	}

}

////////////////////////////////////////////////////////////
/// Vertex Properties frame events
////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(VertexPropWindow, wxMiniFrame)
	EVT_TEXT	(mmID_REDCOLVERTVAL,	VertexPropWindow::OnRedChange)
	EVT_TEXT	(mmID_GREENCOLVERTVAL,	VertexPropWindow::OnGreenChange)
	EVT_TEXT	(mmID_BLUECOLVERTVAL,	VertexPropWindow::OnBlueChange)
	EVT_TEXT	(mmID_ALPHACOLVERTVAL,	VertexPropWindow::OnAlphaChange)

	EVT_TEXT	(mmID_XVALUE,	VertexPropWindow::XChange)
	EVT_TEXT	(mmID_YVALUE,	VertexPropWindow::YChange)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
/// X, Y position change
////////////////////////////////////////////////////////////
void VertexPropWindow::XChange(wxCommandEvent &WXUNUSED(p_Event))
{
	static double x = 0.0f;
	pXValue->GetValue().ToDouble(&x);
	mm->change.vertex->x = x;
}

void VertexPropWindow::YChange(wxCommandEvent &WXUNUSED(p_Event))
{
	static double y = 0.0f;
	pYValue->GetValue().ToDouble(&y);
	mm->change.vertex->y = y;
}

////////////////////////////////////////////////////////////
/// Color change events
////////////////////////////////////////////////////////////
void VertexPropWindow::OnRedChange(wxCommandEvent &p_Event)
{
	long value = 0;
	pRed->GetValue().ToLong(&value);

	float color = value/256.0f;
	mm->change.vertex_color->r = color;

	if(mm->change.pPolygon){
		mm->change.pPolygon->pC->data[0].r  = color;
		mm->change.pPolygon->pC->data[1].r  = color;
		mm->change.pPolygon->pC->data[2].r  = color;
	}
}

void VertexPropWindow::OnGreenChange(wxCommandEvent &p_Event)
{
	long value = 0;
	pGreen->GetValue().ToLong(&value);

	float color = value/256.0f;
	mm->change.vertex_color->g = color;

	if(mm->change.pPolygon){
		mm->change.pPolygon->pC->data[0].g  = color;
		mm->change.pPolygon->pC->data[1].g  = color;
		mm->change.pPolygon->pC->data[2].g  = color;
	}
}

void VertexPropWindow::OnBlueChange(wxCommandEvent &p_Event)
{
	long value = 0;
	pBlue->GetValue().ToLong(&value);

	float color = value/256.0f;
	mm->change.vertex_color->b = color;

	if(mm->change.pPolygon){
		mm->change.pPolygon->pC->data[0].b  = color;
		mm->change.pPolygon->pC->data[1].b  = color;
		mm->change.pPolygon->pC->data[2].b  = color;
	}
}

void VertexPropWindow::OnAlphaChange(wxCommandEvent &WXUNUSED(p_Event))
{
	long value = 0;
	pAlpha->GetValue().ToLong(&value);

	float color = value/256.0f;
	mm->change.vertex_color->a = color;

	if(mm->change.pPolygon){
		mm->change.pPolygon->pC->data[0].a  = color;
		mm->change.pPolygon->pC->data[1].a  = color;
		mm->change.pPolygon->pC->data[2].a  = color;
	}
}
