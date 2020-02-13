#include "GuiCommon/Properties/TreeItems/SceneTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"

#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_SCENE = _( "Scene: " );
		static wxString PROPERTY_SCENE_DEBUG_OVERLAYS = _( "Debug overlays" );
		static wxString PROPERTY_SCENE_AMBIENT_LIGHT = _( "Ambient light" );
	}

	SceneTreeItemProperty::SceneTreeItemProperty( wxWindow * parent
		, bool editable
		, Scene & scene )
		: TreeItemProperty{ scene.getEngine(), editable, ePROPERTY_DATA_TYPE_SCENE }
		, m_parent{ parent }
		, m_scene( scene )
	{
		PROPERTY_CATEGORY_SCENE = _( "Scene: " );
		PROPERTY_SCENE_DEBUG_OVERLAYS = _( "Debug overlays" );
		PROPERTY_SCENE_AMBIENT_LIGHT = _( "Ambient light" );

		CreateTreeItemMenu();
	}

	SceneTreeItemProperty::~SceneTreeItemProperty()
	{
	}

	void SceneTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SCENE + m_scene.getName() ) );
		grid->Append( new wxBoolProperty( PROPERTY_SCENE_DEBUG_OVERLAYS ) )->SetValue( WXVARIANT( m_scene.getEngine()->getRenderLoop().hasDebugOverlays() ) );
		grid->Append( new wxColourProperty( PROPERTY_SCENE_AMBIENT_LIGHT ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( m_scene.getAmbientLight() ) ) ) );
	}

	void SceneTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		wxPGProperty * property = event.GetProperty();

		if ( property )
		{
			if ( property->GetName() == PROPERTY_SCENE_DEBUG_OVERLAYS )
			{
				onDebugOverlaysChange( property->GetValue().GetBool() );
			}
			else if ( property->GetName() == PROPERTY_SCENE_AMBIENT_LIGHT )
			{
				wxColour colour;
				colour << property->GetValue();
				onAmbientLightChange( RgbColour::fromBGR( colour.GetRGB() ) );
			}
		}
	}

	void SceneTreeItemProperty::onDebugOverlaysChange( bool const & value )
	{
		doApplyChange( [value, this]()
		{
			m_scene.getEngine()->getRenderLoop().showDebugOverlays( value );
		} );
	}

	void SceneTreeItemProperty::onAmbientLightChange( castor::RgbColour const & value )
	{
		doApplyChange( [value, this]()
		{
			m_scene.setAmbientLight( value );
		} );
	}
}
