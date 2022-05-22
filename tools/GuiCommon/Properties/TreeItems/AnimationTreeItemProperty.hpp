/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_ANIMATION_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_ANIMATION_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Animation helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les Animation.
	*/
	class AnimationTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	p_editable	Tells if the properties are modifiable.
		 *\param[in]	p_group		The target AnimatedObjectGroup.
		 *\param[in]	p_name		The target animation name.
		 *\param[in]	p_state		The target animation state.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables.
		 *\param[in]	p_group		L'AnimatedObjectGroup cible.
		 *\param[in]	p_name		Le nom de l'animation cible.
		 *\param[in]	p_state		L'état de l'animation cible.
		 */
		AnimationTreeItemProperty( castor3d::Engine * engine
			, bool editable
			, castor3d::AnimatedObjectGroup & group
			, castor::String const & name
			, castor3d::GroupAnimation const & anim );

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )override;

	private:
		castor3d::AnimatedObjectGroup & m_group;
		castor::String m_name;
		castor3d::GroupAnimation m_groupAnim;
	};
}

#endif
