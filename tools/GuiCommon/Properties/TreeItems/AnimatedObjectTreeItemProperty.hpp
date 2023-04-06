/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_ANIMATED_OBJECT_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_ANIMATED_OBJECT_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		AnimatedObject helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les AnimatedObject.
	*/
	class AnimatedObjectTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	light		The target AnimatedObject.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur.
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	light		L"AnimatedObject cible.
		 */
		AnimatedObjectTreeItemProperty( castor3d::Engine * engine, bool editable, castor3d::AnimatedObjectSPtr light );
		/**
		 *\~english
		 *\brief		Retrieves the AnimatedObject.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'AnimatedObject.
		 *\return		La valeur.
		 */
		inline castor3d::AnimatedObjectRPtr getObject()
		{
			return m_object;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )override;

	private:
		castor3d::AnimatedObjectRPtr m_object{};
	};
}

#endif
