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

#include "mmgui.h"

enum {
	mmID_SELECTVERTEX = 7000,
	mmID_SELECTPOLY,
	mmID_SELECTSPRITE,
	mmID_ENABLEVERTSNAP,
	mmID_DELETEPOLY,
	mmID_DELETEVERT,
	mmID_ENABLEGRID,
	mmID_CLEARMODE,
	mmID_MAPPROPERTIES,
	mmID_ALPHASPAWN,
	mmID_BRAVOSPAWN,
};

////////////////////////////////////////////////////////////
/// ctor
////////////////////////////////////////////////////////////
mmFrame::mmFrame(wxWindow *p_Parent) :
wxFrame(p_Parent, wxID_ANY, "wxMapMaker", wxDefaultPosition, wxSize(1024, 675))
{
	//Add canvas
	wxSFMLCanvas *sfmlc = new wxSFMLCanvas(this, wxID_ANY, wxPoint(0, 0), wxSize(1024, 600));
	mm = &sfmlc->mm;

	//Add Status bar
	CreateStatusBar(3);
	SetStatusText(wxT("Normal"), 1);

	//Build Menu bar
	wxMenuBar *mMenuBar = new wxMenuBar();{
		wxMenu				*mEditMenu;
		wxMenu				*mFileMenu;
		wxMenu				*mViewMenu;
		wxMenu				*mMapMenu;
		wxMenu				*mSelectSubMenu;

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
				mSelectSubMenu->Append((mmID_SELECTSPRITE), _("&Sprite\tCtrl+S"), _("Select Sprite"));
			}

			mEditMenu->Append(mmID_CLEARMODE, _T("&Clear Mode\tAlt+C"), _T("Set Mode to 'Normal'"));
			mEditMenu->AppendCheckItem(mmID_ENABLEVERTSNAP, _T("&Vertex Snap"), _T("Snap Vertexes Together"));
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

		mMapMenu = 	new wxMenu();{
			mMapMenu->Append((mmID_MAPPROPERTIES), _T("&Properties\tCtr+M"), _T("Edit Map Properties"));
			mMapMenu->AppendSeparator();

			wxMenu *SpawnSubMenu = new wxMenu();{
				SpawnSubMenu->Append(mmID_ALPHASPAWN, _("&Alpha"), _("Set Alpha Spawn"));
				SpawnSubMenu->Append(mmID_BRAVOSPAWN, _("&Bravo"), _("Set Bravo Spawn"));
			}

			mMapMenu->AppendSubMenu(SpawnSubMenu, _("Spawn"));
		}

		mMenuBar->Append(mFileMenu, _T("&File"));
		mMenuBar->Append(mEditMenu, _T("&Edit"));
		mMenuBar->Append(mViewMenu, _T("&View"));
		mMenuBar->Append(mMapMenu, _T("&Map"));

		SetMenuBar(mMenuBar);
	}

	//Add Vertex Properties window
	mVertexProp = new VertexPropWindow(this, mm);
	mVertexProp->Move(ClientToScreen(wxPoint(1023,0)));
	mVertexProp->Show(false);

	//Add Polygon Properties Window
	pPolyProp	= new PolyPropWindow(this, mm);
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
	EVT_MENU(mmID_SELECTSPRITE,	mmFrame::OnSelectSprite)
	EVT_MENU(mmID_ENABLEVERTSNAP, mmFrame::OnMenuVertexSnapCheck)

	EVT_MENU(mmID_ENABLEGRID, mmFrame::ShowGrid)

	EVT_MENU(mmID_MAPPROPERTIES, mmFrame::OnMenuMapProperties)

	EVT_MENU(mmID_ALPHASPAWN, mmFrame::AlphaSpawn)
	EVT_MENU(mmID_BRAVOSPAWN, mmFrame::BravoSpawn)

	EVT_IDLE(mmFrame::OnIdle)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void mmFrame::OnIdle(wxIdleEvent &p_Event)
{
	wxString str;

	str.Printf(wxT("%f, %f"),mm->mouse.x, mm->mouse.y);

	SetStatusText(str,2);

	//Handle Vertex Selection Mode
	if(mm->change.vertex)
	{
		//Vertex Selected. Show Vertex Properties window
		mVertexProp->Display();
	}
	/*else if(mm->change.vertex){
		mVertexProp->Display();

		mm->change.picked	= false;
	}*/
	else {
		mVertexProp->Display(false);
	}
	//Handle Polygon Selection Mode
	if(mm->change.pick_poly && mm->change.picked && mm->change.pPolygon){
			pPolyProp->Display(true);
			mm->change.picked = false;
	}
	else if(mm->change.pick_poly && !mm->change.pPolygon){
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
		mm->loadTextureData();
		mm->loadSpriteData();
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
	mm->change.Clear();
	SetStatusText(wxT("Normal"), 1);
}

////////////////////////////////////////////////////////////
/// Delete Polygon Mode
////////////////////////////////////////////////////////////
void mmFrame::OnMenuDeletePoly(wxCommandEvent &WXUNUSED(p_Event))
{
	mm->change.Clear();
	mm->change.pick_poly	= true;
	mm->change.remove		= true;

	SetStatusText(wxT("Delete Polygon"), 1);
}

////////////////////////////////////////////////////////////
/// Select polygon
////////////////////////////////////////////////////////////
void mmFrame::OnMenuEditSelPoly(wxCommandEvent &p_Event)
{
	mm->change.Clear();
	mm->change.select	= true;
	mm->change.pick_poly = true;

	SetStatusText(wxT("Select Polygon"), 1);
}

////////////////////////////////////////////////////////////
/// Select Vertex
////////////////////////////////////////////////////////////
void mmFrame::OnMenuEditSelVert(wxCommandEvent &p_Event)
{
	mm->change.Clear();
	mm->change.pick_vertex	= true;

	SetStatusText(wxT("Select Vertex"), 1);
}

////////////////////////////////////////////////////////////
/// Select Sprite
////////////////////////////////////////////////////////////
void mmFrame::OnSelectSprite(wxCommandEvent &WXUNUSED(p_Event))
{
	mm->change.Clear();
	mm->change.pick_sprite = true;

	SetStatusText(_("Select Sprite"), 1);
}

////////////////////////////////////////////////////////////
/// Show grid checkbox
////////////////////////////////////////////////////////////
void mmFrame::ShowGrid(wxCommandEvent &WXUNUSED(p_Event))
{
	mm->drawgrid = !mm->drawgrid;
}

////////////////////////////////////////////////////////////
/// Check vertex snap
////////////////////////////////////////////////////////////
void mmFrame::OnMenuVertexSnapCheck(wxCommandEvent &WXUNUSED(p_Event))
{
	mm->vertex_snap = !mm->vertex_snap;
}

////////////////////////////////////////////////////////////
/// Show Map properties
////////////////////////////////////////////////////////////
void mmFrame::OnMenuMapProperties(wxCommandEvent &WXUNUSED(p_Event))
{
	MapSettingsDialog *MapSettings = new MapSettingsDialog(this, mm);
	MapSettings->Show();
}

void mmFrame::AlphaSpawn(wxCommandEvent &WXUNUSED(p_Event))
{
	mm->change.Clear();
	mm->change.spawna = true;
	mm->change.pick_spawn = true;
	SetStatusText(_("Place Alpha Spawn"),1);
}

void mmFrame::BravoSpawn(wxCommandEvent &WXUNUSED(p_Event))
{
	mm->change.Clear();
	mm->change.pick_spawn;
	mm->change.spawnb = true;
	SetStatusText(_("Place Bravo Spawn"),1);
}
