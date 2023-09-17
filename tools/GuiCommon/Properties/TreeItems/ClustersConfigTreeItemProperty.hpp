/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_ClustersConfigTreeItemProperty_H___
#define ___GuiCommon_ClustersConfigTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Render/Clustered/ClusteredModule.hpp>

namespace GuiCommon
{
	/**
	\~english
	\brief		Clusters configuration helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les configurations des clusters.
	*/
	class ClustersConfigTreeItemProperty
		: public TreeItemProperty
	{
	public:
		ClustersConfigTreeItemProperty( bool editable
			, castor3d::Engine * engine
			, castor3d::RenderTarget & target );

	private:
		void doCreateProperties( wxPGEditor * editor
			, wxPropertyGrid * grid )override;

	private:
		castor3d::RenderTarget & m_target;
	};
}

#endif
