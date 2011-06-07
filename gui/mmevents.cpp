#include "mmgui.h"
#include <wx/colordlg.h>

BEGIN_EVENT_TABLE(wxSFMLCanvas, wxControl)
	EVT_IDLE(wxSFMLCanvas::OnIdle)
	EVT_PAINT(wxSFMLCanvas::OnPaint)
	EVT_ERASE_BACKGROUND(wxSFMLCanvas::OnEraseBackground)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void wxSFMLCanvas::OnIdle(wxIdleEvent&)
{
	Refresh();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void wxSFMLCanvas::OnPaint(wxPaintEvent&)
{
	wxPaintDC Dc(this);

	mm.Step();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void wxSFMLCanvas::OnEraseBackground(wxEraseEvent&)
{

}

////////////////////////////////////////////////////////////
/// Event handlers
////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(mmFrame, wxFrame)
	EVT_MENU(wxID_OPEN, mmFrame::OnMenuFileOpen)
	EVT_MENU(wxID_SAVE, mmFrame::OnMenuFileSave)
	EVT_MENU(wxID_EXIT, mmFrame::OnMenuFileExit)

	EVT_MENU(mmID_MOVEVERTEX, mmFrame::OnMenuEditMoveVert)
	EVT_MENU(mmID_MOVEPOLYGON, mmFrame::OnMenuEditMovePoly)

	EVT_MENU(mmID_SELECTVERTEX, mmFrame::OnMenuEditSelVert)
	EVT_MENU(mmID_SELECTPOLY, mmFrame::OnMenuEditSelPoly)

	EVT_IDLE(mmFrame::OnIdle)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnIdle(wxIdleEvent &p_Event)
{
	wxString str;

	str.Printf(wxT("%f, %f"),mm->mouse.x, mm->mouse.y);

	SetStatusText(str,2);

	if(mm->change == &mm->pick_vertex && mm->change->vertex)
	{
		mVertexProp->pVert.pPos	= mm->change->vertex;
		mVertexProp->pVert.pCol = mm->change->vertex_color;

		mVertexProp->Show(true);
	}
	else{
		mVertexProp->Show(false);
	}
}

/*
 ////////////////////////////////////////////////////////////
 * Menu Event Handlers
 ////////////////////////////////////////////////////////////
*/

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnMenuFileOpen(wxCommandEvent &p_Event)
{
	wxFileDialog *OpenMapDialog = new wxFileDialog(this, _T("Choose a map"), _("./"), _(""), _("*.bgmf*"), wxID_OPEN);
	if( OpenMapDialog->ShowModal() == wxID_OK ){
		char buffer[1000];
		strcpy(buffer, (const char*)OpenMapDialog->GetPath().mb_str(wxConvLibc));

		mm->loadMap(buffer);
	}

	OpenMapDialog->Close();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnMenuFileSave(wxCommandEvent &p_Event)
{

}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnMenuFileExit(wxCommandEvent &WXUNUSED(p_Event))
{
	DestroyChildren();
	Close(true);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnMenuEditMovePoly(wxCommandEvent &p_Event)
{
	if(mm->change != &mm->move_poly){
		mm->change 			= &mm->move_poly;
		mm->change->poly 	= NULL;
		mm->change->color 	= false;
		mm->change->remove	= false;

		SetStatusText(wxT("Move Polygon"), 1);
	}
	else {
		mm->change->poly	= NULL;
		mm->change			= NULL;

		SetStatusText(wxT("Normal"),1);
	}
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnMenuEditMoveVert(wxCommandEvent &p_Event)
{
	if(mm->change != &mm->move_vertex){
		mm->change			= &mm->move_vertex;
		mm->change->poly	= NULL;
		mm->change->color	= false;
		mm->change->remove	= false;
		mm->change->vertex	= NULL;

		SetStatusText(wxT("Move Vertex"), 1);
	}
}

////////////////////////////////////////////////////////////
/// Select polygon
////////////////////////////////////////////////////////////
void mmFrame::OnMenuEditSelPoly(wxCommandEvent &p_Event)
{

}

////////////////////////////////////////////////////////////
/// Select Vertex
////////////////////////////////////////////////////////////
void mmFrame::OnMenuEditSelVert(wxCommandEvent &p_Event)
{
	if(mm->change != &mm->pick_vertex){
		mm->change			= &mm->pick_vertex;
		mm->change->vertex	= NULL;
		mm->change->picked	= false;

		SetStatusText(wxT("Select Vertex"), 1);
	}
	else {
		mm->change->picked	= false;
		mm->change			= NULL;

		SetStatusText(wxT("Normal"), 1);
	}
}


////////////////////////////////////////////////////////////
/// Vertex Properties frame events
////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(mmFrame::VertexPropWindow, wxMiniFrame)
	EVT_BUTTON	(mmID_CHANGECOLORBTN,	mmFrame::VertexPropWindow::OnChangeColorButton)
END_EVENT_TABLE()

void mmFrame::VertexPropWindow::OnChangeColorButton(wxCommandEvent &p_Event)
{
	static wxColourDialog	ChangeColorDlg;

	if(ChangeColorDlg.ShowModal() == wxID_OK){
		wxColourData 	ColorData = ChangeColorDlg.GetColourData();
		glm::vec4		NewColor;

		NewColor.r	= ColorData.GetColour().Red();
		NewColor.g	= ColorData.GetColour().Green();
		NewColor.b	= ColorData.GetColour().Blue();
		NewColor.a	= ColorData.GetColour().Alpha();
	}
}
