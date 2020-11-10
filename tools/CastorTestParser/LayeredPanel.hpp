/*
See LICENSE file in root folder
*/
#ifndef ___CTP_LayeredPanel_HPP___
#define ___CTP_LayeredPanel_HPP___

#include "Prerequisites.hpp"

#include <wx/panel.h>
#include <wx/sizer.h>

namespace test_parser
{
	class LayeredPanel
		: public wxPanel
	{
	public:
		LayeredPanel( wxWindow * parent
			, wxPoint const & position
			, wxSize const & size );

		void addLayer( wxPanel * panel );
		void showLayer( size_t index );
		void hideLayers();
		bool isLayerShown( size_t index )const;

	private:
		std::vector< wxPanel * > m_panels;
		wxPanel * m_current{};
		size_t m_layer{ ~( 0u ) };
	};
}

#endif
