#include "GuiCommon/Properties/TreeItems/ClustersConfigTreeItemProperty.hpp"

#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/Clustered/ClustersConfig.hpp>
#include <Castor3D/Render/Clustered/FrustumClusters.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	ClustersConfigTreeItemProperty::ClustersConfigTreeItemProperty( bool editable
		, castor3d::Engine * engine
		, castor3d::RenderTarget & target )
		: TreeItemProperty{ engine, editable }
		, m_target{ target }
	{
		CreateTreeItemMenu();
	}

	void ClustersConfigTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		TreeItemConfigurationBuilder::submit( grid, *this, m_target.getFrustumClusters().getConfig() );
	}
}
