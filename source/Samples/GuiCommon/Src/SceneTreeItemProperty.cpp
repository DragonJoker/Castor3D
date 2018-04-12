#include "SceneTreeItemProperty.hpp"

#include <Render/RenderLoop.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Background/Image.hpp>
#include <Scene/Background/Skybox.hpp>
#include <Texture/TextureLayout.hpp>

#include "AdditionalProperties.hpp"
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
		static wxString PROPERTY_SCENE_BACKGROUND_COLOUR = _( "Background Colour" );
		static wxString PROPERTY_SCENE_BACKGROUND_IMAGE = _( "Background Image" );
	}

	SceneTreeItemProperty::SceneTreeItemProperty( bool editable, Scene & scene )
		: TreeItemProperty( scene.getEngine(), editable, ePROPERTY_DATA_TYPE_SCENE )
		, m_scene( scene )
	{
		PROPERTY_CATEGORY_SCENE = _( "Scene: " );
		PROPERTY_SCENE_DEBUG_OVERLAYS = _( "Debug overlays" );
		PROPERTY_SCENE_AMBIENT_LIGHT = _( "Ambient light" );
		PROPERTY_SCENE_BACKGROUND_COLOUR = _( "Background Colour" );
		PROPERTY_SCENE_BACKGROUND_IMAGE = _( "Background Image" );

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
		grid->Append( new wxColourProperty( PROPERTY_SCENE_BACKGROUND_COLOUR ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( m_scene.getBackgroundColour() ) ) ) );

		//if ( m_scene.hasBackgroundImage() )
		//{
		//	grid->Append( doCreateTextureImageProperty( PROPERTY_SCENE_BACKGROUND_IMAGE, m_scene.getBackgroundImage() ) );
		//}
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
			else if ( property->GetName() == PROPERTY_SCENE_BACKGROUND_COLOUR )
			{
				wxColour colour;
				colour << property->GetValue();
				onBackgroundColourChange( RgbColour::fromBGR( colour.GetRGB() ) );
			}
			else if ( property->GetName() == PROPERTY_SCENE_BACKGROUND_IMAGE )
			{
				onBackgroundImageChange( make_String( property->GetValueAsString() ) );
			}
		}
	}

	wxPGProperty * SceneTreeItemProperty::doCreateTextureImageProperty( wxString const & name
		, castor3d::TextureLayout const & texture )
	{
		wxPGProperty * property = nullptr;

		if ( texture.getImage().isStaticSource() )
		{
			property = new wxImageFileProperty( PROPERTY_SCENE_BACKGROUND_IMAGE );
		}

		return property;
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

	void SceneTreeItemProperty::onBackgroundColourChange( castor::RgbColour const & value )
	{
		doApplyChange( [value, this]()
		{
			m_scene.setBackgroundColour( value );
		} );
	}

	void SceneTreeItemProperty::onBackgroundImageChange( castor::String const & value )
	{
		if ( File::fileExists( Path{ value } ) )
		{
			doApplyChange( [this, value]()
			{
				static_cast< ImageBackground & >( m_scene.getBackground() ).loadImage( Path{}, Path{ value } );
			} );
		}
		else
		{
			wxMessageBox( _( "Image does not exist" ) );
		}
	}
}
