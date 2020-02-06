#include "PropertiesHolder.hpp"

#include "CameraTreeItemProperty.hpp"
#include "GeometryTreeItemProperty.hpp"
#include "LightTreeItemProperty.hpp"
#include "NodeTreeItemProperty.hpp"
#include "OverlayTreeItemProperty.hpp"
#include "SubmeshTreeItemProperty.hpp"
#include "MaterialTreeItemProperty.hpp"
#include "PassTreeItemProperty.hpp"
#include "TextureTreeItemProperty.hpp"

#include "AdditionalProperties.hpp"

#include <Engine.hpp>
#include <Cache/MaterialCache.hpp>

#include <Material/Pass.hpp>
#include <Mesh/Submesh.hpp>
#include <Overlay/Overlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/SceneNode.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Texture/TextureUnit.hpp>

#include <Graphics/Font.hpp>

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	wxPGEditor * PropertiesHolder::m_buttonEditor = nullptr;

	PropertiesHolder::PropertiesHolder( bool p_bCanEdit, wxWindow * p_parent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxPropertyGrid( p_parent, wxID_ANY, p_ptPos, p_size, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE )
		, m_bCanEdit( p_bCanEdit )
		, m_data( nullptr )
	{
		if ( !m_buttonEditor )
		{
			m_buttonEditor = RegisterEditorClass( new ButtonEventEditor() );
		}

		Connect( wxEVT_PG_CHANGED, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, PropertiesHolder::OnPropertyChange ) );
	}

	PropertiesHolder::~PropertiesHolder()
	{
	}

	void PropertiesHolder::SetPropertyData( TreeItemProperty * p_data )
	{
		if ( m_data )
		{
			wxPropertyGrid::Clear();
		}

		m_data = p_data;

		if ( m_data )
		{
			m_data->CreateProperties( m_buttonEditor, this );
		}
	}

	void PropertiesHolder::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		if ( m_data )
		{
			m_data->OnPropertyChange( p_event );
		}
	}
}
