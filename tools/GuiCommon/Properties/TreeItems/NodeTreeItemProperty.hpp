/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_NODE_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_NODE_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Geometry helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les gàomàtries
	*/
	class NodeTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	engine	The engine
		 *\param[in]	p_node		The target object
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	engine	Le moteur
		 *\param[in]	p_node		L'objet cible
		 */
		NodeTreeItemProperty( bool p_editable, castor3d::Engine * engine, castor3d::SceneNode & p_node );
		/**
		 *\~english
		 *\brief		Retrieves the object
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'objet
		 *\return		La valeur
		 */
		inline castor3d::SceneNode & getNode()const
		{
			return m_node;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )override;

	private:
		castor3d::SceneNode & m_node;
	};
}

#endif
