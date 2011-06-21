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

#include"mapprop.h"
#include<wx/bookctrl.h>

enum{
	mmID_ADDTEXTURE = 50,
	mmID_REMTEXTURE,
	mmID_ADDSPRITE,
	mmID_REMSPRITE,
	mmID_PLCSPRITE
};

////////////////////////////////////////////////////////////
/// Map settings dialog
////////////////////////////////////////////////////////////
IMPLEMENT_CLASS(MapSettingsDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(MapSettingsDialog, wxPropertySheetDialog)
	EVT_BUTTON(mmID_ADDTEXTURE, MapSettingsDialog::OnAddTexture)
	EVT_BUTTON(mmID_REMTEXTURE, MapSettingsDialog::OnRemoveTexture)
	EVT_BUTTON(mmID_ADDSPRITE, MapSettingsDialog::OnAddSprite)
	EVT_BUTTON(mmID_PLCSPRITE, MapSettingsDialog::OnPlaceSprite)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////
/// ctor
////////////////////////////////////////////////////////////
MapSettingsDialog::MapSettingsDialog(wxWindow* p_Parent, MapMaker *p_MM)
{
	Create(p_Parent, wxID_ANY, _("Map Preferences"), wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE|wxNO_3D);

	CreateButtons(wxOK);

	wxBookCtrlBase	*notebook = GetBookCtrl();

	mm = p_MM;

	wxPanel *pMain = new wxPanel(this);{
		TextureList = new wxListCtrl(pMain, wxID_ANY, wxPoint(5,15), wxSize(200,100), wxLC_REPORT);
		{
			wxListItem TextureName;{
				TextureName.SetId(0);
				TextureName.SetText(_("File"));
				TextureName.SetWidth(80);
			}

			wxListItem TextureID;{
				TextureID.SetId(1);
				TextureID.SetText(_("Texture ID"));
				TextureID.SetWidth(115);
			}

			TextureList->InsertColumn(0, TextureName);
			TextureList->InsertColumn(1, TextureID);

			for(size_t i = 0;i < mm->map->texpath.size();i++){
				wxString 	id;
				wxListItem item;

				id.Printf(_("%d"), i);
				item.SetId(i);
				item.SetText(mm->map->texpath[i]);

				TextureList->InsertItem(item);
				TextureList->SetItem(i, 1, id);
			}
		}

		pAdd 	= new wxButton(pMain, mmID_ADDTEXTURE, _("Add Texture"), wxPoint(205, 10), wxDefaultSize);
		pRemove = new wxButton(pMain, mmID_REMTEXTURE, _("Remove Texture"), wxPoint(205,35), wxDefaultSize);
	}

	wxPanel *pSprite = new wxPanel(this);{
		SpriteList	= new wxListCtrl(pSprite, wxID_ANY, wxPoint(5,15), wxSize(200,100), wxLC_REPORT);
		{
			wxListItem SpriteName;{
				SpriteName.SetId(0);
				SpriteName.SetText(_("File"));
				SpriteName.SetWidth(80);
			}

			wxListItem SpriteID;{
				SpriteID.SetId(1);
				SpriteID.SetText(_("Sprite ID"));
				SpriteID.SetWidth(115);
			}

			SpriteList->InsertColumn(0, SpriteName);
			SpriteList->InsertColumn(1, SpriteID);

			for(int i=0;i<mm->map->sprpath.size();i++){
				wxString id;
				wxListItem item;

				id.Printf(_("%d"), i);
				item.SetId(i);
				item.SetText(mm->map->sprpath[i]);

				SpriteList->InsertItem(item);
				SpriteList->SetItem(i, 1, id);
			}
		}

		wxButton *AddSprite	= new wxButton(pSprite, mmID_ADDSPRITE, _("Add Sprite"), wxPoint(205, 10), wxDefaultSize);
		wxButton *RemSprite	= new wxButton(pSprite, mmID_REMSPRITE, _("Remove Sprite"), wxPoint(205, 35), wxDefaultSize);
		wxButton *PlaceSprite = new wxButton(pSprite, mmID_PLCSPRITE, _("Place Sprite"), wxPoint(205, 60), wxDefaultSize);
	}

	notebook->AddPage(pMain, _("General"));
	notebook->AddPage(pSprite, _("Sprites"));

	LayoutDialog();

	Centre();
}

////////////////////////////////////////////////////////////
/// Add Texture
////////////////////////////////////////////////////////////
void MapSettingsDialog::OnAddTexture(wxCommandEvent &WXUNUSED(p_Event))
{
	wxFileDialog *ChooseTexture = new wxFileDialog(this, _("Open Texture File"), _("./"), _(""), _(""), wxID_OPEN);
	if( ChooseTexture->ShowModal() == wxID_OK ){
		wxString fpath = ChooseTexture->GetFilename();

		mm->map->texpath.push_back(std::string(fpath.ToAscii()));
		mm->loadTextureData();

		int spot = mm->map->texpath.size() - 1;
		wxListItem item;
		wxString id;
		id.Printf(_("%d"), spot);
		item.SetId(spot);
		item.SetText(fpath);

		TextureList->InsertItem(item);
		TextureList->SetItem(spot, 1, id);
	}

	ChooseTexture->Close();
	Refresh();
}

void MapSettingsDialog::OnAddSprite(wxCommandEvent &WXUNUSED(p_Event))
{
	wxFileDialog *OpenSprite = new wxFileDialog(this, _("Choose A Sprite"), _("./"), _(""), _(""), wxID_OPEN);
	if( OpenSprite->ShowModal() == wxID_OK ){
		wxString fpath = OpenSprite->GetFilename();
		std::string header(fpath.ToAscii());

		mm->map->sprpath.push_back(std::string(fpath.ToAscii()));

		//build header on the fly
		header = header.substr(0, header.find('.'));
		header = header + ".sprh";
		mm->map->sprheader.push_back(header);

		mm->loadSpriteData();

		int spot = mm->map->sprpath.size() - 1;
		wxListItem item;
		wxString id;
		id.Printf(_("%d"), spot);
		item.SetId(spot);
		item.SetText(fpath);

		SpriteList->InsertItem(item);
		SpriteList->SetItem(spot, 1, id);
	}

	OpenSprite->Close();
	Refresh();
}

////////////////////////////////////////////////////////////
/// Remove Texture
////////////////////////////////////////////////////////////
void MapSettingsDialog::OnRemoveTexture(wxCommandEvent &WXUNUSED(p_Event))
{
	long itemIndex = -1;
	itemIndex = TextureList->GetNextItem(itemIndex,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);

	if( itemIndex > -1 ){
		TextureList->DeleteItem(itemIndex);

		mm->map->texpath.erase( mm->map->texpath.begin() + itemIndex );
		mm->loadTextureData();
	}
}

////////////////////////////////////////////////////////////
/// Place sprite on the map
////////////////////////////////////////////////////////////
void MapSettingsDialog::OnPlaceSprite(wxCommandEvent &WXUNUSED(p_Event))
{
	long itemIndex = -1;
	itemIndex = SpriteList->GetNextItem(itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	if( itemIndex > -1 ){
		mm->change.Clear();

		mm->map->sprite.push_back(bgmf_sprite(mm->mouse,itemIndex));
		mm->map->header.sprc++;
		mm->loadSpriteData();
	}
}
