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
		if ( index != m_layer || !m_current )
		{
			hideLayers();
			assert( m_panels.size() > index );
			m_current = m_panels[index];
			m_current->Show();
			m_layer = index;
			wxBoxSizer * sizer = new wxBoxSizer{ wxVERTICAL };
			sizer->Add( m_current, wxSizerFlags( 1 ).Expand() );
			SetSizer( sizer );
			sizer->SetSizeHints( this );
		}
	}

	void LayeredPanel::hideLayers()
	{
		if ( m_current )
		{
			SetSizer( nullptr );
			m_current->Hide();
			m_current = nullptr;
			m_layer = ~( 0u );
		}
	}

	bool LayeredPanel::isLayerShown( size_t index )const
	{
		return m_current
			&& m_layer == index;
	}
}