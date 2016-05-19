#include "RenderTargetTreeItemProperty.hpp"

#include <Render/RenderTarget.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
	}

	RenderTargetTreeItemProperty::RenderTargetTreeItemProperty( bool p_editable, RenderTargetSPtr p_target )
		: TreeItemProperty( p_target->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_RENDER_TARGET )
		, m_target( p_target )
	{
		PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );

		CreateTreeItemMenu();
	}

	RenderTargetTreeItemProperty::~RenderTargetTreeItemProperty()
	{
	}

	void RenderTargetTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			wxString TARGETS[] =
			{
				_( "Window" ),
				_( "Texture" )
			};

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_RENDER_TARGET + TARGETS[l_target->GetTargetType()] ) );
		}
	}

	void RenderTargetTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		RenderTargetSPtr l_target = GetRenderTarget();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_target )
		{
		}
	}
}
