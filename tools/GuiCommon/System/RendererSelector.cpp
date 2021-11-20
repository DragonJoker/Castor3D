#include "GuiCommon/System/RendererSelector.hpp"

#include "GuiCommon/System/ImagesLoader.hpp"
#include "GuiCommon/System/GradientButton.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>

namespace GuiCommon
{
	namespace
	{
		enum IDs
		{
			ID_RENDERERS,
			ID_DEVICES,
		};

		static constexpr int ListHeight = 100;
		static constexpr int ListWidth = 400;
	}

	//*********************************************************************************************

	RendererSelector::RendererSelector( castor3d::Engine & engine
		, wxWindow * parent
		, wxString const & title )
		: wxDialog{ parent, wxID_ANY, title + _( " - Select renderer" ), wxDefaultPosition, wxSize( 500, 400 + ( ListHeight * 2 ) ), wxDEFAULT_DIALOG_STYLE }
		, m_castorImg{ ImagesLoader::getBitmap( CV_IMG_CASTOR ) }
		, m_engine{ engine }
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_renderersList = doFillRenderers();
		m_devicesList = doInitialiseDevices();

		if ( !m_renderersList->IsEmpty() )
		{
			m_renderersList->Select( 0 );
			doSelectRenderer( false );
		}

		wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

