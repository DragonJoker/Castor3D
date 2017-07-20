#include "SceneTreeItemProperty.hpp"

#include <Scene/Scene.hpp>
#include <Texture/TextureLayout.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

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
		: TreeItemProperty( p_scene->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_SCENE )
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

	void SceneTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		SceneSPtr scene = GetScene();

		if ( scene )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SCENE + scene->GetName() ) );
			p_grid->Append( new wxColourProperty( PROPERTY_SCENE_AMBIENT_LIGHT ) )->SetValue( WXVARIANT( wxColour( bgr_packed( scene->GetAmbientLight() ) ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_SCENE_BACKGROUND_COLOUR ) )->SetValue( WXVARIANT( wxColour( bgr_packed( scene->GetBackgroundColour() ) ) ) );

			if ( scene->GetBackgroundImage() )
			{
				p_grid->Append( DoCreateTextureImageProperty( PROPERTY_SCENE_BACKGROUND_COLOUR, scene->GetBackgroundImage() ) );
			}
		}
	}

	void SceneTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		SceneSPtr scene = GetScene();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && scene )
		{
		}
	}

	wxPGProperty * SceneTreeItemProperty::DoCreateTextureImageProperty( wxString const & p_name, Castor3D::TextureLayoutSPtr p_texture )
	{
		wxPGProperty * property = nullptr;

		if ( p_texture->GetImage().IsStaticSource() )
		{
			property = new wxImageFileProperty( PROPERTY_SCENE_BACKGROUND_IMAGE );
		}

		return property;
	}
}
