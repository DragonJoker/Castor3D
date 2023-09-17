#include "GuiCommon/Properties/TreeItems/ClustersConfigTreeItemProperty.hpp"

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
		auto & config = m_target.getFrustumClusters().getConfig();

		if ( config.enabled )
		{
			addProperty( grid, _( "Clusters" ) );
			addPropertyT( grid, _( "Use BVH" ), &config.useLightsBVH );
			addPropertyT( grid, _( "Sort Lights" ), &config.sortLights );
			addPropertyT( grid, _( "Use Depth Buffer" ), &config.parseDepthBuffer );
		}
	}
}
