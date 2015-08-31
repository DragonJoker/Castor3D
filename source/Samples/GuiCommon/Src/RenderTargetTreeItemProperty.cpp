#include "RenderTargetTreeItemProperty.hpp"

#include <RenderTarget.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
	}

	wxRenderTargetTreeItemProperty::wxRenderTargetTreeItemProperty( RenderTargetSPtr p_target )
		: wxTreeItemProperty( ePROPERTY_DATA_TYPE_RENDER_TARGET )
		, m_target( p_target )
	{
	}

	wxRenderTargetTreeItemProperty::~wxRenderTargetTreeItemProperty()
	{
	}

	void wxRenderTargetTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
	{
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
		}
	}

	void wxRenderTargetTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		RenderTargetSPtr l_target = GetRenderTarget();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_target )
		{
		}
	}
}
