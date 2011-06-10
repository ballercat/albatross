#include "mmgui.h"

enum {
	mmID_SELECTVERTEX = 7000,
	mmID_SELECTPOLY,
	mmID_ENABLEVERTSNAP,
	mmID_DELETEPOLY,
	mmID_DELETEVERT,
	mmID_ENABLEGRID,
	mmID_CLEARMODE
};

////////////////////////////////////////////////////////////
/// ctor
////////////////////////////////////////////////////////////
mmFrame::mmFrame(wxWindow *p_Parent) :
wxFrame(p_Parent, wxID_ANY, "wxMapMaker", wxDefaultPosition, wxSize(1024, 670))
{
	//Add canvas
	wxSFMLCanvas *sfmlc = new wxSFMLCanvas(this, wxID_ANY, wxPoint(0, 0), wxSize(1024, 600));
	mm = &sfmlc->mm;

	//Add Status bar
	CreateStatusBar(3);
	SetStatusText(wxT("Normal"), 1);

	//Build Menu bar
	mMenuBar = new wxMenuBar();{
		//File
		mFileMenu = new wxMenu();{
			mFileMenu->Append(wxID_OPEN, _T("&Open"), _T("Open an existing map file"));
			mFileMenu->Append(wxID_SAVE, _T("&Save"), _T("Save the map file"));
			mFileMenu->AppendSeparator();
			mFileMenu->Append(wxID_EXIT, _T("&Exit\tCtrl+Q"), _T("Quit the map maker"));
		}

		//Edit
		mEditMenu = new wxMenu();{
			//Select
			mSelectSubMenu 	= new wxMenu();{
				mSelectSubMenu->Append((mmID_SELECTVERTEX), _T("&Vertex\tCtrl+V"), _T("Select Vertex"));
				mSelectSubMenu->Append((mmID_SELECTPOLY), _T("&Polygon\tCtrl+F"), _T("Select Polygon"));
			}

			mEditMenu->Append(mmID_CLEARMODE, _T("&Clear Mode\tAlt+C"), _T("Set Mode to 'Normal'"));
			mEditMenu->AppendCheckItem((mmID_ENABLEVERTSNAP), _T("&Vertex Snap"), _T("Snap Vertexes Together"));
			mEditMenu->AppendSeparator();
			mEditMenu->Append(mmID_DELETEPOLY, _T("&Delete Poly\tCtrl+Alt+F"), _T("Delete Polygon"));
			mEditMenu->Append(mmID_DELETEVERT, _T("&Delete Vert\tCtrl+Alt+V"), _T("Delete Vertex"));
			mEditMenu->AppendSeparator();
			mEditMenu->AppendSubMenu(mSelectSubMenu, _T("&Select"));
		}

		//View
		mViewMenu = new wxMenu();{
			mViewMenu->AppendCheckItem(mmID_ENABLEGRID, _T("&Show Grid"), _T("Enable Grid"));
		}

		mMenuBar->Append(mFileMenu, _T("&File"));
		mMenuBar->Append(mEditMenu, _T("&Edit"));
		mMenuBar->Append(mViewMenu, _T("&View"));

		SetMenuBar(mMenuBar);
	}

	//Add Vertex Properties window
	mVertexProp = new VertexPropWindow(this);
	mVertexProp->Move(ClientToScreen(wxPoint(1023,0)));
	mVertexProp->Show(false);

	//Add Polygon Properties Window
	pPolyProp	= new PolyPropWindow(this);
	pPolyProp->Move(ClientToScreen(wxPoint(1023,350)));
	pPolyProp->Show(false);
}

