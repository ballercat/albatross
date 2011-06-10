#include"polyprop.h"

enum{
	mmID_SELECTTEXTURE = 8000,
	mmID_SELECTPOLYVERTS
};

PolyPropWindow::PolyPropWindow(wxWindow *p_Parent) :
	wxMiniFrame(p_Parent, wxID_ANY, "Poly Properties", wxPoint(0, 300), wxSize(200,250))
{

	pMain	= new wxPanel(this);{
		pSelVertsBtn	= new wxButton(	pMain,
										mmID_SELECTPOLYVERTS,
										wxT("Select Vertices"),
										wxPoint(0,205),
										wxSize(190,20));

		pSelTextureBtn	= new wxButton( pMain,
										mmID_SELECTTEXTURE,
										wxT("Select Texture"),
										wxPoint(0,185),
										wxSize(190,20));
	}

	pTexture.LoadFile("assets/texture/plain.png");
	pTexture.Rescale(50,50);
}

BEGIN_EVENT_TABLE(PolyPropWindow, wxMiniFrame)
	EVT_BUTTON	(mmID_SELECTPOLYVERTS, 	PolyPropWindow::OnSelectVerts)
	EVT_BUTTON	(mmID_SELECTTEXTURE, 	PolyPropWindow::OnSelectTexture)
	EVT_PAINT	(PolyPropWindow::OnPaint)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
/// Paint Event
////////////////////////////////////////////////////////////
void PolyPropWindow::OnPaint(wxPaintEvent &WXUNUSED(p_Event))
{
	wxPaintDC	__dc(this);
	wxPaintDC	DC(pMain);

	DC.DrawBitmap(wxBitmap(pTexture), 5, 5);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void PolyPropWindow::OnSelectVerts(wxCommandEvent &p_Event)
{
	pChange->pick_vertex = !pChange->pick_vertex;
}

////////////////////////////////////////////////////////////
/// On select texture file
////////////////////////////////////////////////////////////
void PolyPropWindow::OnSelectTexture(wxCommandEvent &WXUNUSED(p_Event))
{
	wxFileDialog *ChooseTxtr = new wxFileDialog(this, _T("Choose a texture"), _("./"), _(""), _(""), wxID_OPEN);
	if( ChooseTxtr->ShowModal() == wxID_OK ){
		wxString path = ChooseTxtr->GetPath();
		pTexture.Destroy();
		pTexture.LoadFile(path);
		pTexture.Rescale(50,50);
		Refresh();
	}

	ChooseTxtr->Close();
}
