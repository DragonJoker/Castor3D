/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_SsaoConfigTreeItemProperty_H___
#define ___GuiCommon_SsaoConfigTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Render/Ssao/SsaoModule.hpp>

namespace GuiCommon
{
	/**
	\~english
	\brief		SSAO configuration helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les configurations SSAO.
	*/
	class SsaoConfigTreeItemProperty
		: public TreeItemProperty
	{
	public:
		SsaoConfigTreeItemProperty( bool editable
			, castor3d::Engine * engine
			, castor3d::SsaoConfig & config );
		~SsaoConfigTreeItemProperty();

	private:
		void doCreateProperties( wxPGEditor * editor
			, wxPropertyGrid * grid )override;
		void doCreateSsaoProperties( wxPGEditor * editor
			, wxPropertyGrid * grid );

	private:
		castor3d::SsaoConfig & m_config;
	};
}

#endif
