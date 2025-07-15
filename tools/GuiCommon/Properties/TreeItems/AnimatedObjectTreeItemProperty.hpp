/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_ANIMATED_OBJECT_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_ANIMATED_OBJECT_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
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
		 *\param[in]	object		The target AnimatedObject.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur.
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	object		L"AnimatedObject cible.
		 */
		AnimatedObjectTreeItemProperty( c3d::Engine * engine
			, bool editable
			, c3d::AnimatedObjectRPtr object );
		/**
		 *\~english
		 *\brief		Retrieves the AnimatedObject.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'AnimatedObject.
		 *\return		La valeur.
		 */
		inline c3d::AnimatedObjectRPtr getObject()
		{
			return m_object;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		c3d::AnimatedObjectRPtr m_object{};
	};
}

#endif
