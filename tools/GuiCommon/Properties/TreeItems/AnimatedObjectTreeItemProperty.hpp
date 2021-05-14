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
		 *\param[in]	p_editable	Tells if the properties are modifiable.
		 *\param[in]	p_light		The target AnimatedObject.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur.
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables.
		 *\param[in]	p_light		L"AnimatedObject cible.
		 */
		AnimatedObjectTreeItemProperty( castor3d::Engine * engine, bool p_editable, castor3d::AnimatedObjectSPtr p_light );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~AnimatedObjectTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the AnimatedObject.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'AnimatedObject.
		 *\return		La valeur.
		 */
		inline castor3d::AnimatedObjectSPtr getObject()
		{
			return m_object.lock();
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );

	private:
		castor3d::AnimatedObjectWPtr m_object;
	};
}

#endif
