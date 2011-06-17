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

#ifndef GUI_HEADER_GUARD
#define GUI_HEADER_GUARD

#include<wx/wxprec.h>

#ifndef WX_PRECOMP
	#include<wx/wx.h>
#endif

#ifdef __WXGTK__
	#include <gdk/gdkx.h>
	#inlucde <gtk/gtk.h>
	#include <wx/gtk/win_gtk.h>
#endif

#include<wx/minifram.h>
#include<wx/textctrl.h>
#include<wx/stattext.h>
#include<wx/colordlg.h>

#endif //#GUI_HEADER_GUARD
