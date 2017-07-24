/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GUICOMMON_ANIMATION_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_ANIMATION_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Animation helper class to communicate between Scene objects or Materials lists and PropertiesHolder.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les Animation.
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
		AnimationTreeItemProperty( Castor3D::Engine * engine, bool p_editable, Castor3D::AnimatedObjectGroupSPtr p_group, Castor::String const & p_name, Castor3D::GroupAnimation const & p_anim );
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
		inline Castor3D::AnimatedObjectGroupSPtr GetGroup()
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
		inline Castor::String const & GetName()
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
		inline Castor3D::AnimationState GetState()
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
		inline float GetScale()
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
		inline bool IsLooped()
		{
			return m_groupAnim.m_looped;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::DoCreateProperties
		 */
		virtual void DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::DoPropertyChange
		 */
		virtual void DoPropertyChange( wxPropertyGridEvent & p_event );

	private:
		void OnSpeedChange( double p_value );
		void OnLoopedChange( bool p_value );
		bool OnStateChange( wxPGProperty * p_property );

	private:
		Castor3D::AnimatedObjectGroupWPtr m_group;
		Castor::String m_name;
		Castor3D::GroupAnimation m_groupAnim;
	};
}

#endif
