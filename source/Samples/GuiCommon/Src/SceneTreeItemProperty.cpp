#include "SceneTreeItemProperty.hpp"

#include <Scene/Scene.hpp>
#include <Texture/StaticTexture.hpp>

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
		SceneSPtr l_scene = GetScene();

		if ( l_scene )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SCENE + l_scene->GetName() ) );
			p_grid->Append( new wxColourProperty( PROPERTY_SCENE_AMBIENT_LIGHT ) )->SetValue( WXVARIANT( wxColour( bgr_packed( l_scene->GetAmbientLight() ) ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_SCENE_BACKGROUND_COLOUR ) )->SetValue( WXVARIANT( wxColour( bgr_packed( l_scene->GetBackgroundColour() ) ) ) );

			if ( l_scene->GetBackgroundImage() )
			{
				p_grid->Append( DoCreateTextureImageProperty( PROPERTY_SCENE_BACKGROUND_COLOUR, l_scene->GetBackgroundImage() ) );
			}
		}
	}

	void SceneTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		SceneSPtr l_scene = GetScene();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_scene )
		{
		}
	}

	wxPGProperty * SceneTreeItemProperty::DoCreateTextureImageProperty( wxString const & p_name, Castor3D::TextureSPtr p_texture )
	{
		wxPGProperty * l_property = nullptr;

		if ( p_texture->GetBaseType() == eTEXTURE_BASE_TYPE_STATIC )
		{
			l_property = new wxImageFileProperty( PROPERTY_SCENE_BACKGROUND_IMAGE );
		}

		return l_property;
	}
}
