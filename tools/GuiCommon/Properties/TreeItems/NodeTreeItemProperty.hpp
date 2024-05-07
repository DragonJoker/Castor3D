/*
See LICENSE file in root folder
*/
#ifndef ___GC_NodeTreeItemProperty_H___
#define ___GC_NodeTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer, for SceneNode.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour SceneNode.
	*/
	class NodeTreeItemProperty
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
		NodeTreeItemProperty( bool editable, castor3d::Engine * engine );

		void setData( castor3d::SceneNode & data )noexcept
		{
			clearProperties();
			m_node = &data;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		castor3d::SceneNode * m_node{};
	};
}

#endif
