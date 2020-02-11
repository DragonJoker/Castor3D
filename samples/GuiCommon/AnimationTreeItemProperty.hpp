/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_ANIMATION_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_ANIMATION_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/TreeItemProperty.hpp"

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
		AnimationTreeItemProperty( castor3d::Engine * engine, bool p_editable, castor3d::AnimatedObjectGroupSPtr p_group, castor::String const & p_name, castor3d::GroupAnimation const & p_anim );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~AnimationTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the AnimatedObjectGroup.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'AnimatedObjectGroup.
		 *\return		La valeur.
		 */
		inline castor3d::AnimatedObjectGroupSPtr getGroup()
		{
			return m_group.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation name.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le nom de l'animation.
		 *\return		La valeur.
		 */
		inline castor::String const & getName()
		{
			return m_name;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation state.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'état de l'animation.
		 *\return		La valeur.
		 */
		inline castor3d::AnimationState getState()
		{
			return m_groupAnim.m_state;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation state.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'état de l'animation.
		 *\return		La valeur.
		 */
		inline float getScale()
		{
			return m_groupAnim.m_scale;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation state.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'état de l'animation.
		 *\return		La valeur.
		 */
		inline bool isLooped()
		{
			return m_groupAnim.m_looped;
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
		void OnSpeedChange( double p_value );
		void OnLoopedChange( bool p_value );
		bool OnStateChange( wxPGProperty * p_property );

	private:
		castor3d::AnimatedObjectGroupWPtr m_group;
		castor::String m_name;
		castor3d::GroupAnimation m_groupAnim;
	};
}

#endif
