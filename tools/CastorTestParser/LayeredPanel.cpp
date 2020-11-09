#include "CastorTestParser/LayeredPanel.hpp"

namespace test_parser
{
	LayeredPanel::LayeredPanel( wxWindow * parent
		, wxPoint const & position
		, wxSize const & size )
		: wxPanel{ parent, wxID_ANY, position, size }
	{
	}

	void LayeredPanel::addLayer( wxPanel * panel )
	{
		m_panels.push_back( panel );
		panel->Hide();
	}

	void LayeredPanel::showLayer( size_t index )
	{
		hideLayers();
		assert( m_panels.size() > index );
		m_current = m_panels[index];
		m_current->Show();
		wxBoxSizer * sizer = new wxBoxSizer{ wxVERTICAL };
		sizer->Add( m_current, wxSizerFlags( 1 ).Expand() );
		SetSizer( sizer );
		sizer->SetSizeHints( this );
	}

	void LayeredPanel::hideLayers()
	{
		if ( m_current )
		{
			SetSizer( nullptr );
			m_current->Hide();
			m_current = nullptr;
		}
	}
}