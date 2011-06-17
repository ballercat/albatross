#include"polyprop.h"
#include<cassert>

enum{
	mmID_SELECTTEXTURE = 8000,
	mmID_SELECTPOLYVERTS,
	mmID_HOLLOW
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

		pTextureChoice = new wxChoice(pMain, mmID_SELECTTEXTURE, wxPoint(0, 25));
		mm = p_MM;

		HollowChk 	= new wxCheckBox(pMain, mmID_HOLLOW, _("Hollow"), wxPoint(0, 45));
		pPolyID		= new wxStaticText(pMain, wxID_ANY, _(""));//, wxPoint(20, 5));
	}

}

BEGIN_EVENT_TABLE(PolyPropWindow, wxMiniFrame)
	EVT_BUTTON	(mmID_SELECTPOLYVERTS, 	PolyPropWindow::OnSelectVerts)
	EVT_CHOICE	(mmID_SELECTTEXTURE, 	PolyPropWindow::OnTextureSelect)
	EVT_CHECKBOX(mmID_HOLLOW, 			PolyPropWindow::SetHollowMask)
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

void PolyPropWindow::SetHollowMask(wxCommandEvent &WXUNUSED(p_Event))
{
	unsigned int mask = *(mm->change->pPolygon->pM);
	if(HollowChk->IsChecked()){
		mask = mask ^ POLY_SOLID;
	}
	else {
		mask = mask & POLY_SOLID;
	}

	*mm->change->pPolygon->pM = mask;
}

////////////////////////////////////////////////////////////
/// Fill in polygon data, display polygon window
////////////////////////////////////////////////////////////
void PolyPropWindow::Display(bool p_Show)
{
	if(!p_Show){
		Show(false);
		return;
	}

	assert(!mm);

	//Set ID label
	wxString str;
	str.Printf(_("Polygon ID: %d"), mm->change->pPolygon->pID);
	pPolyID->SetLabel(str);
	pPolyID->CenterOnParent(wxHORIZONTAL);

	//Set texture file choice
	pTextureChoice->Clear();
	for(size_t i=0;i<mm->map->texpath.size();i++)
		pTextureChoice->Insert(_(mm->map->texpath[i].c_str()), i);
	pTextureChoice->SetSelection(*mm->change->pPolygon->pT);

	//Set Mask choices
	if((*mm->change->pPolygon->pM) & POLY_SOLID){
		HollowChk->SetValue(false);
	}
	else {
		HollowChk->SetValue(true);
	}

	Show();
}

