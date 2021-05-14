#include "GuiCommon/Properties/PropertiesContainer.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/TreeItems/CameraTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/ExportOptionsTreeItemProperty.hpp"
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

	PropertiesContainer::PropertiesContainer( bool canEdit
		, wxWindow * parent
		, wxPoint const & pos
		, wxSize const & size )
		: wxPropertyGrid( parent, wxID_ANY, pos, size, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE )
		, m_canEdit( canEdit )
		, m_data( nullptr )
	{
		if ( !m_buttonEditor )
		{
			m_buttonEditor = RegisterEditorClass( new ButtonEventEditor() );
		}

		Connect( wxEVT_PG_CHANGED, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, PropertiesContainer::onPropertyChange ) );
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		SetCaptionBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetCaptionTextColour( PANEL_FOREGROUND_COLOUR );
		SetSelectionBackgroundColour( ACTIVE_TAB_COLOUR );
		SetSelectionTextColour( ACTIVE_TEXT_COLOUR );
		SetCellBackgroundColour( INACTIVE_TAB_COLOUR );
		SetCellTextColour( INACTIVE_TEXT_COLOUR );
		SetLineColour( BORDER_COLOUR );
		SetMarginColour( BORDER_COLOUR );
	}

	PropertiesContainer::~PropertiesContainer()
	{
	}

	void PropertiesContainer::setPropertyData( TreeItemProperty * data )
	{
		if ( m_data )
		{
			wxPropertyGrid::Clear();
		}

		m_data = data;

		if ( m_data )
		{
			m_data->CreateProperties( m_buttonEditor, this );
		}
	}

	void PropertiesContainer::onPropertyChange( wxPropertyGridEvent & event )
	{
		if ( m_data )
		{
			m_data->onPropertyChange( event );
		}
	}
}
