#include "SceneTreeItemProperty.hpp"

#include <Scene.hpp>
#include <StaticTexture.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_SCENE = _( "Scene: " );
		static const wxString PROPERTY_SCENE_AMBIENT_LIGHT = _( "Ambient light" );
		static const wxString PROPERTY_SCENE_BACKGROUND_COLOUR = _( "Background Colour" );
		static const wxString PROPERTY_SCENE_BACKGROUND_IMAGE = _( "Background Image" );
	}

	SceneTreeItemProperty::SceneTreeItemProperty( bool p_editable, SceneSPtr p_scene )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_SCENE )
		, m_scene( p_scene )
	{
	}

	SceneTreeItemProperty::~SceneTreeItemProperty()
	{
	}

	void SceneTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
	{
		SceneSPtr l_scene = GetScene();

		if ( l_scene )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SCENE + l_scene->GetName() ) );
			p_grid->Append( new wxColourProperty( PROPERTY_SCENE_AMBIENT_LIGHT ) )->SetValue( wxVariant( wxColour( l_scene->GetAmbientLight().to_bgr() ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_SCENE_BACKGROUND_COLOUR ) )->SetValue( wxVariant( wxColour( l_scene->GetBackgroundColour().to_bgr() ) ) );

			if ( l_scene->GetBackgroundImage() )
			{
				p_grid->Append( DoCreateTextureImageProperty( PROPERTY_SCENE_BACKGROUND_COLOUR, l_scene->GetBackgroundImage() ) );
			}
		}
	}

	void SceneTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		SceneSPtr l_scene = GetScene();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_scene )
		{
		}
	}

	wxPGProperty * SceneTreeItemProperty::DoCreateTextureImageProperty( wxString const & p_name, Castor3D::TextureBaseSPtr p_texture )
	{
		wxPGProperty * l_property = NULL;

		if ( p_texture->GetType() == eTEXTURE_TYPE_STATIC )
		{
			l_property = new wxImageFileProperty( PROPERTY_SCENE_BACKGROUND_IMAGE );
		}

		return l_property;
	}
}
