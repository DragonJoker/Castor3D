#include "ImgToIco/PrecompiledHeader.hpp"

#include "ImgToIco/MainFrame.hpp"
#include "ImgToIco/ImgToIco.hpp"

using namespace ImgToIco;

DECLARE_APP( ImgToIcoApp)

MainFrame :: MainFrame()
	:	wxFrame( NULL, wxID_ANY, wxT( "Image To ICO Converter"), wxPoint( 0, 0), wxSize( 400, 300), wxSYSTEM_MENU | wxCLOSE_BOX | wxCAPTION)
{
	CreateStatusBar();

	wxSize l_size = GetClientSize();
	wxPanel * l_pPanel = new wxPanel( this, wxID_ANY, wxPoint( 0, 0), l_size);
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

	while (m_pListImages->GetCount())
	{
		l_fileName = m_pListImages->GetString( 0);
		m_pListImages->Delete( 0);
		wxImage l_image( l_fileName.GetFullPath());

		if (l_image.IsOk())
		{
			if (l_image.GetWidth() > 64 || l_image.GetHeight() > 64)
			{
				l_image.Rescale( 64, 64, wxIMAGE_QUALITY_HIGH);
			}
			l_image.SaveFile( l_fileName.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + l_fileName.GetName() + wxT( ".ico"));
		}
	}
}

void MainFrame :: _onExit( wxCommandEvent & p_event)
{
	Close();
}
