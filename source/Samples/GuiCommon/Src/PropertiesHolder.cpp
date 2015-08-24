#include "PropertiesHolder.hpp"

#include "CameraTreeItemData.hpp"
#include "GeometryTreeItemData.hpp"
#include "LightTreeItemData.hpp"
#include "NodeTreeItemData.hpp"
#include "OverlayTreeItemData.hpp"
#include "SubmeshTreeItemData.hpp"
#include "MaterialTreeItemData.hpp"
#include "PassTreeItemData.hpp"
#include "TextureTreeItemData.hpp"

#include "AdditionalProperties.hpp"

#include <Camera.hpp>
#include <Geometry.hpp>
#include <Submesh.hpp>
#include <Light.hpp>
#include <DirectionalLight.hpp>
#include <PointLight.hpp>
#include <SpotLight.hpp>
#include <SceneNode.hpp>
#include <Overlay.hpp>
#include <PanelOverlay.hpp>
#include <BorderPanelOverlay.hpp>
#include <TextOverlay.hpp>
#include <Material.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>

#include <MaterialManager.hpp>
#include <Engine.hpp>
#include <Font.hpp>

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	wxPropertiesHolder::wxPropertiesHolder( bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxPropertyGrid( p_pParent, wxID_ANY, p_ptPos, p_size, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE )
		, m_bCanEdit( p_bCanEdit )
	{
		SetBackgroundColour( *wxWHITE );
	}

	wxPropertiesHolder::~wxPropertiesHolder()
	{
	}

	void wxPropertiesHolder::SetPropertyData( wxTreeItemPropertyData * p_data )
	{
		if ( p_data )
		{
			wxPropertyGrid::Clear();
			m_dataType = p_data->GetType();

			switch ( p_data->GetType() )
			{
			case ePROPERTY_DATA_TYPE_CAMERA:
				DoSetCameraProperties( reinterpret_cast< wxCameraTreeItemData * >( p_data ) );
				break;

			case ePROPERTY_DATA_TYPE_GEOMETRY:
				DoSetGeometryProperties( reinterpret_cast< wxGeometryTreeItemData * >( p_data ) );
				break;

			case ePROPERTY_DATA_TYPE_SUBMESH:
				DoSetSubmeshProperties( reinterpret_cast< wxSubmeshTreeItemData * >( p_data ) );
				break;

			case ePROPERTY_DATA_TYPE_LIGHT:
				DoSetLightProperties( reinterpret_cast< wxLightTreeItemData * >( p_data ) );
				break;

			case ePROPERTY_DATA_TYPE_NODE:
				DoSetNodeProperties( reinterpret_cast< wxNodeTreeItemData * >( p_data ) );
				break;

			case ePROPERTY_DATA_TYPE_OVERLAY:
				DoSetOverlayProperties( reinterpret_cast< wxOverlayTreeItemData * >( p_data ) );
				break;

			case ePROPERTY_DATA_TYPE_MATERIAL:
				DoSetMaterialProperties( reinterpret_cast< wxMaterialTreeItemData * >( p_data ) );
				break;

			case ePROPERTY_DATA_TYPE_PASS:
				DoSetPassProperties( reinterpret_cast< wxPassTreeItemData * >( p_data ) );
				break;

			case ePROPERTY_DATA_TYPE_TEXTURE:
				DoSetTextureProperties( reinterpret_cast< wxTextureTreeItemData * >( p_data ) );
				break;
			}
		}
	}

	void wxPropertiesHolder::DoSetCameraProperties( wxCameraTreeItemData * p_data )
	{
		wxPGChoices l_topologies;
		l_topologies.Add( _( "Points" ) );
		l_topologies.Add( _( "Lines" ) );
		l_topologies.Add( _( "Line Loop" ) );
		l_topologies.Add( _( "Line Strip" ) );
		l_topologies.Add( _( "Triangles" ) );
		l_topologies.Add( _( "Triangle Strip" ) );
		l_topologies.Add( _( "Triangle Fan" ) );
		l_topologies.Add( _( "Quads" ) );
		l_topologies.Add( _( "Quad Strip" ) );
		l_topologies.Add( _( "Polygon" ) );
		wxString l_selected;

		switch ( p_data->GetCamera()->GetPrimitiveType() )
		{
		case eTOPOLOGY_POINTS:
			l_selected = _( "Points" );
			break;

		case eTOPOLOGY_LINES:
			l_selected = _( "Lines" );
			break;

		case eTOPOLOGY_LINE_LOOP:
			l_selected = _( "Line Loop" );
			break;

		case eTOPOLOGY_LINE_STRIP:
			l_selected = _( "Line Strip" );
			break;

		case eTOPOLOGY_TRIANGLES:
			l_selected = _( "Triangles" );
			break;

		case eTOPOLOGY_TRIANGLE_STRIPS:
			l_selected = _( "Triangle Strip" );
			break;

		case eTOPOLOGY_TRIANGLE_FAN:
			l_selected = _( "Triangle Fan" );
			break;

		case eTOPOLOGY_QUADS:
			l_selected = _( "Quads" );
			break;

		case eTOPOLOGY_QUAD_STRIPS:
			l_selected = _( "Quad Strip" );
			break;

		case eTOPOLOGY_POLYGON:
			l_selected = _( "Polygon" );
			break;
		}

		Append( new wxEnumProperty( _( "Topology" ), _( "Topology" ), l_topologies ) )->SetValue( l_selected );

		wxPGChoices l_viewports;
		l_viewports.Add( _( "3D" ) );
		l_viewports.Add( _( "2D" ) );

		switch ( p_data->GetCamera()->GetViewportType() )
		{
		case eVIEWPORT_TYPE_3D:
			l_selected = _( "3D" );
			break;

		case eVIEWPORT_TYPE_2D:
			l_selected = _( "2D" );
			break;
		}

		Append( new wxEnumProperty( _( "Viewport" ), _( "Viewport" ), l_viewports ) )->SetValue( l_selected );
		Append( new SizeProperty( _( "Size" ) ) )->SetValue( wxVariant( Size( p_data->GetCamera()->GetWidth(), p_data->GetCamera()->GetHeight() ) ) );
	}

	void wxPropertiesHolder::DoSetGeometryProperties( wxGeometryTreeItemData * p_data )
	{
	}

	void wxPropertiesHolder::DoSetSubmeshProperties( wxSubmeshTreeItemData * p_data )
	{
		GeometrySPtr l_geometry = p_data->GetGeometry();
		Append( DoBuildMaterialProperty( _( "Material" ), l_geometry->GetScene()->GetEngine() ) )->SetValue( wxVariant( l_geometry->GetMaterial( p_data->GetSubmesh() )->GetName() ) );
	}

	void wxPropertiesHolder::DoSetLightProperties( wxLightTreeItemData * p_data )
	{
		Colour l_colour = Colour::from_rgba( p_data->GetLight()->GetAmbient() );
		Append( new wxColourProperty( _( "Ambient" ) ) )->SetValue( wxVariant( wxColour( l_colour.to_bgr() ) ) );
		l_colour = Colour::from_rgba( p_data->GetLight()->GetDiffuse() );
		Append( new wxColourProperty( _( "Diffuse" ) ) )->SetValue( wxVariant( wxColour( l_colour.to_bgr() ) ) );
		l_colour = Colour::from_rgba( p_data->GetLight()->GetSpecular() );
		Append( new wxColourProperty( _( "Specular" ) ) )->SetValue( wxVariant( wxColour( l_colour.to_bgr() ) ) );

		switch ( p_data->GetLight()->GetLightType() )
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			DoSetDirectionalLightProperties( std::static_pointer_cast< DirectionalLight >( p_data->GetLight()->GetLightCategory() ) );
			break;

		case eLIGHT_TYPE_POINT:
			DoSetPointLightProperties( std::static_pointer_cast< PointLight >( p_data->GetLight()->GetLightCategory() ) );
			break;

		case eLIGHT_TYPE_SPOT:
			DoSetSpotLightProperties( std::static_pointer_cast< SpotLight >( p_data->GetLight()->GetLightCategory() ) );
			break;
		}
	}

	void wxPropertiesHolder::DoSetNodeProperties( wxNodeTreeItemData * p_data )
	{
		Append( new Point3rProperty( _( "Position" ) ) )->SetValue( wxVariant( p_data->GetNode()->GetPosition() ) );
		Append( new Point3rProperty( _( "Scale" ) ) )->SetValue( wxVariant( p_data->GetNode()->GetScale() ) );
		Append( new QuaternionProperty( _( "Orientation" ) ) )->SetValue( wxVariant( p_data->GetNode()->GetOrientation() ) );
	}

	void wxPropertiesHolder::DoSetOverlayProperties( wxOverlayTreeItemData * p_data )
	{
		Append( new PositionProperty( _( "Position" ) ) )->SetValue( wxVariant( p_data->GetOverlay()->GetPixelPosition() ) );
		Append( new SizeProperty( _( "Size" ) ) )->SetValue( wxVariant( p_data->GetOverlay()->GetPixelSize() ) );
		Append( DoBuildMaterialProperty( _( "Material" ), p_data->GetOverlay()->GetEngine() ) )->SetValue( wxVariant( p_data->GetOverlay()->GetMaterial()->GetName() ) );

		switch ( p_data->GetOverlay()->GetType() )
		{
		case eOVERLAY_TYPE_PANEL:
			DoSetPanelOverlayProperties( std::static_pointer_cast< PanelOverlay >( p_data->GetOverlay()->GetOverlayCategory() ) );
			break;

		case eOVERLAY_TYPE_BORDER_PANEL:
			DoSetBorderPanelOverlayProperties( std::static_pointer_cast< BorderPanelOverlay >( p_data->GetOverlay()->GetOverlayCategory() ) );
			break;

		case eOVERLAY_TYPE_TEXT:
			DoSetTextOverlayProperties( std::static_pointer_cast< TextOverlay >( p_data->GetOverlay()->GetOverlayCategory() ) );
			break;
		}
	}

	void wxPropertiesHolder::DoSetMaterialProperties( wxMaterialTreeItemData * p_data )
	{
	}

	void wxPropertiesHolder::DoSetPassProperties( wxPassTreeItemData * p_data )
	{
		Append( new wxColourProperty( _( "Ambient" ) ) )->SetValue( wxVariant( wxColour( p_data->GetPass()->GetAmbient().to_bgr() ) ) );
		Append( new wxColourProperty( _( "Diffuse" ) ) )->SetValue( wxVariant( wxColour( p_data->GetPass()->GetDiffuse().to_bgr() ) ) );
		Append( new wxColourProperty( _( "Specular" ) ) )->SetValue( wxVariant( wxColour( p_data->GetPass()->GetSpecular().to_bgr() ) ) );
		Append( new wxColourProperty( _( "Emissive" ) ) )->SetValue( wxVariant( wxColour( p_data->GetPass()->GetEmissive().to_bgr() ) ) );
		Append( new wxFloatProperty( _( "Exponent" ) ) )->SetValue( p_data->GetPass()->GetShininess() );
		Append( new wxBoolProperty( _( "Two sided" ) ) )->SetValue( p_data->GetPass()->IsTwoSided() );
		Append( new wxFloatProperty( _( "Opacity" ) ) )->SetValue( p_data->GetPass()->GetAlpha() );
	}

	void wxPropertiesHolder::DoSetTextureProperties( wxTextureTreeItemData * p_data )
	{
		Append( new wxImageFileProperty( _( "Texture" ) ) )->SetValue( p_data->GetTexture()->GetTexturePath() );

		wxEnumProperty * l_channel = new wxEnumProperty( _( "Channel" ) );
		wxPGChoices l_choices;
		l_choices.Add( _( "Colour" ) ); //	eTEXTURE_CHANNEL_COLOUR		= 0x00000001	//!< Colour map
		l_choices.Add( _( "Diffuse" ) ); //	eTEXTURE_CHANNEL_DIFFUSE	= 0x00000002	//!< Diffuse map
		l_choices.Add( _( "Normal" ) ); //	eTEXTURE_CHANNEL_NORMAL		= 0x00000004	//!< Normal map
		l_choices.Add( _( "Opacity" ) ); //	eTEXTURE_CHANNEL_OPACITY	= 0x00000008	//!< Opacity map
		l_choices.Add( _( "Specular" ) );//	eTEXTURE_CHANNEL_SPECULAR	= 0x00000010	//!< Specular map
		l_choices.Add( _( "Height" ) ); //	eTEXTURE_CHANNEL_HEIGHT		= 0x00000020	//!< Height map
		l_choices.Add( _( "Ambient" ) ); //	eTEXTURE_CHANNEL_AMBIENT	= 0x00000040	//!< Ambient map
		l_choices.Add( _( "Gloss" ) ); //	eTEXTURE_CHANNEL_GLOSS		= 0x00000080	//!< Gloss map
		wxString l_selected;

		switch ( p_data->GetTexture()->GetChannel() )
		{
		case eTEXTURE_CHANNEL_COLOUR:
			l_selected = _( "Colour" );
			break;

		case eTEXTURE_CHANNEL_DIFFUSE:
			l_selected = _( "Diffuse" );
			break;

		case eTEXTURE_CHANNEL_NORMAL:
			l_selected = _( "Normal" );
			break;

		case eTEXTURE_CHANNEL_OPACITY:
			l_selected = _( "Opacity" );
			break;

		case eTEXTURE_CHANNEL_SPECULAR:
			l_selected = _( "Specular" );
			break;

		case eTEXTURE_CHANNEL_HEIGHT:
			l_selected = _( "Height" );
			break;

		case eTEXTURE_CHANNEL_AMBIENT:
			l_selected = _( "Ambient" );
			break;

		case eTEXTURE_CHANNEL_GLOSS:
			l_selected = _( "Gloss" );
			break;
		}

		l_channel->SetChoices( l_choices );
		Append( l_channel )->SetValue( l_selected );
	}

	void wxPropertiesHolder::DoSetDirectionalLightProperties( DirectionalLightSPtr p_light )
	{
	}

	void wxPropertiesHolder::DoSetPointLightProperties( PointLightSPtr p_light )
	{
		Append( new Point3rProperty( _( "Attenuation" ) ) )->SetValue( wxVariant( p_light->GetAttenuation() ) );
	}

	void wxPropertiesHolder::DoSetSpotLightProperties( SpotLightSPtr p_light )
	{
		Append( new Point3rProperty( _( "Attenuation" ) ) )->SetValue( wxVariant( p_light->GetAttenuation() ) );
		Append( new wxFloatProperty( _( "Cut off" ) ) )->SetValue( p_light->GetCutOff() );
		Append( new wxFloatProperty( _( "Exponent" ) ) )->SetValue( p_light->GetExponent() );
	}

	void wxPropertiesHolder::DoSetPanelOverlayProperties( PanelOverlaySPtr p_overlay )
	{
	}

	void wxPropertiesHolder::DoSetBorderPanelOverlayProperties( BorderPanelOverlaySPtr p_overlay )
	{
		Point4i l_rec = p_overlay->GetBorderPixelSize();
		Append( new RectangleProperty( _( "Border Size" ) ) )->SetValue( wxVariant( l_rec ) );
		Append( DoBuildMaterialProperty( _( "Border Material" ), p_overlay->GetOverlay().GetEngine() ) )->SetValue( wxVariant( p_overlay->GetBorderMaterial()->GetName() ) );
	}

	void wxPropertiesHolder::DoSetTextOverlayProperties( TextOverlaySPtr p_overlay )
	{
		FontSPtr l_font = p_overlay->GetFont();
		wxFontInfo l_info( l_font->GetHeight() );
		l_info.FaceName( l_font->GetFaceName() );
		Append( new wxFontProperty( _( "Font" ) ) )->SetValue( wxVariant( wxFont( l_info ) ) );
		Append( new wxStringProperty( _( "Caption" ) ) )->SetValue( p_overlay->GetCaption() );
	}

	wxEnumProperty * wxPropertiesHolder::DoBuildMaterialProperty( wxString const & p_name, Engine * p_engine )
	{
		wxEnumProperty * l_material = new wxEnumProperty( p_name );
		MaterialManager & l_manager = p_engine->GetMaterialManager();
		wxPGChoices l_choices;
		l_manager.lock();

		for ( auto && l_pair: l_manager )
		{
			l_choices.Add( l_pair.first );
		}

		l_material->SetChoices( l_choices );
		l_manager.unlock();
		return l_material;
	}
}
