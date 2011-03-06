#include "ImgToXpm/PrecompiledHeader.h"

#include "ImgToXpm/MainFrame.h"
#include "ImgToXpm/ImgToXpm.h"

using namespace ImgToXpm;

DECLARE_APP( ImgToXpmApp)

MainFrame :: MainFrame()
	:	wxFrame( NULL, wxID_ANY, "Image To XPM Converter", wxPoint( 0, 0), wxSize( 400, 300), wxSYSTEM_MENU | wxCLOSE_BOX | wxCAPTION)
{
	CreateStatusBar();

	wxSize l_size = GetClientSize();
	wxPanel * l_pPanel = new wxPanel( this, 0, 0, l_size.x, l_size.y);
	m_pListImages = new wxListBox( l_pPanel, wxID_ANY, wxPoint( 0, 0), wxSize( l_size.x, l_size.y - 45));
	m_pButtonBrowse = new wxButton( l_pPanel, eBrowse, wxT( "Browse"), wxPoint( 10, l_size.y - 35), wxSize( 100, 25));
	m_pButtonProcess = new wxButton( l_pPanel, eProcess, wxT( "Process"), wxPoint( (l_size.x - 100) / 2, l_size.y - 35), wxSize( 100, 25));
	m_pButtonExit = new wxButton( l_pPanel, eExit, wxT( "Exit"), wxPoint( l_size.x - 110, l_size.y - 35), wxSize( 100, 25));
}

MainFrame :: ~MainFrame()
{
}

BEGIN_EVENT_TABLE( MainFrame, wxFrame)
	EVT_BUTTON( eBrowse,	MainFrame::_onBrowse)
	EVT_BUTTON( eProcess,	MainFrame::_onProcess)
	EVT_BUTTON( eExit,		MainFrame::_onExit)
END_EVENT_TABLE()

void MainFrame :: _onBrowse( wxCommandEvent & p_event)
{
	wxFileDialog l_dialog( this, wxT( "Choose an image"), wxEmptyString, wxEmptyString, wxT( "Fichiers BITMAP (*.bmp)|*.bmp|Fichiers GIF (*.gif)|*.gif|Fichiers JPEG (*.jpg)|*.jpg|Fichiers PNG (*.png)|*.png|Toutes Images (*.bmp;*.gif;*.png;*.jpg;*.tga)|*.bmp;*.gif;*.png;*.jpg"), wxFD_DEFAULT_STYLE | wxFD_MULTIPLE);

	if (l_dialog.ShowModal() == wxID_OK)
	{
		wxArrayString l_arrayFiles;
		l_dialog.GetPaths( l_arrayFiles);
		m_pListImages->InsertItems( l_arrayFiles, m_pListImages->GetCount());
	}
}

void MainFrame :: _onProcess( wxCommandEvent & p_event)
{
	wxFileName l_fileName;

	for (size_t i = 0 ; i < m_pListImages->GetCount() ; i++)
	{
		l_fileName = m_pListImages->GetString( i);
		wxImage l_image( l_fileName.GetFullPath());

		if (l_image.IsOk())
		{
			l_image.SaveFile( l_fileName.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + l_fileName.GetName() + wxT( ".xpm"));
		}
	}
}

void MainFrame :: _onExit( wxCommandEvent & p_event)
{
	Close();
}
