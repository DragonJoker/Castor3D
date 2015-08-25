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
	namespace
	{
		static const wxString PROPERTY_TOPOLOGY = _( "Topology" );
		static const wxString PROPERTY_TOPOLOGY_POINTS = _( "Points" );
		static const wxString PROPERTY_TOPOLOGY_LINES = _( "Lines" );
		static const wxString PROPERTY_TOPOLOGY_LINE_LOOP = _( "Line Loop" );
		static const wxString PROPERTY_TOPOLOGY_LINE_STRIP = _( "Line Strip" );
		static const wxString PROPERTY_TOPOLOGY_TRIANGLES = _( "Triangles" );
		static const wxString PROPERTY_TOPOLOGY_TRIANGLE_STRIP = _( "Triangle Strip" );
		static const wxString PROPERTY_TOPOLOGY_TRIANGLE_FAN = _( "Triangle Fan" );
		static const wxString PROPERTY_TOPOLOGY_QUADS = _( "Quads" );
		static const wxString PROPERTY_TOPOLOGY_QUAD_STRIP = _( "Quad Strip" );
		static const wxString PROPERTY_TOPOLOGY_POLYGON = _( "Polygon" );
		static const wxString PROPERTY_VIEWPORT = _( "Viewport" );
		static const wxString PROPERTY_VIEWPORT_3D = _( "3D" );
		static const wxString PROPERTY_VIEWPORT_2D = _( "2D" );
		static const wxString PROPERTY_CAMERA_SIZE = _( "Size" );
		static const wxString PROPERTY_SUBMESH_MATERIAL = _( "Material" );
		static const wxString PROPERTY_LIGHT_AMBIENT = _( "Ambient" );
		static const wxString PROPERTY_LIGHT_DIFFUSE = _( "Diffuse" );
		static const wxString PROPERTY_LIGHT_SPECULAR = _( "Specular" );
		static const wxString PROPERTY_LIGHT_ATTENUATION = _( "Attenuation" );
		static const wxString PROPERTY_LIGHT_CUT_OFF = _( "Cut off" );
		static const wxString PROPERTY_LIGHT_EXPONENT = _( "Exponent" );
		static const wxString PROPERTY_NODE_POSITION = _( "Position" );
		static const wxString PROPERTY_NODE_SCALE = _( "Scale" );
		static const wxString PROPERTY_NODE_ORIENTATION = _( "Orientation" );
		static const wxString PROPERTY_OVERLAY_POSITION = _( "Position" );
		static const wxString PROPERTY_OVERLAY_SIZE = _( "Size" );
		static const wxString PROPERTY_OVERLAY_MATERIAL = _( "Material" );
		static const wxString PROPERTY_OVERLAY_BORDER_SIZE = _( "Border Size" );
		static const wxString PROPERTY_OVERLAY_BORDER_MATERIAL = _( "Border Material" );
		static const wxString PROPERTY_OVERLAY_BORDER_INNER_UV = _( "Border Inner UV" );
		static const wxString PROPERTY_OVERLAY_BORDER_OUTER_UV = _( "Border Outer UV" );
		static const wxString PROPERTY_OVERLAY_BORDER_POSITION = _( "Border Position" );
		static const wxString PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL = _( "Internal" );
		static const wxString PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE = _( "Middle" );
		static const wxString PROPERTY_OVERLAY_BORDER_POSITION_EXTERNAL = _( "External" );
		static const wxString PROPERTY_OVERLAY_FONT = _( "Font" );
		static const wxString PROPERTY_OVERLAY_CAPTION = _( "Caption" );
		static const wxString PROPERTY_PASS_AMBIENT = _( "Ambient" );
		static const wxString PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		static const wxString PROPERTY_PASS_SPECULAR = _( "Specular" );
		static const wxString PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		static const wxString PROPERTY_PASS_EXPONENT = _( "Exponent" );
		static const wxString PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		static const wxString PROPERTY_PASS_OPACITY = _( "Opacity" );
		static const wxString PROPERTY_TEXTURE_IMAGE = _( "Image" );
		static const wxString PROPERTY_CHANNEL = _( "Channel" );
		static const wxString PROPERTY_CHANNEL_COLOUR = _( "Colour" );
		static const wxString PROPERTY_CHANNEL_DIFFUSE = _( "Diffuse" );
		static const wxString PROPERTY_CHANNEL_NORMAL = _( "Normal" );
		static const wxString PROPERTY_CHANNEL_OPACITY = _( "Opacity" );
		static const wxString PROPERTY_CHANNEL_SPECULAR = _( "Specular" );
		static const wxString PROPERTY_CHANNEL_HEIGHT = _( "Height" );
		static const wxString PROPERTY_CHANNEL_AMBIENT = _( "Ambient" );
		static const wxString PROPERTY_CHANNEL_GLOSS = _( "Gloss" );
	}

	wxPropertiesHolder::wxPropertiesHolder( bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxPropertyGrid( p_pParent, wxID_ANY, p_ptPos, p_size, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE )
		, m_bCanEdit( p_bCanEdit )
		, m_data( NULL )
		, m_currentHandler( NULL )
	{
		SetBackgroundColour( *wxWHITE );
	}

	wxPropertiesHolder::~wxPropertiesHolder()
	{
	}

	void wxPropertiesHolder::SetPropertyData( wxTreeItemPropertyData * p_data )
	{
		if ( m_data )
		{
			Disconnect( wxEVT_PG_CHANGED, m_currentHandler );
			wxPropertyGrid::Clear();
			m_currentHandler = NULL;
		}

		m_data = p_data;

		if ( m_data )
		{
			switch ( m_data->GetType() )
			{
			case ePROPERTY_DATA_TYPE_CAMERA:
				DoSetCameraProperties( reinterpret_cast< wxCameraTreeItemData * >( m_data ) );
				break;

			case ePROPERTY_DATA_TYPE_GEOMETRY:
				DoSetGeometryProperties( reinterpret_cast< wxGeometryTreeItemData * >( m_data ) );
				break;

			case ePROPERTY_DATA_TYPE_SUBMESH:
				DoSetSubmeshProperties( reinterpret_cast< wxSubmeshTreeItemData * >( m_data ) );
				break;

			case ePROPERTY_DATA_TYPE_LIGHT:
				DoSetLightProperties( reinterpret_cast< wxLightTreeItemData * >( m_data ) );
				break;

			case ePROPERTY_DATA_TYPE_NODE:
				DoSetNodeProperties( reinterpret_cast< wxNodeTreeItemData * >( m_data ) );
				break;

			case ePROPERTY_DATA_TYPE_OVERLAY:
				DoSetOverlayProperties( reinterpret_cast< wxOverlayTreeItemData * >( m_data ) );
				break;

			case ePROPERTY_DATA_TYPE_MATERIAL:
				DoSetMaterialProperties( reinterpret_cast< wxMaterialTreeItemData * >( m_data ) );
				break;

			case ePROPERTY_DATA_TYPE_PASS:
				DoSetPassProperties( reinterpret_cast< wxPassTreeItemData * >( m_data ) );
				break;

			case ePROPERTY_DATA_TYPE_TEXTURE:
				DoSetTextureProperties( reinterpret_cast< wxTextureTreeItemData * >( m_data ) );
				break;
			}

			if ( m_currentHandler )
			{
				Connect( wxEVT_PG_CHANGED, m_currentHandler );
			}
		}
	}

	void wxPropertiesHolder::DoSetCameraProperties( wxCameraTreeItemData * p_data )
	{
		wxPGChoices l_topologies;
		l_topologies.Add( PROPERTY_TOPOLOGY_POINTS );
		l_topologies.Add( PROPERTY_TOPOLOGY_LINES );
		l_topologies.Add( PROPERTY_TOPOLOGY_LINE_LOOP );
		l_topologies.Add( PROPERTY_TOPOLOGY_LINE_STRIP );
		l_topologies.Add( PROPERTY_TOPOLOGY_TRIANGLES );
		l_topologies.Add( PROPERTY_TOPOLOGY_TRIANGLE_STRIP );
		l_topologies.Add( PROPERTY_TOPOLOGY_TRIANGLE_FAN );
		l_topologies.Add( PROPERTY_TOPOLOGY_QUADS );
		l_topologies.Add( PROPERTY_TOPOLOGY_QUAD_STRIP );
		l_topologies.Add( PROPERTY_TOPOLOGY_POLYGON );
		wxString l_selected;

		switch ( p_data->GetCamera()->GetPrimitiveType() )
		{
		case eTOPOLOGY_POINTS:
			l_selected = PROPERTY_TOPOLOGY_POINTS;
			break;

		case eTOPOLOGY_LINES:
			l_selected = PROPERTY_TOPOLOGY_LINES;
			break;

		case eTOPOLOGY_LINE_LOOP:
			l_selected = PROPERTY_TOPOLOGY_LINE_LOOP;
			break;

		case eTOPOLOGY_LINE_STRIP:
			l_selected = PROPERTY_TOPOLOGY_LINE_STRIP;
			break;

		case eTOPOLOGY_TRIANGLES:
			l_selected = PROPERTY_TOPOLOGY_TRIANGLES;
			break;

		case eTOPOLOGY_TRIANGLE_STRIPS:
			l_selected = PROPERTY_TOPOLOGY_TRIANGLE_STRIP;
			break;

		case eTOPOLOGY_TRIANGLE_FAN:
			l_selected = PROPERTY_TOPOLOGY_TRIANGLE_FAN;
			break;

		case eTOPOLOGY_QUADS:
			l_selected = PROPERTY_TOPOLOGY_QUADS;
			break;

		case eTOPOLOGY_QUAD_STRIPS:
			l_selected = PROPERTY_TOPOLOGY_QUAD_STRIP;
			break;

		case eTOPOLOGY_POLYGON:
			l_selected = PROPERTY_TOPOLOGY_POLYGON;
			break;
		}

		Append( new wxEnumProperty( PROPERTY_TOPOLOGY, PROPERTY_TOPOLOGY, l_topologies ) )->SetValue( l_selected );

		wxPGChoices l_viewports;
		l_viewports.Add( PROPERTY_VIEWPORT_3D );
		l_viewports.Add( PROPERTY_VIEWPORT_3D );

		switch ( p_data->GetCamera()->GetViewportType() )
		{
		case eVIEWPORT_TYPE_3D:
			l_selected = PROPERTY_VIEWPORT_3D;
			break;

		case eVIEWPORT_TYPE_2D:
			l_selected = PROPERTY_VIEWPORT_2D;
			break;
		}

		Append( new wxEnumProperty( PROPERTY_VIEWPORT, PROPERTY_VIEWPORT, l_viewports ) )->SetValue( l_selected );
		Append( new SizeProperty( PROPERTY_CAMERA_SIZE ) )->SetValue( wxVariant( Size( p_data->GetCamera()->GetWidth(), p_data->GetCamera()->GetHeight() ) ) );
		m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnCameraPropertyChanged );
	}

	void wxPropertiesHolder::DoSetGeometryProperties( wxGeometryTreeItemData * p_data )
	{
		m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnGeometryPropertyChanged );
	}

	void wxPropertiesHolder::DoSetSubmeshProperties( wxSubmeshTreeItemData * p_data )
	{
		GeometrySPtr l_geometry = p_data->GetGeometry();
		Append( DoBuildMaterialProperty( PROPERTY_SUBMESH_MATERIAL, l_geometry->GetScene()->GetEngine() ) )->SetValue( wxVariant( l_geometry->GetMaterial( p_data->GetSubmesh() )->GetName() ) );
		m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnSubmeshPropertyChanged );
	}

	void wxPropertiesHolder::DoSetLightProperties( wxLightTreeItemData * p_data )
	{
		Colour l_colour = Colour::from_rgba( p_data->GetLight()->GetAmbient() );
		Append( new wxColourProperty( PROPERTY_LIGHT_AMBIENT ) )->SetValue( wxVariant( wxColour( l_colour.to_bgr() ) ) );
		l_colour = Colour::from_rgba( p_data->GetLight()->GetDiffuse() );
		Append( new wxColourProperty( PROPERTY_LIGHT_DIFFUSE ) )->SetValue( wxVariant( wxColour( l_colour.to_bgr() ) ) );
		l_colour = Colour::from_rgba( p_data->GetLight()->GetSpecular() );
		Append( new wxColourProperty( PROPERTY_LIGHT_SPECULAR ) )->SetValue( wxVariant( wxColour( l_colour.to_bgr() ) ) );

		switch ( p_data->GetLight()->GetLightType() )
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			DoSetDirectionalLightProperties( std::static_pointer_cast< DirectionalLight >( p_data->GetLight()->GetLightCategory() ) );
			m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnDirectionalLightPropertyChanged );
			break;

		case eLIGHT_TYPE_POINT:
			DoSetPointLightProperties( std::static_pointer_cast< PointLight >( p_data->GetLight()->GetLightCategory() ) );
			m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnPointLightPropertyChanged );
			break;

		case eLIGHT_TYPE_SPOT:
			DoSetSpotLightProperties( std::static_pointer_cast< SpotLight >( p_data->GetLight()->GetLightCategory() ) );
			m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnSpotLightPropertyChanged );
			break;
		}
	}

	void wxPropertiesHolder::DoSetNodeProperties( wxNodeTreeItemData * p_data )
	{
		Append( new Point3rProperty( PROPERTY_NODE_POSITION ) )->SetValue( wxVariant( p_data->GetNode()->GetPosition() ) );
		Append( new Point3rProperty( PROPERTY_NODE_SCALE ) )->SetValue( wxVariant( p_data->GetNode()->GetScale() ) );
		Append( new QuaternionProperty( PROPERTY_NODE_ORIENTATION ) )->SetValue( wxVariant( p_data->GetNode()->GetOrientation() ) );
		m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnNodePropertyChanged );
	}

	void wxPropertiesHolder::DoSetOverlayProperties( wxOverlayTreeItemData * p_data )
	{
		Append( new PositionProperty( PROPERTY_OVERLAY_POSITION ) )->SetValue( wxVariant( p_data->GetOverlay()->GetPixelPosition() ) );
		Append( new SizeProperty( PROPERTY_OVERLAY_SIZE ) )->SetValue( wxVariant( p_data->GetOverlay()->GetPixelSize() ) );
		Append( DoBuildMaterialProperty( PROPERTY_OVERLAY_MATERIAL, p_data->GetOverlay()->GetEngine() ) )->SetValue( wxVariant( p_data->GetOverlay()->GetMaterial()->GetName() ) );

		switch ( p_data->GetOverlay()->GetType() )
		{
		case eOVERLAY_TYPE_PANEL:
			DoSetPanelOverlayProperties( std::static_pointer_cast< PanelOverlay >( p_data->GetOverlay()->GetOverlayCategory() ) );
			m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnPanelOverlayPropertyChanged );
			break;

		case eOVERLAY_TYPE_BORDER_PANEL:
			DoSetBorderPanelOverlayProperties( std::static_pointer_cast< BorderPanelOverlay >( p_data->GetOverlay()->GetOverlayCategory() ) );
			m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnBorderPanelOverlayPropertyChanged );
			break;

		case eOVERLAY_TYPE_TEXT:
			DoSetTextOverlayProperties( std::static_pointer_cast< TextOverlay >( p_data->GetOverlay()->GetOverlayCategory() ) );
			m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnTextOverlayPropertyChanged );
			break;
		}
	}

	void wxPropertiesHolder::DoSetMaterialProperties( wxMaterialTreeItemData * p_data )
	{
		m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnMaterialPropertyChanged );
	}

	void wxPropertiesHolder::DoSetPassProperties( wxPassTreeItemData * p_data )
	{
		Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( wxVariant( wxColour( p_data->GetPass()->GetAmbient().to_bgr() ) ) );
		Append( new wxColourProperty( PROPERTY_PASS_AMBIENT ) )->SetValue( wxVariant( wxColour( p_data->GetPass()->GetDiffuse().to_bgr() ) ) );
		Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( wxVariant( wxColour( p_data->GetPass()->GetSpecular().to_bgr() ) ) );
		Append( new wxColourProperty( PROPERTY_PASS_EMISSIVE ) )->SetValue( wxVariant( wxColour( p_data->GetPass()->GetEmissive().to_bgr() ) ) );
		Append( new wxFloatProperty( PROPERTY_PASS_EXPONENT ) )->SetValue( p_data->GetPass()->GetShininess() );
		Append( new wxBoolProperty( PROPERTY_PASS_TWO_SIDED, wxPG_BOOL_USE_CHECKBOX ) )->SetValue( p_data->GetPass()->IsTwoSided() );
		Append( new wxFloatProperty( PROPERTY_PASS_OPACITY ) )->SetValue( p_data->GetPass()->GetAlpha() );
		m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnPassPropertyChanged );
	}

	void wxPropertiesHolder::DoSetTextureProperties( wxTextureTreeItemData * p_data )
	{
		wxPGChoices l_choices;
		l_choices.Add( PROPERTY_CHANNEL_COLOUR );
		l_choices.Add( PROPERTY_CHANNEL_DIFFUSE );
		l_choices.Add( PROPERTY_CHANNEL_NORMAL );
		l_choices.Add( PROPERTY_CHANNEL_OPACITY );
		l_choices.Add( PROPERTY_CHANNEL_SPECULAR );
		l_choices.Add( PROPERTY_CHANNEL_HEIGHT );
		l_choices.Add( PROPERTY_CHANNEL_AMBIENT );
		l_choices.Add( PROPERTY_CHANNEL_GLOSS );
		wxString l_selected;

		switch ( p_data->GetTexture()->GetChannel() )
		{
		case eTEXTURE_CHANNEL_COLOUR:
			l_selected = PROPERTY_CHANNEL_COLOUR;
			break;

		case eTEXTURE_CHANNEL_DIFFUSE:
			l_selected = PROPERTY_CHANNEL_DIFFUSE;
			break;

		case eTEXTURE_CHANNEL_NORMAL:
			l_selected = PROPERTY_CHANNEL_NORMAL;
			break;

		case eTEXTURE_CHANNEL_OPACITY:
			l_selected = PROPERTY_CHANNEL_OPACITY;
			break;

		case eTEXTURE_CHANNEL_SPECULAR:
			l_selected = PROPERTY_CHANNEL_SPECULAR;
			break;

		case eTEXTURE_CHANNEL_HEIGHT:
			l_selected = PROPERTY_CHANNEL_HEIGHT;
			break;

		case eTEXTURE_CHANNEL_AMBIENT:
			l_selected = PROPERTY_CHANNEL_AMBIENT;
			break;

		case eTEXTURE_CHANNEL_GLOSS:
			l_selected = PROPERTY_CHANNEL_GLOSS;
			break;
		}

		Append( new wxImageFileProperty( PROPERTY_TEXTURE_IMAGE ) )->SetValue( p_data->GetTexture()->GetTexturePath() );
		Append( new wxEnumProperty( PROPERTY_CHANNEL, PROPERTY_CHANNEL, l_choices ) )->SetValue( l_selected );
		m_currentHandler = wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, wxPropertiesHolder::OnTexturePropertyChanged );
	}

	void wxPropertiesHolder::DoSetDirectionalLightProperties( DirectionalLightSPtr p_light )
	{
	}

	void wxPropertiesHolder::DoSetPointLightProperties( PointLightSPtr p_light )
	{
		Append( new Point3rProperty( PROPERTY_LIGHT_ATTENUATION ) )->SetValue( wxVariant( p_light->GetAttenuation() ) );
	}

	void wxPropertiesHolder::DoSetSpotLightProperties( SpotLightSPtr p_light )
	{
		Append( new Point3rProperty( PROPERTY_LIGHT_ATTENUATION ) )->SetValue( wxVariant( p_light->GetAttenuation() ) );
		Append( new wxFloatProperty( PROPERTY_LIGHT_CUT_OFF ) )->SetValue( p_light->GetCutOff() );
		Append( new wxFloatProperty( PROPERTY_LIGHT_EXPONENT ) )->SetValue( p_light->GetExponent() );
	}

	void wxPropertiesHolder::DoSetPanelOverlayProperties( PanelOverlaySPtr p_overlay )
	{
	}

	void wxPropertiesHolder::DoSetBorderPanelOverlayProperties( BorderPanelOverlaySPtr p_overlay )
	{
		wxPGChoices l_choices;
		l_choices.Add( PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL );
		l_choices.Add( PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE );
		l_choices.Add( PROPERTY_OVERLAY_BORDER_POSITION_EXTERNAL );
		wxString l_selected;

		switch ( p_overlay->GetBorderPosition() )
		{
		case eBORDER_POSITION_INTERNAL:
			l_selected = PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL;
			break;

		case eBORDER_POSITION_MIDDLE:
			l_selected = PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE;
			break;

		case eBORDER_POSITION_EXTERNAL:
			l_selected = PROPERTY_OVERLAY_BORDER_POSITION_EXTERNAL;
			break;
		}

		Point4i l_rec = p_overlay->GetBorderPixelSize();
		Append( new RectangleProperty( PROPERTY_OVERLAY_BORDER_SIZE ) )->SetValue( wxVariant( l_rec ) );
		Append( DoBuildMaterialProperty( PROPERTY_OVERLAY_BORDER_MATERIAL, p_overlay->GetOverlay().GetEngine() ) )->SetValue( wxVariant( p_overlay->GetBorderMaterial()->GetName() ) );
		Append( new Point4dProperty( PROPERTY_OVERLAY_BORDER_INNER_UV ) )->SetValue( wxVariant( p_overlay->GetBorderInnerUV() ) );
		Append( new Point4dProperty( PROPERTY_OVERLAY_BORDER_OUTER_UV ) )->SetValue( wxVariant( p_overlay->GetBorderOuterUV() ) );
		Append( new wxEnumProperty( PROPERTY_OVERLAY_BORDER_POSITION, PROPERTY_OVERLAY_BORDER_POSITION, l_choices ) )->SetValue( l_selected );
	}

	void wxPropertiesHolder::DoSetTextOverlayProperties( TextOverlaySPtr p_overlay )
	{
		FontSPtr l_font = p_overlay->GetFont();
		wxFontInfo l_info( l_font->GetHeight() );
		l_info.FaceName( l_font->GetFaceName() );
		Append( new wxFontProperty( PROPERTY_OVERLAY_FONT ) )->SetValue( wxVariant( wxFont( l_info ) ) );
		Append( new wxStringProperty( PROPERTY_OVERLAY_CAPTION ) )->SetValue( p_overlay->GetCaption() );
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

	void wxPropertiesHolder::OnCameraPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxCameraTreeItemData * l_data = static_cast< wxCameraTreeItemData * >( m_data );
		}
	}

	void wxPropertiesHolder::OnGeometryPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxGeometryTreeItemData * l_data = static_cast< wxGeometryTreeItemData * >( m_data );
		}
	}

	void wxPropertiesHolder::OnSubmeshPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxSubmeshTreeItemData * l_data = static_cast< wxSubmeshTreeItemData * >( m_data );
		}
	}

	void wxPropertiesHolder::OnDirectionalLightPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxLightTreeItemData * l_data = static_cast< wxLightTreeItemData * >( m_data );
		}
	}

	void wxPropertiesHolder::OnPointLightPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxLightTreeItemData * l_data = static_cast< wxLightTreeItemData * >( m_data );
		}
	}

	void wxPropertiesHolder::OnSpotLightPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxLightTreeItemData * l_data = static_cast< wxLightTreeItemData * >( m_data );
		}
	}

	void wxPropertiesHolder::OnNodePropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxNodeTreeItemData * l_data = static_cast< wxNodeTreeItemData * >( m_data );
		}
	}

	void wxPropertiesHolder::OnPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxOverlayTreeItemData * l_data = static_cast< wxOverlayTreeItemData * >( m_data );

			if ( l_property->GetName() == PROPERTY_OVERLAY_POSITION )
			{
				const Position & l_position = PositionRefFromVariant( l_property->GetValue() );
				l_data->OnPositionChange( l_position );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_SIZE )
			{
				const Size & l_size = SizeRefFromVariant( l_property->GetValue() );
				l_data->OnSizeChange( l_size );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_MATERIAL )
			{
				l_data->OnMaterialChange( String( l_property->GetValueAsString().c_str() ) );
			}
		}
	}

	void wxPropertiesHolder::OnBorderPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxOverlayTreeItemData * l_data = static_cast< wxOverlayTreeItemData * >( m_data );

			if ( l_property->GetName() == PROPERTY_OVERLAY_POSITION )
			{
				const Position & l_position = PositionRefFromVariant( l_property->GetValue() );
				l_data->OnPositionChange( l_position );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_SIZE )
			{
				const Size & l_size = SizeRefFromVariant( l_property->GetValue() );
				l_data->OnSizeChange( l_size );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_MATERIAL )
			{
				l_data->OnMaterialChange( String( l_property->GetValueAsString().c_str() ) );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_SIZE )
			{
				const Point4i & l_size = Point4iRefFromVariant( l_property->GetValue() );
				l_data->OnBorderSizeChange( Castor::Rectangle( l_size[0], l_size[1], l_size[2], l_size[3] ) );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_MATERIAL )
			{
				l_data->OnBorderMaterialChange( String( l_property->GetValueAsString().c_str() ) );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_POSITION )
			{
				if ( l_property->GetValueAsString() == PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL )
				{
					l_data->OnBorderPositionChange( eBORDER_POSITION_INTERNAL );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE )
				{
					l_data->OnBorderPositionChange( eBORDER_POSITION_MIDDLE );
				}
				else
				{
					l_data->OnBorderPositionChange( eBORDER_POSITION_EXTERNAL );
				}
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_INNER_UV )
			{
				l_data->OnBorderInnerUVChange( Point4d( l_property->GetValue() ) );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_OUTER_UV )
			{
				l_data->OnBorderOuterUVChange( Point4d( l_property->GetValue() ) );
			}
		}
	}

	void wxPropertiesHolder::OnTextOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxOverlayTreeItemData * l_data = static_cast< wxOverlayTreeItemData * >( m_data );

			if ( l_property->GetName() == PROPERTY_OVERLAY_POSITION )
			{
				const Position & l_position = PositionRefFromVariant( l_property->GetValue() );
				l_data->OnPositionChange( l_position );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_SIZE )
			{
				const Size & l_size = SizeRefFromVariant( l_property->GetValue() );
				l_data->OnSizeChange( l_size );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_MATERIAL )
			{
				l_data->OnMaterialChange( String( l_property->GetValueAsString().c_str() ) );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_FONT )
			{
				wxFont l_wxfont;
				l_wxfont << l_property->GetValue();
				FontSPtr l_font = wxLoadFont( l_data->GetOverlay()->GetEngine(), l_wxfont );

				if ( l_font )
				{
					l_data->OnFontChange( l_font );
				}
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_CAPTION )
			{
				l_data->OnCaptionChange( String( l_property->GetValueAsString().c_str() ) );
			}
		}
	}

	void wxPropertiesHolder::OnMaterialPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxMaterialTreeItemData * l_data = static_cast< wxMaterialTreeItemData * >( m_data );
		}
	}

	void wxPropertiesHolder::OnPassPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxPassTreeItemData * l_data = static_cast< wxPassTreeItemData * >( m_data );
		}
	}

	void wxPropertiesHolder::OnTexturePropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			wxTextureTreeItemData * l_data = static_cast< wxTextureTreeItemData * >( m_data );
		}
	}
}
