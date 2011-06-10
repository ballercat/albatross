#include "vertexprop.h"

enum {
	mmID_CHANGECOLORBTN = 9000,
	mmID_REDCOLVERTVAL,
	mmID_GREENCOLVERTVAL,
	mmID_BLUECOLVERTVAL,
	mmID_ALPHACOLVERTVAL
};

////////////////////////////////////////////////////////////
/// ctor
////////////////////////////////////////////////////////////
VertexPropWindow::VertexPropWindow(wxWindow* p_Parent) :
	wxMiniFrame(p_Parent,
				wxID_ANY,
				"Vertex Prop",
				wxDefaultPosition,
				wxSize(150,200),
				wxCAPTION | wxCLOSE_BOX )
{
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

		//Change Color with a color Dialog
		mColorButton = new wxButton(mMain, mmID_CHANGECOLORBTN, wxT("Change Color"), wxPoint(5,80));
	}

	Centre();
}

////////////////////////////////////////////////////////////
/// Vertex Properties frame events
////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(VertexPropWindow, wxMiniFrame)
	EVT_BUTTON	(mmID_CHANGECOLORBTN,	VertexPropWindow::OnChangeColorButton)
	EVT_TEXT	(mmID_REDCOLVERTVAL,	VertexPropWindow::OnRedChange)
	EVT_TEXT	(mmID_GREENCOLVERTVAL,	VertexPropWindow::OnGreenChange)
	EVT_TEXT	(mmID_BLUECOLVERTVAL,	VertexPropWindow::OnBlueChange)
	EVT_TEXT	(mmID_ALPHACOLVERTVAL,	VertexPropWindow::OnAlphaChange)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
/// Change the color for the vertex with a wxColorDialog
////////////////////////////////////////////////////////////
void VertexPropWindow::OnChangeColorButton(wxCommandEvent &p_Event)
{
	wxColourDialog			*ChangeColorDlg;
	wxColourData 	ColorData; ColorData.SetChooseFull(true);

	// Convert the existing color of the vertex to wxColour & wxColourData
	//wxColor Color((uint8_t)pVert.pCol->r, (uint8_t)pVert.pCol->g, (uint8_t)pVert.pCol->b, (uint8_t)pVert.pCol->a);
	wxColor Color(256*pVert.pCol->r,256*pVert.pCol->g,256*pVert.pCol->b,256*pVert.pCol->a);

	ColorData.SetCustomColour(0,Color);

	//Create the Dialog
	ChangeColorDlg 	= new wxColourDialog(this, &ColorData);

	if(ChangeColorDlg->ShowModal() == wxID_OK){
		//Convert the new color back into vertex color
		glm::vec4		NewColor;

		Color	 	= ChangeColorDlg->GetColourData().GetColour();

		NewColor.r	= Color.Red()/256.0f;
		NewColor.g	= Color.Green()/256.0f;
		NewColor.b	= Color.Blue()/256.0f;
		NewColor.a	= Color.Alpha()/256.0f;

		*(pVert.pCol)	= NewColor;
	}

	//Destroy the dialog
	ChangeColorDlg->Destroy();
}

void VertexPropWindow::OnRedChange(wxCommandEvent &p_Event)
{
	long value = 0;
	pRed->GetValue().ToLong(&value);

	pVert.pCol->r = value/256.0f;
}

void VertexPropWindow::OnGreenChange(wxCommandEvent &p_Event)
{
	long value = 0;
	pGreen->GetValue().ToLong(&value);

	pVert.pCol->g = value/256.0f;
}

void VertexPropWindow::OnBlueChange(wxCommandEvent &p_Event)
{
	long value = 0;
	pBlue->GetValue().ToLong(&value);

	pVert.pCol->b = value/256.0f;
}

void VertexPropWindow::OnAlphaChange(wxCommandEvent &WXUNUSED(p_Event))
{
	long value = 0;
	pAlpha->GetValue().ToLong(&value);

	pVert.pCol->a = value/256.0f;
}
