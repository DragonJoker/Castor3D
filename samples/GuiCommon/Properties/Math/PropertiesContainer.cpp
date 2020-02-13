#include "GuiCommon/Properties/Math/PropertiesContainer.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/TreeItems/CameraTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/GeometryTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/LightTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/NodeTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/OverlayTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/SubmeshTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/MaterialTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/PassTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/TextureTreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Graphics/Font.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	wxPGEditor * PropertiesContainer::m_buttonEditor = nullptr;

	PropertiesContainer::PropertiesContainer( bool p_bCanEdit, wxWindow * p_parent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxPropertyGrid( p_parent, wxID_ANY, p_ptPos, p_size, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE )
		, m_bCanEdit( p_bCanEdit )
		, m_data( nullptr )
	{
		if ( !m_buttonEditor )
		{
			m_buttonEditor = RegisterEditorClass( new ButtonEventEditor() );
		}

		Connect( wxEVT_PG_CHANGED, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, PropertiesContainer::onPropertyChange ) );
	}

	PropertiesContainer::~PropertiesContainer()
	{
	}

	void PropertiesContainer::setPropertyData( TreeItemProperty * p_data )
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

	void PropertiesContainer::onPropertyChange( wxPropertyGridEvent & p_event )
	{
		if ( m_data )
		{
			m_data->onPropertyChange( p_event );
		}
	}
}
