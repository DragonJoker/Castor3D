#include "SceneTreeItemProperty.hpp"

#include <Scene/Scene.hpp>
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
		static wxString PROPERTY_SCENE_AMBIENT_LIGHT = _( "Ambient light" );
		static wxString PROPERTY_SCENE_BACKGROUND_COLOUR = _( "Background Colour" );
		static wxString PROPERTY_SCENE_BACKGROUND_IMAGE = _( "Background Image" );
	}

	SceneTreeItemProperty::SceneTreeItemProperty( bool p_editable, SceneSPtr p_scene )
		: TreeItemProperty( p_scene->getEngine(), p_editable, ePROPERTY_DATA_TYPE_SCENE )
		, m_scene( p_scene )
	{
		PROPERTY_CATEGORY_SCENE = _( "Scene: " );
		PROPERTY_SCENE_AMBIENT_LIGHT = _( "Ambient light" );
		PROPERTY_SCENE_BACKGROUND_COLOUR = _( "Background Colour" );
		PROPERTY_SCENE_BACKGROUND_IMAGE = _( "Background Image" );

		CreateTreeItemMenu();
	}

	SceneTreeItemProperty::~SceneTreeItemProperty()
	{
	}

	void SceneTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		SceneSPtr scene = getScene();

		if ( scene )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SCENE + scene->getName() ) );
			p_grid->Append( new wxColourProperty( PROPERTY_SCENE_AMBIENT_LIGHT ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( scene->getAmbientLight() ) ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_SCENE_BACKGROUND_COLOUR ) )->SetValue( WXVARIANT( wxColour( toBGRPacked( scene->getBackgroundColour() ) ) ) );

			if ( scene->getBackgroundImage() )
			{
				p_grid->Append( doCreateTextureImageProperty( PROPERTY_SCENE_BACKGROUND_COLOUR, scene->getBackgroundImage() ) );
			}
		}
	}

	void SceneTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		SceneSPtr scene = getScene();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && scene )
		{
		}
	}

	wxPGProperty * SceneTreeItemProperty::doCreateTextureImageProperty( wxString const & p_name, castor3d::TextureLayoutSPtr p_texture )
	{
		wxPGProperty * property = nullptr;

		if ( p_texture->getImage().isStaticSource() )
		{
			property = new wxImageFileProperty( PROPERTY_SCENE_BACKGROUND_IMAGE );
		}

		return property;
	}
}
