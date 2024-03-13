/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_ColourGradingConfigTreeItemProperty_H___
#define ___GuiCommon_ColourGradingConfigTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Render/ToneMapping/ColourGradingConfig.hpp>

namespace GuiCommon
{
	/**
	\~english
	\brief		Colour grading configuration helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les configurations de colour grading.
	*/
	class ColourGradingConfigTreeItemProperty
		: public TreeItemProperty
	{
	public:
		ColourGradingConfigTreeItemProperty( bool editable
			, castor3d::Engine * engine
			, castor3d::ColourGradingConfig & config );

	private:
		void doCreateProperties( wxPropertyGrid * grid )override;
		void doCreateSsaoProperties( wxPropertyGrid * grid );

	private:
		castor3d::ColourGradingConfig & m_config;
	};
}

#endif
