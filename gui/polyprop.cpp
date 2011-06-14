#include"polyprop.h"
#include<cassert>

enum{
	mmID_SELECTTEXTURE = 8000,
	mmID_SELECTPOLYVERTS
};

PolyPropWindow::PolyPropWindow(wxWindow *p_Parent, MapMaker *p_MM) :
	wxMiniFrame(p_Parent, wxID_ANY, "Poly Properties", wxPoint(0, 300), wxSize(200,250))
{

	pMain	= new wxPanel(this);{
		pSelVertsBtn	= new wxButton(	pMain,
										mmID_SELECTPOLYVERTS,
										wxT("Select Vertices"),
										wxPoint(0,205),
										wxSize(190,20));

		pTextureChoice = new wxChoice(pMain, mmID_SELECTTEXTURE);
		mm = p_MM;

	}

}

BEGIN_EVENT_TABLE(PolyPropWindow, wxMiniFrame)
	EVT_BUTTON	(mmID_SELECTPOLYVERTS, 	PolyPropWindow::OnSelectVerts)
	EVT_CHOICE	(mmID_SELECTTEXTURE, PolyPropWindow::OnTextureSelect)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
/// Select Vertexes that belong to this polygon
////////////////////////////////////////////////////////////
void PolyPropWindow::OnSelectVerts(wxCommandEvent &p_Event)
{
	pChange->pick_vertex = !pChange->pick_vertex;
}

////////////////////////////////////////////////////////////
/// Select Texture for the polygon
////////////////////////////////////////////////////////////
void PolyPropWindow::OnTextureSelect(wxCommandEvent &WXUNUSED(p_Event))
{
	//void
	*mm->change->pPolygon->pT = pTextureChoice->GetSelection();
	mm->FixTC(mm->change->pPolygon->pTC, *mm->change->pPolygon->pT);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void PolyPropWindow::Display(bool p_Show)
{
	if(!p_Show){
		Show(false);
		return;
	}

	assert(!mm);

	pTextureChoice->Clear();
	for(size_t i=0;i<mm->map->texpath.size();i++)
		pTextureChoice->Insert(_(mm->map->texpath[i].c_str()), i);
	pTextureChoice->SetSelection(*mm->change->pPolygon->pT);

	Show();
}