		wxBoxSizer * titleSizer = new wxBoxSizer( wxHORIZONTAL );
		titleSizer->Add( 50, 0, 1 );
		auto titleTxt = new wxStaticText( this, wxID_ANY, title );
		titleTxt->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		titleTxt->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		titleTxt->SetFont( wxFont{ 30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) } );
		titleSizer->Add( titleTxt, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		sizer->Add( titleSizer, wxSizerFlags( 0 ).Border( wxALL, 5 ).Expand() );

		wxBoxSizer * descSizer = new wxBoxSizer( wxHORIZONTAL );
		descSizer->Add( 50, 0, 1 );
		auto staticTxt = new wxStaticText( this, wxID_ANY, _( "Select your renderer in the list below" ) );
		staticTxt->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		staticTxt->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		descSizer->Add( staticTxt, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		sizer->Add( descSizer, wxSizerFlags( 0 ).Border( wxALL, 5 ).Expand() );

		wxBoxSizer * buttonSizer = new wxBoxSizer( wxHORIZONTAL );
		GradientButton * ok = new GradientButton( this, wxID_OK, _( "OK" ) );
		buttonSizer->Add( ok, wxSizerFlags( 0 ).Border( wxLEFT, 5 ) );
		buttonSizer->Add( 200, 0, 1 );
		GradientButton * cancel = new GradientButton( this, wxID_CANCEL, _( "Cancel" ) );
		buttonSizer->Add( cancel, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );

		sizer->Add( 0, 60, 0 );
		sizer->Add( m_renderersList, wxSizerFlags( 1 ).Border( wxALL, 10 ).Expand() );
		sizer->Add( m_devicesList, wxSizerFlags( 0 ).Border( wxALL, 10 ).Expand() );
		sizer->Add( buttonSizer, wxSizerFlags( 0 ).Border( wxALL,  5 ).Expand() );
		SetSizer( sizer );
		sizer->SetSizeHints( this );
		wxClientDC clientDC( this );
		doDraw( & clientDC );
	}

	castor3d::Renderer RendererSelector::getSelected()
	{
		castor3d::Renderer result;
		auto selected = uint32_t( m_renderersList->GetSelection() );

		if ( selected < m_renderersList->GetCount() )
		{
			auto it = std::next( m_renderers.begin(), selected );
			result = std::move( *it );
			m_renderers.erase( it );
		}

		return result;
	}

	wxListBox * RendererSelector::doFillRenderers()
	{
		auto result = new wxListBox{ this
			, ID_RENDERERS
			, wxDefaultPosition
			, wxSize{ ListWidth, ListHeight } };
		result->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		result->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		result->Bind( wxEVT_LISTBOX
			, [this]( wxCommandEvent & event )
			{
				doSelectRenderer( false );
				event.Skip();
			} );
		uint32_t count{};

		for ( auto & renderer : m_engine.getRenderersList() )
		{
			m_renderers.emplace_back( m_engine, renderer );
			auto name = make_wxString( renderer.description );
			name.Replace( wxT( " for Ashes" ), wxEmptyString );
			result->Insert( name, count++ );
		}

		return result;
	}

	wxListBox * RendererSelector::doInitialiseDevices()
	{
		auto result = new wxListBox{ this
			, ID_DEVICES
			, wxPoint{ 0, ListHeight }
			, wxSize{ ListWidth, ListHeight } };
		result->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		result->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		result->Bind( wxEVT_LISTBOX
			, [this]( wxCommandEvent & event )
			{
				doSelectDevice( false );
				event.Skip();
			} );
		return result;
	}

	void RendererSelector::doFillDevices( castor3d::Renderer const & renderer )
	{
		uint32_t count{};
		m_devicesList->Clear();

		for ( auto & gpu : renderer.gpus )
		{
			m_devicesList->Insert( gpu->getProperties().deviceName, count++ );
		}

		m_devicesList->Update();
	}

	void RendererSelector::doDraw( wxDC * dc )
	{
		dc->DrawBitmap( *m_castorImg, wxPoint( 0, 0 ), true );
	}

	void RendererSelector::doSelectRenderer( bool next )
	{
		if ( !m_renderers.empty() )
		{
			auto selected = uint32_t( m_renderersList->GetSelection() );

			if ( selected < m_renderersList->GetCount() )
			{
				m_currentRenderer = &m_renderers[selected];
			}

			doFillDevices( *m_currentRenderer );

			if ( m_devicesList->GetCount() > 0 )
			{
				m_devicesList->Select( 0 );
				doSelectDevice( false );
			}

			if ( next )
			{
				m_devicesList->SetFocus();
			}
		}
		else if ( next )
		{
			EndModal( wxID_CANCEL );
		}
	}

	void RendererSelector::doSelectDevice( bool next )
	{
		if ( m_currentRenderer )
		{
			auto selected = uint32_t( m_devicesList->GetSelection() );

			if ( selected < m_currentRenderer->gpus.size() )
			{
				m_currentRenderer->gpu = m_currentRenderer->gpus[selected].get();
			}

			if ( next )
			{
				EndModal( wxID_OK );
			}
		}
			else if ( next )
		{
			EndModal( wxID_CANCEL );
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( RendererSelector, wxDialog )
		EVT_PAINT(	RendererSelector::onPaint )
		EVT_BUTTON(	wxID_OK, RendererSelector::onButtonOk )
		EVT_BUTTON(	wxID_CANCEL, RendererSelector::onButtonCancel )
		EVT_LISTBOX_DCLICK( ID_RENDERERS, RendererSelector::onSelectRenderer )
		EVT_LISTBOX_DCLICK( ID_DEVICES, RendererSelector::onButtonOk )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void RendererSelector::onPaint( wxPaintEvent & event )
	{
		wxPaintDC paintDC( this );
		doDraw( & paintDC );
		event.Skip();
	}

	void RendererSelector::onRenderersKeyUp( wxKeyEvent & event )
	{
		switch ( event.GetKeyCode() )
		{
		case WXK_RETURN:
			doSelectRenderer( false );
			break;
		}

		event.Skip();
	}

	void RendererSelector::onDevicesKeyUp( wxKeyEvent & event )
	{
		switch ( event.GetKeyCode() )
		{
		case WXK_RETURN:
			doSelectDevice( false );
			break;
		}

		event.Skip();
	}

	void RendererSelector::onButtonOk( wxCommandEvent & event )
	{
		doSelectDevice( true );
		event.Skip();
	}

	void RendererSelector::onButtonCancel( wxCommandEvent & event )
	{
		EndModal( wxID_CANCEL );
		event.Skip();
	}

	void RendererSelector::onSelectRenderer( wxCommandEvent & event )
	{
		doSelectRenderer( true );
		event.Skip();
	}
}
