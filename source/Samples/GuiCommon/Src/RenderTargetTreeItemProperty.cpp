#include "RenderTargetTreeItemProperty.hpp"

#include <Render/RenderTarget.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
	}

	RenderTargetTreeItemProperty::RenderTargetTreeItemProperty( bool p_editable, RenderTargetSPtr p_target )
		: TreeItemProperty( p_target->getEngine(), p_editable, ePROPERTY_DATA_TYPE_RENDER_TARGET )
		, m_target( p_target )
	{
		PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );

		CreateTreeItemMenu();
	}

	RenderTargetTreeItemProperty::~RenderTargetTreeItemProperty()
	{
	}

	void RenderTargetTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			wxString TARGETS[] =
			{
				_( "Window" ),
				_( "Texture" )
			};

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_RENDER_TARGET + TARGETS[size_t( target->getTargetType() )] ) );
		}
	}

	void RenderTargetTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		RenderTargetSPtr target = getRenderTarget();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && target )
		{
		}
	}
}
