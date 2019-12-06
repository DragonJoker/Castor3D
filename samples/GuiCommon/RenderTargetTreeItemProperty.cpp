#include "GuiCommon/RenderTargetTreeItemProperty.hpp"

#include "GuiCommon/AdditionalProperties.hpp"
#include "GuiCommon/ShaderDialog.hpp"

#include <Castor3D/Render/RenderTarget.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
		static wxString PROPERTY_RENDER_TARGET_SHADER = _( "Shader" );
		static wxString PROPERTY_RENDER_TARGET_EDIT_SHADER = _( "View Shaders..." );
	}

	RenderTargetTreeItemProperty::RenderTargetTreeItemProperty( bool editable
		, RenderTargetSPtr target )
		: TreeItemProperty( target->getEngine(), editable, ePROPERTY_DATA_TYPE_RENDER_TARGET )
		, m_target( target )
	{
		PROPERTY_CATEGORY_RENDER_TARGET = _( "Render Target: " );
		PROPERTY_RENDER_TARGET_SHADER = _( "Shader" );
		PROPERTY_RENDER_TARGET_EDIT_SHADER = _( "View Shaders..." );

		CreateTreeItemMenu();
	}

	RenderTargetTreeItemProperty::~RenderTargetTreeItemProperty()
	{
	}

	void RenderTargetTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			wxString TARGETS[] =
			{
				_( "Window" ),
				_( "Texture" )
			};

			grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_RENDER_TARGET + TARGETS[size_t( target->getTargetType() )] ) );

			for ( auto & postEffect : target->getHDRPostEffects() )
			{
				grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_RENDER_TARGET + TARGETS[size_t( target->getTargetType() )] ) );
			}
		}
	}

	void RenderTargetTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		RenderTargetSPtr target = getRenderTarget();
		wxPGProperty * property = event.GetProperty();

		if ( property && target )
		{
		}
	}
}
