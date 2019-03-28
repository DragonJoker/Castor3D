/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_ANIMATED_OBJECT_GROUP_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_ANIMATED_OBJECT_GROUP_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		01/02/2016
	\version	0.8.0
	\~english
	\brief		AnimatedObjectGroup helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les AnimatedObjectGroup.
	*/
	class AnimatedObjectGroupTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_group		The target AnimatedObjectGroup
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_group		L"AnimatedObjectGroup cible
		 */
		AnimatedObjectGroupTreeItemProperty( bool p_editable, castor3d::AnimatedObjectGroupSPtr p_group );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~AnimatedObjectGroupTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the AnimatedObjectGroup
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'AnimatedObjectGroup
		 *\return		La valeur
		 */
		inline castor3d::AnimatedObjectGroupSPtr getGroup()
		{
			return m_group.lock();
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & p_event );

	private:
		castor3d::AnimatedObjectGroupWPtr m_group;
	};
}

#endif