////////////////////////////////////////////////////////////
/// Event handlers
////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(mmFrame, wxFrame)
	EVT_MENU(wxID_OPEN, mmFrame::OnMenuFileOpen)
	EVT_MENU(wxID_SAVE, mmFrame::OnMenuFileSave)
	EVT_MENU(wxID_EXIT, mmFrame::OnMenuFileExit)

	EVT_MENU(mmID_CLEARMODE, 	mmFrame::OnMenuEditClearMode)
	EVT_MENU(mmID_DELETEPOLY, 	mmFrame::OnMenuDeletePoly)

	EVT_MENU(mmID_SELECTVERTEX, mmFrame::OnMenuEditSelVert)
	EVT_MENU(mmID_SELECTPOLY, 	mmFrame::OnMenuEditSelPoly)

	EVT_IDLE(mmFrame::OnIdle)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnIdle(wxIdleEvent &p_Event)
{
	wxString str;

	str.Printf(wxT("%f, %f"),mm->mouse.x, mm->mouse.y);

	SetStatusText(str,2);

	if(mEditMenu->IsChecked(mmID_ENABLEVERTSNAP))
		mm->vertex_snap = true;
	else
		mm->vertex_snap = false;

	//Handle Vertex Selection Mode
	if(mm->change == &mm->pick_vertex && mm->change->vertex)
	{
		//Vertex Selected. Show Vertex Properties window

		mVertexProp->pVert.pPos	= mm->change->vertex;
		mVertexProp->pVert.pCol = mm->change->vertex_color;

		mVertexProp->Show(true);

		//Set apropriate values
		str.Printf(wxT("%d"), int(mVertexProp->pVert.pCol->r*256));
		mVertexProp->pRed->SetValue(str);
		str.Printf(wxT("%d"), int(mVertexProp->pVert.pCol->g*256));
		mVertexProp->pGreen->SetValue(str);
		str.Printf(wxT("%d"), int(mVertexProp->pVert.pCol->b*256));
		mVertexProp->pBlue->SetValue(str);
		str.Printf(wxT("%d"), int(mVertexProp->pVert.pCol->a*256));
		mVertexProp->pAlpha->SetValue(str);
	}
	else if(mm->change == &mm->pick_polygon && mm->change->pick_vertex && mm->change->vertex){
		mVertexProp->pVert.pPos	= mm->change->vertex;
		mVertexProp->pVert.pCol	= mm->change->vertex_color;

		mVertexProp->Show();

		//Set apropriate values
		str.Printf(wxT("%d"), int(mVertexProp->pVert.pCol->r*256));
		mVertexProp->pRed->SetValue(str);
		str.Printf(wxT("%d"), int(mVertexProp->pVert.pCol->g*256));
		mVertexProp->pGreen->SetValue(str);
		str.Printf(wxT("%d"), int(mVertexProp->pVert.pCol->b*256));
		mVertexProp->pBlue->SetValue(str);
		str.Printf(wxT("%d"), int(mVertexProp->pVert.pCol->a*256));
		mVertexProp->pAlpha->SetValue(str);

		mm->change->picked	= false;
	}
	else {
		mVertexProp->Show(false);
	}
	//Handle Polygon Selection Mode
	if(mm->change == &mm->pick_polygon && mm->change->poly){
			pPolyProp->pChange	= mm->change;
			//pPolyProp->Move(ClientToScreen(wxPoint(0,350)));
			pPolyProp->Show();
	}
	else {
		pPolyProp->Show(false);
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
		char buffer[100];
		strcpy(buffer, (const char*)OpenMapDialog->GetPath().mb_str(wxConvLibc));

		mm->loadMap(buffer);
	}

	OpenMapDialog->Close();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnMenuFileSave(wxCommandEvent &p_Event)
{
	wxFileDialog *SaveMapDialog	= new wxFileDialog(this, _T("Save map file"), _("./"), _(""), _("*.bgmf*"), wxID_SAVE);
	if( SaveMapDialog->ShowModal() == wxID_OK ){
		wxString path = SaveMapDialog->GetPath();
		char buffer[100];
		strcpy(buffer, (const char*)path.mb_str(wxConvLibc));

		mm->saveMap(buffer);
	}

	SaveMapDialog->Close();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnMenuFileExit(wxCommandEvent &WXUNUSED(p_Event))
{
	Close(true);
}

////////////////////////////////////////////////////////////
/// Clear Mode
////////////////////////////////////////////////////////////
void mmFrame::OnMenuEditClearMode(wxCommandEvent &WXUNUSED(p_Event))
{
	mm->change = NULL;
	SetStatusText(wxT("Normal"), 1);
}

////////////////////////////////////////////////////////////
/// Delete Polygon Mode
////////////////////////////////////////////////////////////
void mmFrame::OnMenuDeletePoly(wxCommandEvent &WXUNUSED(p_Event))
{
	mm->change				= &mm->pick_polygon;
	mm->change->Clear();
	mm->change->pick_poly	= true;
	mm->change->remove		= true;

	SetStatusText(wxT("Delete Polygon"), 1);
}

////////////////////////////////////////////////////////////
/// Select polygon
////////////////////////////////////////////////////////////
void mmFrame::OnMenuEditSelPoly(wxCommandEvent &p_Event)
{
	mm->change			= &mm->pick_polygon;
	mm->change->Clear();
	mm->change->select	= true;

	SetStatusText(wxT("Select Polygon"), 1);
}

////////////////////////////////////////////////////////////
/// Select Vertex
////////////////////////////////////////////////////////////
void mmFrame::OnMenuEditSelVert(wxCommandEvent &p_Event)
{
	mm->change				= &mm->pick_vertex;
	mm->change->Clear();
	mm->change->pick_vertex	= true;

	SetStatusText(wxT("Select Vertex"), 1);
}
