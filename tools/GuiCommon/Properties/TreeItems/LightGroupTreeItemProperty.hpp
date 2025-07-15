/*
See LICENSE file in root folder
*/
#ifndef ___GC_LightGroupTreeItemProperty_H___
#define ___GC_LightGroupTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer, for Light.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour Light.
	*/
	class LightGroupTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	engine		The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	engine		Le moteur.
		 */
		LightGroupTreeItemProperty( bool editable, c3d::Engine * engine );

		void setData( c3d::LightGroup & data )noexcept
		{
			clearProperties();
			m_lightGroup = &data;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		c3d::LightGroup * m_lightGroup{};
	};
}

#endif
