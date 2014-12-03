#include "MainFrame.hpp"
#include "ImgConverter.hpp"

using namespace ImgToIco;

DECLARE_APP( ImgToIcoApp )

MainFrame::MainFrame()
	:	wxFrame( NULL, wxID_ANY, _( "Image To ICO/XPM Converter" ), wxPoint( 0, 0 ), wxSize( 400, 300 ), wxSYSTEM_MENU | wxCLOSE_BOX | wxCAPTION )
{
	CreateStatusBar();
	wxSize l_size = GetClientSize();
//	wxPanel * l_pPanel	= new wxPanel( this, wxID_ANY, wxPoint( 0, 0), l_size);
	m_pListImages		= new wxListBox(	this,	wxID_ANY,						wxDefaultPosition,	wxSize( l_size.x, l_size.y - 45 ) );
	m_pButtonBrowse		= new wxButton(	this,	eBrowse,	_( "Browse"	)	);
	m_pButtonProcess	= new wxButton(	this,	eProcess,	_( "Process"	)	);
	m_pButtonExit		= new wxButton(	this,	eExit,		_( "Exit"	)	);
	wxBoxSizer * l_pSizer		= new wxBoxSizer( wxVERTICAL	);
	wxBoxSizer * l_pSizerBtn	= new wxBoxSizer( wxHORIZONTAL	);
	l_pSizerBtn->Add(	m_pButtonBrowse,	wxSizerFlags( 0 ).Border( wxALL,	 5 ).Left()	);
	l_pSizerBtn->Add(	5, 0,				1	);
	l_pSizerBtn->Add(	m_pButtonProcess,	wxSizerFlags( 0 ).Border( wxALL,	 5 ).Center()	);
	l_pSizerBtn->Add(	5, 0,				1	);
	l_pSizerBtn->Add(	m_pButtonExit,		wxSizerFlags( 0 ).Border( wxALL,	 5 ).Right()	);
	l_pSizer->Add(	m_pListImages,		wxSizerFlags( 1 ).Expand()	);
	l_pSizer->Add(	l_pSizerBtn,		wxSizerFlags( 0 ).Expand()	);
	SetSizer( l_pSizer );
	l_pSizer->SetSizeHints( this );
}

MainFrame::~MainFrame()
{
}

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
	EVT_BUTTON( eBrowse,	MainFrame::_onBrowse )
	EVT_BUTTON( eProcess,	MainFrame::_onProcess )
	EVT_BUTTON( eExit,		MainFrame::_onExit )
END_EVENT_TABLE()

void MainFrame::_onBrowse( wxCommandEvent & p_event )
{
	wxFileDialog l_dialog( this, _( "Choose an image" ), wxEmptyString, wxEmptyString, _( "BITMAP Images (*.bmp)|*.bmp|GIF Images (*.gif)|*.gif|JPEG Images (*.jpg)|*.jpg|PNG Images (*.png)|*.png|TARGA Images (*.tga)|*.tga|All Images (*.bmp;*.gif;*.png;*.jpg;*.tga)|*.bmp;*.gif;*.png;*.jpg" ), wxFD_DEFAULT_STYLE | wxFD_MULTIPLE );

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		wxArrayString l_arrayFiles;
		l_dialog.GetPaths( l_arrayFiles );
		m_pListImages->InsertItems( l_arrayFiles, m_pListImages->GetCount() );
	}

	p_event.Skip();
}

void MainFrame::_onProcess( wxCommandEvent & p_event )
{
	wxFileName l_fileName;
	wxArrayString l_arrayChoices;
	wxString l_strExt;
	l_arrayChoices.push_back( wxT( "ico" ) );
	l_arrayChoices.push_back( wxT( "xpm" ) );
	l_strExt = wxGetSingleChoice( _( "Select output image format" ), _( "Image format" ), l_arrayChoices );

	if ( !l_strExt.empty() )
	{
		while ( m_pListImages->GetCount() )
		{
			l_fileName = m_pListImages->GetString( 0 );
			m_pListImages->Delete( 0 );
			wxImage l_image( l_fileName.GetFullPath() );
			SetStatusText( _( "Processing : " ) + l_fileName.GetFullPath() );

			if ( l_image.IsOk() )
			{
				if ( ( l_image.GetWidth() > 64 || l_image.GetHeight() > 64 ) && l_strExt == wxT( "ico" ) )
				{
					l_image.Rescale( 64, 64, wxIMAGE_QUALITY_HIGH );
				}

				l_image.SaveFile( l_fileName.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR ) + l_fileName.GetName() + wxT( "." ) + l_strExt );
			}
		}

		SetStatusText( _( "Done" ) );
	}

	p_event.Skip();
}

void MainFrame::_onExit( wxCommandEvent & p_event )
{
	Close();
	p_event.Skip();
}
