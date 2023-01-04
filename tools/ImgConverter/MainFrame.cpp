#include "ImgConverter/MainFrame.hpp"
#include "ImgConverter/ImgConverter.hpp"

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
#pragma warning( pop )

namespace ImgToIco
{
	MainFrame::MainFrame()
		: wxFrame( nullptr, wxID_ANY, _( "Image To ICO/XPM Converter" ), wxPoint( 0, 0 ), wxSize( 400, 300 ), wxSYSTEM_MENU | wxCLOSE_BOX | wxCAPTION )
	{
		CreateStatusBar();
		wxSize size = GetClientSize();
		m_pListImages = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( size.x, size.y - 45 ) );
		m_pButtonBrowse = new wxButton( this, eBrowse, _( "Browse" ) );
		m_pButtonProcess = new wxButton( this, eProcess, _( "Process" ) );
		m_pButtonExit = new wxButton( this, eExit, _( "Exit" ) );
		wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );
		wxBoxSizer * sizerBtn = new wxBoxSizer( wxHORIZONTAL );
		sizerBtn->Add( m_pButtonBrowse, wxSizerFlags( 0 ).Border( wxALL, 5 ).Left() );
		sizerBtn->Add( 5, 0, 1 );
		sizerBtn->Add( m_pButtonProcess, wxSizerFlags( 0 ).Border( wxALL, 5 ).Center() );
		sizerBtn->Add( 5, 0, 1 );
		sizerBtn->Add( m_pButtonExit, wxSizerFlags( 0 ).Border( wxALL, 5 ).Right() );
		sizer->Add( m_pListImages, wxSizerFlags( 1 ).Expand() );
		sizer->Add( sizerBtn, wxSizerFlags( 0 ).Expand() );
		SetSizer( sizer );
		sizer->SetSizeHints( this );
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_BUTTON( eBrowse, MainFrame::_onBrowse )
		EVT_BUTTON( eProcess, MainFrame::_onProcess )
		EVT_BUTTON( eExit, MainFrame::_onExit )
		END_EVENT_TABLE()
#pragma GCC diagnostic pop

		void MainFrame::_onBrowse( wxCommandEvent & event )
	{
		wxFileDialog dialog( this
			, _( "Choose an image" )
			, wxEmptyString
			, wxEmptyString
			, _( "BITMAP Images (*.bmp)|*.bmp|GIF Images (*.gif)|*.gif|JPEG Images (*.jpg)|*.jpg|PNG Images (*.png)|*.png|TARGA Images (*.tga)|*.tga|All Images (*.bmp;*.gif;*.png;*.jpg;*.tga)|*.bmp;*.gif;*.png;*.jpg" )
			, wxFD_DEFAULT_STYLE | wxFD_MULTIPLE );

		if ( dialog.ShowModal() == wxID_OK )
		{
			wxArrayString arrayFiles;
			dialog.GetPaths( arrayFiles );
			m_pListImages->InsertItems( arrayFiles, m_pListImages->GetCount() );
		}

		event.Skip();
	}

	void MainFrame::_onProcess( wxCommandEvent & event )
	{
		wxArrayString arrayChoices;
		wxString extension;
		arrayChoices.push_back( wxT( "ico" ) );
		arrayChoices.push_back( wxT( "xpm" ) );
		extension = wxGetSingleChoice( _( "Select output image format" ), _( "Image format" ), arrayChoices );

		if ( !extension.empty() )
		{
			while ( m_pListImages->GetCount() )
			{
				wxFileName fileName = m_pListImages->GetString( 0 );
				m_pListImages->Delete( 0 );
				wxImage image( fileName.GetFullPath() );
				SetStatusText( _( "Processing : " ) + fileName.GetFullPath() );

				if ( image.IsOk() )
				{
					if ( ( image.GetWidth() > 64 || image.GetHeight() > 64 ) && extension == wxT( "ico" ) )
					{
						image.Rescale( 64, 64, wxIMAGE_QUALITY_HIGH );
					}

					image.SaveFile( fileName.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR ) + fileName.GetName() + wxT( "." ) + extension );
				}
			}

			SetStatusText( _( "Done" ) );
		}

		event.Skip();
	}

	void MainFrame::_onExit( wxCommandEvent & event )
	{
		Close();
		event.Skip();
	}
}
