#ifndef MMGUI_HEADER_GUARD
#define MMGUI_HEADER_GUARD

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

#include"mapmaker.h"
#include"messages.h"

class wxSFMLCanvas : public wxControl
{
public:
	wxSFMLCanvas(wxWindow* p_Parent = NULL,
				 wxWindowID p_Id = -1,
				 const wxPoint& p_Position = wxDefaultPosition,
				 const wxSize& p_Size = wxDefaultSize,
				 long p_Style = 0);

	virtual ~wxSFMLCanvas();

protected:
	DECLARE_EVENT_TABLE()

private:
	void OnIdle(wxIdleEvent&);
	void OnPaint(wxPaintEvent&);
	void OnEraseBackground(wxEraseEvent&);

public:
	MapMaker	mm;

private:
	gfx::FixedPipeline*	mContext;
	Input*				mInput;
	message::Queue		mMessageQueue;
};

////////////////////////////////////////////////////////////
/// Predifined ID's (6000+)
////////////////////////////////////////////////////////////
enum {
	mmID_MOVEVERTEX = 6000,
	mmID_MOVEPOLYGON,
	mmID_SELECTVERTEX,
	mmID_SELECTPOLY,
	mmID_CHANGECOLORBTN
};

////////////////////////////////////////////////////////////
/// Main wxFrame holding everything
////////////////////////////////////////////////////////////
class mmFrame : public wxFrame
{
public:
	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
	mmFrame() :
	wxFrame(NULL, wxID_ANY, "wxMapMaker", wxDefaultPosition, wxSize(1024, 670))
	{
		//Add canvas
		wxSFMLCanvas *sfmlc = new wxSFMLCanvas(this, wxID_ANY, wxPoint(0, 0), wxSize(1024, 600));
		mm = &sfmlc->mm;

		//Add Status bar
		CreateStatusBar(3);
		SetStatusText(wxT("Normal"), 1);

		//Build Menu bar
		mMenuBar = new wxMenuBar();{
			mFileMenu = new wxMenu();{
				mFileMenu->Append(wxID_OPEN, _T("&Open"), _T("Open an existing map file"));
				mFileMenu->Append(wxID_SAVE, _T("&Save"), _T("Save the map file"));
				mFileMenu->AppendSeparator();
				mFileMenu->Append(wxID_EXIT, _T("&Exit\tCtrl+Q"), _T("Quit the map maker"));
			}

			mEditMenu = new wxMenu();{

				//Movement
				mMoveSubMenu = new wxMenu();{
					mMoveSubMenu->Append((mmID_MOVEPOLYGON), _T("&Polygon"), _T("Move a Polygon"));
					mMoveSubMenu->Append((mmID_MOVEVERTEX), _T("&Vertex"), _T("Move a Vertex"));
				}

				//Select
				mSelectSubMenu = new wxMenu();{
					mSelectSubMenu->Append((mmID_SELECTVERTEX), _T("&Vertex\tCtrl+Alt+V"), _T("Select Vertex"));
					mSelectSubMenu->Append((mmID_SELECTPOLY), _T("&Polygon\tCtrl+Alt+P"), _T("Select Polygon"));
				}

				mEditMenu->AppendSubMenu(mSelectSubMenu, _T("&Select"));
				mEditMenu->AppendSubMenu(mMoveSubMenu, _T("&Movement"));
			}

			mMenuBar->Append(mFileMenu, _T("&File"));
			mMenuBar->Append(mEditMenu, _T("&Edit"));

			SetMenuBar(mMenuBar);
		}

		//Add Vertex Properties window
		mVertexProp = new VertexPropWindow(this);
		mVertexProp->Show(false);
	}

public:
	////////////////////////////////////////////////////////////
	/// Vertex properties window
	////////////////////////////////////////////////////////////
	class VertexPropWindow : public wxMiniFrame
	{
	public:
		////////////////////////////////////////////////////////////
		/// ctor
		////////////////////////////////////////////////////////////
		VertexPropWindow(wxWindow* p_Parent) :
			wxMiniFrame(p_Parent,
						wxID_ANY,
						"Vertex Prop",
						wxDefaultPosition,
						wxSize(150,200),
						wxCAPTION | wxCLOSE_BOX )
		{
			//Main Panel
			mMain = new wxPanel(this);{
				mColorButton = new wxButton(mMain, mmID_CHANGECOLORBTN, wxT("Change Color"));

			}
		}

	public:
		////////////////////////////////////////////////////////////
		/// Events
		////////////////////////////////////////////////////////////
		void OnChangeColorButton(wxCommandEvent &p_Event);

	protected:

		DECLARE_EVENT_TABLE();

	public:
		wxPanel		*mMain;
		wxButton	*mColorButton;
		bgmf_vert	pVert;

	};

public:
	//Event Handlers
	////////////////////////////////////////////////////////////
	/// On file open
	////////////////////////////////////////////////////////////
	void OnMenuFileOpen(wxCommandEvent &p_Event);

	////////////////////////////////////////////////////////////
	/// On file save
	////////////////////////////////////////////////////////////
	void OnMenuFileSave(wxCommandEvent &p_Event);

	////////////////////////////////////////////////////////////
	/// On exit
	////////////////////////////////////////////////////////////
	void OnMenuFileExit(wxCommandEvent &p_Event);

	////////////////////////////////////////////////////////////
	/// Movement
	////////////////////////////////////////////////////////////
	void OnMenuEditMovePoly(wxCommandEvent &p_Event);
	void OnMenuEditMoveVert(wxCommandEvent &p_Event);

	////////////////////////////////////////////////////////////
	/// Select
	////////////////////////////////////////////////////////////
	void OnMenuEditSelVert(wxCommandEvent &p_Event);
	void OnMenuEditSelPoly(wxCommandEvent &p_Event);

	////////////////////////////////////////////////////////////
	/// On Idle
	////////////////////////////////////////////////////////////
	void OnIdle(wxIdleEvent& p_Event);

protected:
	DECLARE_EVENT_TABLE();

private:
	MapMaker*			mm;
	wxMenuBar*			mMenuBar;
	wxMenu*				mFileMenu;
	wxMenu*				mEditMenu;
	wxMenu*				mMoveSubMenu, *mSelectSubMenu;
	VertexPropWindow*	mVertexProp;
};

class mmApplication : public wxApp
{
	virtual bool OnInit()
	{
		mmFrame* MapMakerFrame = new mmFrame;
		MapMakerFrame->Show();

		return true;
	}
};

DECLARE_APP(mmApplication);

#endif //#define MMGUI_HEADER_GUARD
