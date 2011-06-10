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
